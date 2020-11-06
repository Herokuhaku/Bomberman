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
	mut_.detach();	//join ������B�قƂ��join
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
		TRACE("�Z�b�g����NetWorkMode���ُ�l");
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
		TRACE("%d �𑗐M\n",d);
	}
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
		MesHeader data = { MesType::STANBY,0,0,0};
		NetWorkSend(lpNetWork.GetNetWorkHandle(), &data, sizeof(MesHeader));
		TRACE("�X�^���o�Cok�@�����������Q�X�g�ɑ����\n");
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
		MesHeader data = { MesType::GAME_START,0,0,0};
		NetWorkSend(lpNetWork.GetNetWorkHandle(), &data, sizeof(MesHeader));
		TRACE("�X�^���o�Cok�@�J�n���Ă�������ăz�X�g�ɑ����\n");
	}
}
void NetWork::SendTmxSize(void)
{
	std::ifstream ifp("Tiled/mapdata/map.tmx");
	ifp.seekg(0, std::ios::end);		// �Ō�܂ŃV�[�N����

	// ��	~1400�܂�0 �ɂȂ邩�炻���1�ɂ��邽��+1
	// 1401~ 2800 �܂ł� 1 + 1 �ŉ�2
	MesSizeData tmpd;

	MesData mesdata = SendMesHeader({ MesType::TMX_SIZE,0,0,sizeof(tmpd.AllByte) / 4 });
	tmpd.AllByte = (((357 * 4) * 4) / 8);
	tmpd.times = tmpd.AllByte / MAXSENDBYTE + 1;
	tmpd.oneByte = tmpd.AllByte / tmpd.times;

	//mesdata.emplace_back(tmpd.times);
	//// �P�̃o�C�g��
	//mesdata.emplace_back(tmpd.oneByte);
	//// ���o�C�g��
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
	revStandby_ = false;
	mipdata_ = {};
}

NetWork::~NetWork()
{
}
