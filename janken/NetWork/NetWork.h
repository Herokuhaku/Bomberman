#pragma once
#include <Dxlib.h>
#include <memory>
#include <array>
#include <vector>
#include <mutex>
#include <thread>
#include "NetWorkState.h"

#define lpNetWork NetWork::GetInstance()
#define MAXSENDBYTE 1000

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

	std::vector<int> SendMesHeader(MesHeader data);
	bool SendMesData(MesData data);
	void SendStandby(void);
	void SendStart(void);
	void SendTmxSize(void);

	//IPDATA GetIP(void);		// ipdataの中身を返り値として返す
	std::array<IPDATA, 5> GetIP(void);
	NetWorkMode GetNetWorkMode(void);
	ActiveState GetActive(void);
	int GetNetWorkHandle(void);
	bool GetRevStandby(void);


	ActiveState ConnectHost(IPDATA hostip);
private:
	std::thread mut_;
	bool revStandby_;
//	IPDATA ipdata_;			// IPの保存
	std::array<IPDATA, 5> mipdata_;
	std::unique_ptr<NetWorkState> network_state_;	//net work state

	NetWork();
	~NetWork();
};

