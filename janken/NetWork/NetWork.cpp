#include <fstream>
#include <string>
#include "NetWork.h"
#include "HostState.h"
#include "GuestState.h"
#include "../_debug/_DebugConOut.h"
#include "../TiledLoader.h"

void NetWork::newUpdate(void)
{
	while (ProcessMessage() == 0)
	{
		if (network_state_ != nullptr)
		{
			network_state_->CheckNetWork();
		}
	}
}

void NetWork::Update(void)
{
	mut_ = std::thread(&NetWork::newUpdate, this);
	mut_.detach();	//join もある。ほとんどjoin
}

bool NetWork::SetNetWorkMode(NetWorkMode nwmode)
{
	switch (nwmode)
	{
	case NetWorkMode::OFFLINE:
		network_state_ = std::make_unique<NetWorkState>();
		break;
	case NetWorkMode::HOST:
		network_state_ = std::make_unique<HostState>();
		break;
	case NetWorkMode::GUEST:
		network_state_ = std::make_unique<GuestState>();
		break;
	case NetWorkMode::NON:
		network_state_.reset();
		break;
	default:
		TRACE("セットするNetWorkModeが異常値");
		break;
	}
	return false;
}
void NetWork::SetRevStandby(bool rev)
{
	revStandby_ = rev;
}
MesPacket NetWork::SendMesHeader(MesHeader data)
{
	 MesPacket mesdata;
	 Header head;
	 head.header = data;
	 mesdata.emplace_back(head.uniheader[0]);
	 mesdata.emplace_back(head.uniheader[1]);
		
	 return mesdata;
}

bool NetWork::SendMesData(MesType type, MesPacket data,int handle)
{
	if (network_state_ == nullptr)
	{
		return false;
	}
	header = { type,0,0,0 };
	size_ = 0;
	do
	{
		for (int i = 0;data.size() > i && i < MaxCnt; i++)
		{
			tmpmesdata_.emplace_back(data[i]);
			size_++;
		}
		header.header.length = size_;

		data.erase(data.begin(), data.begin() + size_);
		if (data.size() > 0) {
			header.header.next = 1;
		}
		else { header.header.next = 0; }
		tmpmesdata_[0].iData = header.iheader[0];
		tmpmesdata_[1].iData = header.iheader[1];
		header.header.sendid++;
		
		NetWorkSend(handle, tmpmesdata_.data(), tmpmesdata_.size() * sizeof(int));
		
		tmpmesdata_.erase(tmpmesdata_.begin() + 2, tmpmesdata_.end());
		size_ = 0;
	} while (header.header.next);

	return true;
}

bool NetWork::SendMesData(MesType type)
{
	for (auto& handle : handlist_)
	{
		MesPacket data;
		SendMesData(type, std::move(data), handle.first);
	}
	return true;
}

bool NetWork::SendMesAll(MesType type, MesPacket data)
{
	return false;
}

void NetWork::SendStandby(void)
{
	if (network_state_ == nullptr)
	{
		return;
	}
	if (network_state_->GetActive() == ActiveState::Init)
	{
		//MesHeader data = { MesType::STANBY,0,0,0};
		//NetWorkSend(lpNetWork.GetNetWorkHandle(), &data, sizeof(MesHeader));
		SendMesData(MesType::STANBY_HOST);
		TRACE("スタンバイok　初期化情報をゲストに送るよ\n");
		network_state_->SetActive(ActiveState::Standby);
	}
}
void NetWork::SendStart(void)
{
	if (network_state_ == nullptr)
	{
		return;
	}
	if (network_state_->GetActive() == ActiveState::Matching)
	{
		network_state_->SetActive(ActiveState::Play);
		SendMesData(MesType::STNBY_GUEST);

		TRACE("スタンバイok　開始していいよってホストに送るよ\n");
	}
}
void NetWork::SendTmxSize(void)
{
	TmxData tmx = lpTiledLoader.ReadTmx("Tiled/mapdata/map");

	unionData uni;
	uni.cData[0] = std::atoi(tmx.num["width"].c_str());			// たて
	uni.cData[1] = std::atoi(tmx.num["height"].c_str());		// よこ
	uni.cData[2] = std::atoi(tmx.num["nextlayerid"].c_str())-1;	// レイヤー数
	uni.cData[3] = 0;											// リザーブ
	
	SendMesAll(MesType::TMX_SIZE,{uni});
	return;
}

std::array<IPDATA,5> NetWork::GetIP(void)
{
	GetMyIPAddress(mipdata_.data(),5);
	return	mipdata_;
}

NetWorkMode NetWork::GetNetWorkMode(void)
{
	if (network_state_ == nullptr)
	{
		return NetWorkMode::NON;
	}
	return network_state_->GetMode();
}

ActiveState NetWork::GetActive(void)
{
	if (network_state_ == nullptr)
	{
		return ActiveState::Non;
	}
	return network_state_->GetActive();
}

int NetWork::GetNetWorkHandle(void)
{
	if (network_state_ == nullptr)
	{
		return -1;
	}
	return network_state_->GetNetWorkHandle();
}

bool NetWork::GetRevStandby(void)
{
	return revStandby_;
}

int NetWork::GetMaxByte(void)
{
	return maxByte_;
}

ActiveState NetWork::ConnectHost(IPDATA hostip)
{
	if (network_state_ == nullptr)
	{
		return ActiveState::Non;
	}
	return network_state_->ConnectHost(hostip);
}

void NetWork::AddMesList(int id, MesList& list, std::mutex& mtx)
{
	if (network_state_ == nullptr)
	{
		return;
	}
	network_state_->SetPlayerList(id,list,mtx);
}

chronoi NetWork::TimeStart(void)
{
	if (network_state_ == nullptr)
	{
		chronoi tmp{ std::chrono::system_clock::now() };
		return tmp;
	}
	return network_state_->TimeStart();
}

void NetWork::SetListID(void)
{
	int playermax_ = handlist_.size() + 1;

	unionData data[2];
	data[0].iData = 0;
	data[1].iData = playermax_;

	int id = 5;
	for (auto& handlist : handlist_)
	{
		handlist.second = id;
		id += 5;
	}
	for (auto hand : handlist_)
	{
		data[0].iData = hand.second;
		SendMesData(MesType::ID,{ data[0],data[1]}, hand.first);
	}
}

ListInt NetWork::GetListID(void)
{
	return handlist_;
}

std::pair<int, int> NetWork::PlayerID(void)
{
	if (network_state_ == nullptr)
	{
		return {-1,-1};
	}
	return network_state_->PlayerID();
}

void NetWork::AddList(std::pair<int, unsigned int> add)
{
	handlist_.emplace_back(add);
}

void NetWork::RemoveList(int lost)
{
	handlist_.remove_if([&](std::pair<int, unsigned int> hl) {return hl.first == lost;});
}

int NetWork::ListSize(void)
{
	return handlist_.size();
}

bool NetWork::Setting(void)
{
	maxByte_ = 0;
	tmpmesdata_.resize(2);
	size_ = 0;
	MaxCnt = maxByte_ / sizeof(int) - (sizeof(MesHeader) / sizeof(int));
	std::ifstream ifs("ini/setting.txt");
	std::string str;
	getline(ifs,str);
	if (str.find_first_of("=\"") != std::string::basic_string::npos)
	{
		int one = str.find_first_of("\"")+1;
		int two = str.find_last_of("\"");
		int three = two - one;
		maxByte_ = atoi(str.substr(one,three).c_str());
		return true;
	}
	return false;
}

NetWork::NetWork()
{
	revStandby_ = false;
	mipdata_ = {};
	Setting();
}

NetWork::~NetWork()
{
}
