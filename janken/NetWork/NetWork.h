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

	std::vector<int> SendMesHeader(MesHeader data);
	bool SendMesData(MesData data);
	bool SendMesData(MesType type, MesData data);
	bool SendMesData(MesType type);
	void SendStandby(void);
	void SendStart(void);
	void SendTmxSize(void);

	//IPDATA GetIP(void);		// ipdata‚Ì’†g‚ğ•Ô‚è’l‚Æ‚µ‚Ä•Ô‚·
	std::array<IPDATA, 5> GetIP(void);
	NetWorkMode GetNetWorkMode(void);
	ActiveState GetActive(void);
	int GetNetWorkHandle(void);
	bool GetRevStandby(void);
	int GetMaxByte(void);

	ActiveState ConnectHost(IPDATA hostip);
private:
	bool Setting(void);

	std::thread mut_;
	bool revStandby_;
//	IPDATA ipdata_;			// IP‚Ì•Û‘¶
	std::array<IPDATA, 5> mipdata_;
	std::unique_ptr<NetWorkState> network_state_;	//net work state
	int maxByte_;

	NetWork();
	~NetWork();
};

