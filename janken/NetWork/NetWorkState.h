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
	char cData[8];
	int iData[2];
	long long lData;
};

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
//	std::vector<int> revtmx_;
};