#pragma once
#include <Dxlib.h>
#include <memory>
#include <array>
#include <vector>
#include <mutex>
#include <thread>
#include "NetWorkState.h"

#define lpNetWork NetWork::GetInstance()

enum class MesType:unsigned char
{
	STANBY,			// 初期化情報送信完了(ホスト用)
	GAME_START,		// ホストからの初期化情報での初期化完了,ゲーム開始(ゲスト用)
	TMX_SIZE,
	TMX_DATA,
	POS,
	NON
};

struct MesHeader
{
	MesType type;
	unsigned short id;
	unsigned char cdata;
	unsigned int length;
};

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

	bool SendMes(MesHeader data);
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
	IPDATA ipdata_;			// IPの保存
	std::array<IPDATA, 5> mipdata_;
	std::unique_ptr<NetWorkState> network_state_;	//net work state

	NetWork();
	~NetWork();
};

