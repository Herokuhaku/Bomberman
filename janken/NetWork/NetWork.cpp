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
	std::vector<unionData> tmp;
	for (auto& j : data)
	{
		unionData t;
		t.iData = j;
		tmp.emplace_back(t);
	}
	//for (auto& d : data)
	//{
	//	TRACE("%d �𑗐M\n",d);
	//}
	return true;
}
bool NetWork::SendMesData(MesType type, MesData data)
{
	if (network_state_ == nullptr)
	{
		return false;
	}
	//TRACE("MesType : %d\n", static_cast<int>(type));
	MesData mesdata = SendMesHeader({ type,0,0,0 });
	Header header = {type,0,0,0};
	auto hSize = sizeof(MesHeader) / sizeof(int);
	auto MaxCnt = maxByte_ / sizeof(int) - hSize;
	bool headerf = false;
	if (data.size() <= 0)headerf = true;
	while (data.size() > MaxCnt)
	{
		for (int i = 0; i < MaxCnt; i++)
		{
			mesdata.emplace_back(data[i]);
		}
		// ���M���f�[�^�̍폜���s��
		// MaxByte /4 ��int�̐��ɂ���header����-2 �������B
		data.erase(data.begin(),data.begin() + MaxCnt);
		if (data.size() > 0) {
			header.header.next = 1;
		}
		else { header.header.next = 0; }
		header.header.length = MaxCnt;	// �w�b�_�[���������f�[�^���̐�
		mesdata[0] = header.iheader[0];
		mesdata[1] = header.iheader[1];
		header.header.sendid++;

		SendMesData(mesdata);
		// ���M�f�[�^�̍폜�@�w�b�_�[�͏����Ȃ��Ŏ��ɉ�
		// �J�n�ʒu��header�̏I���ʒu����@�@�I���ʒu��header�̏I���ʒu + int�̐� - �w�b�_�[��; ����Ńw�b�_�[���������ɑ�����������������
		mesdata.erase(mesdata.begin() + 2, mesdata.begin() + 2 + MaxCnt);
	}

	// �f�[�^�Ɏc�肪�Ȃ���΂��̂܂ܔ�����@�c�肪����Ύc���mes�f�[�^�ɋl�߂đ���
	if (data.size() > 0 || headerf)
	{
		// unionData�������header ������������ // length��data�̃T�C�Y��
		mesdata = SendMesHeader({ type,0,header.header.sendid,static_cast<unsigned int>(data.size()) });
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
		SendMesData(MesType::GAME_START);
		//MesHeader data = { MesType::GAME_START,0,0,0};
		//NetWorkSend(lpNetWork.GetNetWorkHandle(), &data, sizeof(MesHeader));
		TRACE("�X�^���o�Cok�@�J�n���Ă�������ăz�X�g�ɑ����\n");
	}
}
void NetWork::SendTmxSize(void)
{
	std::ifstream ifp("Tiled/mapdata/map.tmx");
	ifp.seekg(0, std::ios::end);		// �Ō�܂ŃV�[�N����
	TmxData tmx = lpTiledLoader.ReadTmx("Tiled/mapdata/map");
	// ��	~1400�܂�0 �ɂȂ邩�炻���1�ɂ��邽��+1
	// 1401~ 2800 �܂ł� 1 + 1 �ŉ�2
	MesSizeData tmpd;

	MesData mesdata = SendMesHeader({ MesType::TMX_SIZE,0,0,sizeof(tmpd.AllByte) / 4 });
	tmpd.AllByte = (((357 * 4) * 4) / 8);
	tmpd.times = tmpd.AllByte / MAXSENDBYTE + 1;
	tmpd.oneByte = tmpd.AllByte / tmpd.times;
	unionData uni;
	uni.cData[0] = std::atoi(tmx.num["width"].c_str());			// ����
	uni.cData[1] = std::atoi(tmx.num["height"].c_str());		// �悱
	uni.cData[2] = std::atoi(tmx.num["nextlayerid"].c_str())-1;	// ���C���[��
	uni.cData[3] = 0;											// ���U�[�u
	
																
	//uni.iData = uni.cData[0] * uni.cData[1] * uni.cData[2];
	
	
	//mesdata.emplace_back(tmpd.times);
	//// �P�̃o�C�g��
	//mesdata.emplace_back(tmpd.oneByte);
	//// ���o�C�g��
//	uni.iData /= 8;
//	if (uni.iData % 8 != 0) { uni.iData++; }
	mesdata.emplace_back(uni.iData);
	//mesdata.emplace_back(tmpd.AllByte/4);

	SendMesData(mesdata);
	return ;
}
MesData NetWork::TakeOutRevData(int no)
{
	if (network_state_ == nullptr)
	{
		MesData data;
		data.insert(data.begin(),-1);
		return data;
	}
	return network_state_->GetPosdata(no);
}
std::vector<unionData> NetWork::TakeOutRevData(void)
{
	if (network_state_ == nullptr)
	{
		std::vector<unionData> data;
		unionData i;
		i.iData = -1;
		data.insert(data.begin(), i);
		return data;
	}
	return network_state_->GetRevdata();
}
int NetWork::RevPosSize(void)
{
	return network_state_->RevPosSize();
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
