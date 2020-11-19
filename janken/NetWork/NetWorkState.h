#pragma once
#include <DxLib.h>
#include <vector>
#include <mutex>
#include <map>
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
	//Instance,	// �C���X�^���X��
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
	NON = 100,
	STANBY,			// ��������񑗐M����(�z�X�g�p)
	GAME_START,		// �z�X�g����̏��������ł̏���������,�Q�[���J�n(�Q�X�g�p)
	TMX_SIZE,		// TMX�T�C�Y�@���T�C�Y
	TMX_DATA,		// TMX�f�[�^�@CSV�̂ݐ؂�����,���O��������
	POS,			// �Q�[�����ɑ���
	SET_BOMB,		
	MAX
};


struct MesHeader
{
	MesType type;				// ���b�Z�[�W�̎��
	unsigned char next;			// �f�[�^���M�������̏ꍇ�ɁA,  �����Ȃ��@0 ��������@1 
	unsigned short sendid;		// �������M���̃i���o�����O
	unsigned int length;		// �������ǂ����͊֌W�Ȃ��P��p�P�b�g�̃f�[�^��(int�̐�) �o�C�g��/4
};

union Header
{
	MesHeader header;
	unsigned int iheader[2];
	int uheader[2];
};

struct MesSizeData
{
	unsigned int times;
	unsigned int oneByte;
	unsigned int AllByte;
};

using MesData = std::vector<int>;
using MesPacket = std::vector<unionData>;
using MesList = std::vector<std::pair<MesType, MesPacket>>;

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
	MesData GetPosdata(int no);
	int RevPosSize(void);
protected:
	const int portNum_ = 8086;
	ActiveState active_;
	int networkHandle_ = 0;		// dxlib�̃l�b�g���[�N�n���h��

	std::mutex mtx_;
//	std::vector<unionData> revtmx;
//	MesType nowtype_;
	MesSizeData sizedata_;
	//std::vector<unionData> revdata_;

	std::pair<MesType, std::vector<unionData>> revtmx_;
	MesList revlist_;
	std::map<int,std::vector<int>> posdata_;
};