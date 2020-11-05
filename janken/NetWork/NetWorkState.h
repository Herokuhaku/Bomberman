#pragma once
#include <DxLib.h>
#include <vector>
#include <mutex>
#include "../_debug/_DebugConOut.h"

enum class NetWorkMode
{
	NON,
	OFFLINE,
	HOST,
	GUEST,
	MAX
};

enum class ActiveState
{
	Non,		// ���ݒ�
	Wait,		// �ڑ��ҋ@��(�z�X�g�p)
	Init,		// ��������(�Q�[���J�n������)(�z�X�g/�Q�X�g)
	Standby,	// ��������񑗐M�ς݂̊J�n�҂�(�z�X�g�p)
	Play,		// �Q�[����(�z�X�g/�Q�X�g)
	OFFLINE,
};

union unionData
{
	char cData[4];
	int iData;
	//long long lData;
};

enum class MesType :unsigned char
{
	STANBY,			// ��������񑗐M����(�z�X�g�p)
	GAME_START,		// �z�X�g����̏��������ł̏���������,�Q�[���J�n(�Q�X�g�p)
	TMX_SIZE,
	TMX_DATA,
	POS,
	NON
};


struct MesHeader
{
	MesType type;
	unsigned char cdata;
	unsigned short id;
	int length;
};

union Header
{
	MesHeader header;
	int iheader[2];
};

struct MesSizeData
{
	unsigned int times;
	unsigned int oneByte;
	unsigned int AllByte;
};

using MesData = std::vector<int>;

class NetWorkState
{
public:
	NetWorkState();
	virtual	~NetWorkState();
	virtual NetWorkMode GetMode(void) { return NetWorkMode::OFFLINE; };
	ActiveState GetActive(void);
	bool SetActive(ActiveState act);
	virtual ActiveState ConnectHost(IPDATA hostip);
	virtual int GetNetWorkHandle(void);
	virtual void SetNetWorkHandle(int nethandle);
	virtual bool CheckNetWork(void);
protected:
	const int portNum_ = 8086;
	ActiveState active_;
	int networkHandle_ = 0;		// dxlib�̃l�b�g���[�N�n���h��

	std::mutex mtx_;
	std::vector<unionData> revtmx;
//	MesType nowtype_;
	MesSizeData sizedata_;
//	std::vector<int> revtmx_;
};