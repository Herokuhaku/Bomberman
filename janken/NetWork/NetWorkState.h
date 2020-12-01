#pragma once
#include <DxLib.h>
#include <vector>
#include <mutex>
#include <map>
#include <utility>
#include <list>
#include <functional>
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
	Matching,
	Play,		// �Q�[����(�z�X�g/�Q�X�g)
	//Instance,	// �C���X�^���X��
	OFFLINE,
};

union unionData
{
	char cData[4];
	int iData;
	unsigned int uiData;
	//long long lData;
};

union chronoi
{
	std::chrono::system_clock::time_point now;
	unsigned int inow[2];
	unionData uninow[2];
};

using ListInt = std::list<std::pair<int, unsigned int>>;	// playerid�ƃl�b�g���[�N�n���h��
enum class MesType :unsigned char
{
	NON = 100,
	COUNT_DOWN_ROOM,	// �ڑ���t�J�E���g�_�E��
	ID,					// ������ID�ƃv���C���[����
	STANBY_HOST,		// ��������񑗐M����(�z�X�g�p)
	STNBY_GUEST,		// �z�X�g����̏��������ł̏���������,�Q�[���J�n(�Q�X�g�p)
	COUNT_DOWN_GAME,	// �S���̏�����������̃Q�[���J�n����
	TMX_SIZE,			// TMX�T�C�Y�@���T�C�Y
	TMX_DATA,			// TMX�f�[�^�@CSV�̂ݐ؂�����,���O��������
	POS,				// �Q�[�����ɑ���
	SET_BOMB,			// �{���̔z�u
	DEATH,				// ���S		�ǂ̃L�������킩��悤��ID�����t�^����
	LOST,				// �ڑ����ɐ���(�z�X�g�͎����̃l�b�g���[�N�L�����ɂ��Z�b�g����)
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
	unionData uniheader[2];
};

struct MesSizeData
{
	unsigned int now;
	unsigned int oneByte;
	unsigned int AllByte;
};

using MesPacket = std::vector<unionData>;
using SavePacket = std::pair<MesType, MesPacket>;
//using SavePacket = std::vector<int>;
using MesList = std::vector<SavePacket>;		// Obj�S�ʂ̏��

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
	virtual void SetPlayerList(int id, MesList& list, std::mutex& mtx);
	virtual chronoi TimeStart(void);
	virtual std::pair<int, int> PlayerID(void);
	void OutCsv(void);
	void OutData(void);
	std::list<int> DeathList(void);
protected:

	const int portNum_ = 8086;
	ActiveState active_;
	int networkHandle_ = 0;		// dxlib�̃l�b�g���[�N�n���h��

	std::mutex mtx_;
	MesPacket revtmx;			// �󂯎��pbox(tmx)
	//MesSizeData sizedata_;		// �T�C�Y�f�[�^
	std::vector<std::pair<MesList&,std::mutex&>> revlist;	// �󂯎��pbox Player�p
	MesList reset;

	chronoi timestart_;		// ���ԕϊ��p
	std::pair<int, int> player;	// ID��Max

	std::map<MesType, std::function<bool(MesHeader,MesPacket,int&)>> MesTypeList_;

	std::map<std::string, int> num;

	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;

	std::list<int> deathlist_;
};