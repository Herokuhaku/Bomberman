#pragma once
#include <Dxlib.h>
#include <memory>
#include <array>
#include <vector>
#include <mutex>
#include <thread>
#include "NetWorkState.h"

#define lpNetWork NetWork::GetInstance()
#define MAXSENDBYTE 400

class NetWork
{
public:
	static NetWork& GetInstance()
	{
		Create();
		return *sInstance;
	}
	static void Create()
	{
		if (sInstance == nullptr)
		{
			sInstance = new NetWork;
		}
	}
	static void Destroy()
	{
		if (sInstance != nullptr)
		{
			delete sInstance;
		}
		sInstance = nullptr;
	}
	void newUpdate(void);
	void Update(void);

	bool SetNetWorkMode(NetWorkMode data);
	void SetRevStandby(bool rev);

	MesPacket SendMesHeader(MesHeader data);
	bool SendMesData(MesType type, MesPacket data,int handle);
	bool SendMesData(MesType type);
	bool SendMesAll(MesType type,MesPacket data);
	void SendStandby(void);
	void SendStart(void);
	void SendTmxSize(void);
	bool GetNetWorkState(void);
	
	//IPDATA GetIP(void);		// ipdata�̒��g��Ԃ�l�Ƃ��ĕԂ�
	std::array<IPDATA, 5> GetIP(void);
	NetWorkMode GetNetWorkMode(void);
	ActiveState GetActive(void);
	void SetActive(ActiveState active);
	int GetNetWorkHandle(void);
	bool GetRevStandby(void);
	int GetMaxByte(void);
	std::list<int> GetDeathNote(void);

	ActiveState ConnectHost(IPDATA hostip);
	void AddMesList(int id,MesList&,std::mutex& mtx);
	void DeleteMesList();
	void DeleteNetWorkState();

	chronoi TimeStart(void);
	
	void SetListID(void);
	ListInt GetListID(void);
	std::pair<int, int> PlayerID(void);
	void AddList(std::pair<int,unsigned int> add);
	void RemoveList(int lost);
	int ListSize(void);
	int StanbyCountUp(int num);
	std::pair<int unsigned,int> GetListIdFront();
private:
	bool Setting(void);

	std::thread mut_;
	bool revStandby_;
//	IPDATA ipdata_;			// IP�̕ۑ�
	std::array<IPDATA, 5> mipdata_;
	std::unique_ptr<NetWorkState> network_state_;	//net work state
	int maxByte_;


	// SendMes�ő���f�[�^�̐錾�@��������͑����� 
	MesPacket tmpmesdata_;
	int size_;
	Header header;
	unsigned long long MaxCnt;
	ListInt handlist_;
	int StanbyCount_;
	bool delflag;
	//
	NetWork();
	~NetWork();

	static NetWork* sInstance;
};

