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
		static NetWork sInstance;
		return sInstance;
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
	
	//IPDATA GetIP(void);		// ipdataの中身を返り値として返す
	std::array<IPDATA, 5> GetIP(void);
	NetWorkMode GetNetWorkMode(void);
	ActiveState GetActive(void);
	int GetNetWorkHandle(void);
	bool GetRevStandby(void);
	int GetMaxByte(void);

	ActiveState ConnectHost(IPDATA hostip);
	void AddMesList(int id,MesList&,std::mutex& mtx);
	chronoi TimeStart(void);
	
	void SetListID(void);
	ListInt GetListID(void);
	std::pair<int, int> PlayerID(void);
	void AddList(std::pair<int,unsigned int> add);
	void RemoveList(int lost);
	int ListSize(void);
	int StanbyCountUp(int num);
private:
	bool Setting(void);

	std::thread mut_;
	bool revStandby_;
//	IPDATA ipdata_;			// IPの保存
	std::array<IPDATA, 5> mipdata_;
	std::unique_ptr<NetWorkState> network_state_;	//net work state
	int maxByte_;


	// SendMesで送るデータの宣言　毎回作るよりは早そう 
	MesPacket tmpmesdata_;
	int size_;
	Header header;
	unsigned long long MaxCnt;
	ListInt handlist_;
	int StanbyCount_;
	//
	NetWork();
	~NetWork();
};

