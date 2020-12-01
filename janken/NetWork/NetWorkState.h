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
	Non,		// 未設定
	Wait,		// 接続待機中(ホスト用)
	Init,		// 初期化中(ゲーム開始準備中)(ホスト/ゲスト)
	Standby,	// 初期化情報送信済みの開始待ち(ホスト用)
	Matching,
	Play,		// ゲーム中(ホスト/ゲスト)
	//Instance,	// インスタンス中
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

using ListInt = std::list<std::pair<int, unsigned int>>;	// playeridとネットワークハンドル
enum class MesType :unsigned char
{
	NON = 100,
	COUNT_DOWN_ROOM,	// 接続受付カウントダウン
	ID,					// 自分のIDとプレイヤー総数
	STANBY_HOST,		// 初期化情報送信完了(ホスト用)
	STNBY_GUEST,		// ホストからの初期化情報での初期化完了,ゲーム開始(ゲスト用)
	COUNT_DOWN_GAME,	// 全員の初期化完了後のゲーム開始時間
	TMX_SIZE,			// TMXサイズ　総サイズ
	TMX_DATA,			// TMXデータ　CSVのみ切り取って,を外したもの
	POS,				// ゲーム中に送る
	SET_BOMB,			// ボムの配置
	DEATH,				// 死亡		どのキャラかわかるようにIDだけ付与する
	LOST,				// 接続時に生成(ホストは自分のネットワークキャラにもセットする)
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
using MesList = std::vector<SavePacket>;		// Obj全般の情報

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
	int networkHandle_ = 0;		// dxlibのネットワークハンドル

	std::mutex mtx_;
	MesPacket revtmx;			// 受け取り用box(tmx)
	//MesSizeData sizedata_;		// サイズデータ
	std::vector<std::pair<MesList&,std::mutex&>> revlist;	// 受け取り用box Player用
	MesList reset;

	chronoi timestart_;		// 時間変換用
	std::pair<int, int> player;	// IDとMax

	std::map<MesType, std::function<bool(MesHeader,MesPacket,int&)>> MesTypeList_;

	std::map<std::string, int> num;

	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;

	std::list<int> deathlist_;
};