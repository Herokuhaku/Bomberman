#include <fstream>
#include <string>
#include "NetWork.h"
#include "HostState.h"
#include "GuestState.h"
#include "../_debug/_DebugConOut.h"

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
bool NetWork::SendMes(MesHeader data)
{
	NetWorkSend(lpNetWork.GetNetWorkHandle(), &data, sizeof(MesHeader));
	//TRACE("種類 : %d,ID : %d,data[0] : %d, data[1] : %d を送信\n\n",data.type,data.id, data.data[0], data.data[1]);
	return true;
}
void NetWork::SendStandby(void)
{
	if (network_state_ == nullptr)
	{
		return;
	}
	if (network_state_->GetActive() == ActiveState::Init)
	{
		MesHeader data = { MesType::STANBY,0,0 };
		NetWorkSend(lpNetWork.GetNetWorkHandle(), &data, sizeof(MesHeader));
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
	if (network_state_->GetActive() == ActiveState::Init)
	{
		network_state_->SetActive(ActiveState::Play);
		MesHeader data = { MesType::GAME_START,0,0 };
		NetWorkSend(lpNetWork.GetNetWorkHandle(), &data, sizeof(MesHeader));
		TRACE("スタンバイok　開始していいよってホストに送るよ\n");
	}
}
void NetWork::SendTmxSize(void)
{
	std::ifstream ifp("Tiled/mapdata/map.tmx");
	// 行数を数える
	std::string save;
	int i = 0;
	int a = 0;
	while (std::getline(ifp, save))
	{
		i+=static_cast<int>(save.size())+1;
	}
	MesHeader data = { MesType::TMX_SIZE,0,0,0};

	SendMes(data);
	//std::ifstream ifp("Tiled/mapdata/map.tmx");
	//ifp.seekg(0, std::ios::end);		// 最後までシークする
	//
	//// 行数を数える
	//std::ifstream fp("Tiled/mapdata/map.tmx");
	//std::string save;
	//int i = 0;
	//while (std::getline(fp, save))
	//{
	//	i++;
	//}
	// i = static_cast<int>(ifp.tellg()) - i;
	//MesData data = {MesType::TMX_SIZE,i,0};
	//SendMes(data);
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

ActiveState NetWork::ConnectHost(IPDATA hostip)
{
	if (network_state_ == nullptr)
	{
		return ActiveState::Non;
	}
	return network_state_->ConnectHost(hostip);
}

NetWork::NetWork()
{
	ipdata_ = {};
	revStandby_ = false;
}

NetWork::~NetWork()
{
}
