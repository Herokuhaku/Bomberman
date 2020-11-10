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
 std::vector<int> NetWork::SendMesHeader(MesHeader data)
{
	 MesData mesdata;
	 Header head;
	 head.header = data;
	 mesdata.emplace_back(head.iheader[0]);
	 mesdata.emplace_back(head.iheader[1]);
		
	 return mesdata;
}
bool NetWork::SendMesData(MesData data)
{
	NetWorkSend(lpNetWork.GetNetWorkHandle(),data.data(),data.size()*sizeof(int));
	int count = 0;
	for (auto& d : data)
	{
		TRACE("%d を送信\n",d);
	}
	return true;
}
bool NetWork::SendMesData(MesType type, MesData data)
{
	if (network_state_ == nullptr)
	{
		return false;
	}
	TRACE("MesType : %d", static_cast<int>(type));
	MesData mesdata = SendMesHeader({ type,0,0,0 });
	Header header = {type,0,0,0};
	auto hSize = sizeof(MesHeader) / sizeof(int);
	auto MaxCnt = maxByte_ / sizeof(int) - hSize;
	while (data.size() > MaxCnt)
	{
		for (int i = 0; i < MaxCnt; i++)
		{
			mesdata.emplace_back(data[i]);
		}
		// 送信元データの削除を行う
		// MaxByte /4 でintの数にしてheader分の-2 を消す。
		data.erase(data.begin(),data.begin() + MaxCnt);
		if (data.size() > 0) {
			header.header.next = 1;
		}
		else { header.header.next = 0; }
		header.header.length = MaxCnt;
		mesdata[0] = header.iheader[0];
		mesdata[1] = header.iheader[1];
		header.header.sendid++;

		SendMesData(mesdata);
		// 送信データの削除　ヘッダーは消さないで次に回す
		// 開始位置はheaderの終わり位置から　　終了位置はheaderの終わり位置 + intの数 - ヘッダー分; これでヘッダーを消さずに送った分だけ消せる
		mesdata.erase(mesdata.begin() + 2, mesdata.begin() + 2 + MaxCnt);
	}

	// データに残りがなければそのまま抜ける　残りがあれば残りをmesデータに詰めて送る
	if (data.size() > 0)
	{
		// unionDataを作ってheader を書き換える // lengthはdataのサイズ分
		MesData tmpl = SendMesHeader({ type,0,header.header.sendid,static_cast<unsigned int>(data.size())});
		mesdata[0] = tmpl[0];
		mesdata[1] = tmpl[1];
		int c = 0;
		for (auto d : data)
		{
			mesdata.emplace_back(d);
			c++;
		}
		data.erase(data.begin(), data.begin() + c);
		SendMesData(mesdata);
	}
	return true;
}
bool NetWork::SendMesData(MesType type)
{
	MesData data;
	SendMesData(type, std::move(data));
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
		//MesHeader data = { MesType::STANBY,0,0,0};
		//NetWorkSend(lpNetWork.GetNetWorkHandle(), &data, sizeof(MesHeader));
		SendMesData(MesType::STANBY);
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
		SendMesData(MesType::GAME_START);
		//MesHeader data = { MesType::GAME_START,0,0,0};
		//NetWorkSend(lpNetWork.GetNetWorkHandle(), &data, sizeof(MesHeader));
		TRACE("スタンバイok　開始していいよってホストに送るよ\n");
	}
}
void NetWork::SendTmxSize(void)
{
	std::ifstream ifp("Tiled/mapdata/map.tmx");
	ifp.seekg(0, std::ios::end);		// 最後までシークする

	// 回数	~1400まで0 になるからそれを1にするため+1
	// 1401~ 2800 までは 1 + 1 で回数2
	MesSizeData tmpd;

	MesData mesdata = SendMesHeader({ MesType::TMX_SIZE,0,0,sizeof(tmpd.AllByte) / 4 });
	tmpd.AllByte = (((357 * 4) * 4) / 8);
	tmpd.times = tmpd.AllByte / MAXSENDBYTE + 1;
	tmpd.oneByte = tmpd.AllByte / tmpd.times;

	//mesdata.emplace_back(tmpd.times);
	//// 単体バイト数
	//mesdata.emplace_back(tmpd.oneByte);
	//// 総バイト数
	mesdata.emplace_back(tmpd.AllByte/4);

	SendMesData(mesdata);
	return ;
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

bool NetWork::Setting(void)
{
	maxByte_ = 0;
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
