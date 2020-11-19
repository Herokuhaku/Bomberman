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
	Non,		// 未設定
	Wait,		// 接続待機中(ホスト用)
	Init,		// 初期化中(ゲーム開始準備中)(ホスト/ゲスト)
	Standby,	// 初期化情報送信済みの開始待ち(ホスト用)
	Play,		// ゲーム中(ホスト/ゲスト)
	//Instance,	// インスタンス中
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
	STANBY,			// 初期化情報送信完了(ホスト用)
	GAME_START,		// ホストからの初期化情報での初期化完了,ゲーム開始(ゲスト用)
	TMX_SIZE,		// TMXサイズ　総サイズ
	TMX_DATA,		// TMXデータ　CSVのみ切り取って,を外したもの
	POS,			// ゲーム中に送る
	SET_BOMB,		
	MAX
};


struct MesHeader
{
	MesType type;				// メッセージの種別
	unsigned char next;			// データ送信が分割の場合に、,  次がない　0 次がある　1 
	unsigned short sendid;		// 分割送信時のナンバリング
	unsigned int length;		// 分割かどうかは関係ない単一パケットのデータ長(intの数) バイト数/4
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
	int networkHandle_ = 0;		// dxlibのネットワークハンドル

	std::mutex mtx_;
//	std::vector<unionData> revtmx;
//	MesType nowtype_;
	MesSizeData sizedata_;
	//std::vector<unionData> revdata_;

	std::pair<MesType, std::vector<unionData>> revtmx_;
	MesList revlist_;
	std::map<int,std::vector<int>> posdata_;
};