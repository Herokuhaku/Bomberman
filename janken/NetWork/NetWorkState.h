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
	Non,		// 未設定
	Wait,		// 接続待機中(ホスト用)
	Init,		// 初期化中(ゲーム開始準備中)(ホスト/ゲスト)
	Standby,	// 初期化情報送信済みの開始待ち(ホスト用)
	Play,		// ゲーム中(ホスト/ゲスト)
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
	int networkHandle_ = 0;		// dxlibのネットワークハンドル

	std::mutex mtx_;
	std::vector<unionData> revtmx;
//	std::vector<int> revtmx_;
};