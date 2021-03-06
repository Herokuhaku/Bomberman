#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <list>
#include <array>
#include "Obj.h"
#include "../Wall.h"
#include "../TiledLoader.h"
#include "../NetWork/NetWork.h"
#include "../AllControl/Control.h"
#include "../AllControl/INPUT_ID.h"
class BaseScene;

#define UNIT_ID_BASE 5

enum class MOVE_TYPE
{
	Def,
	Auto,
	Net
};

struct HaveState
{
	int havebomb_;
	int havelength_;
	int havespeed_;
};

enum class Have
{
	Bomb,
	Length,
	Speed
};


using DellistData = std::pair<std::pair<INPUT_ID, TrgBool>, bool>;

class Player :
	public Obj
{
public:
	Player(Vector2 pos,Vector2 size,std::shared_ptr<Wall>& wall,BaseScene& scene);
	~Player();
	void Draw(void)override;
	void Update(void)override;
	int GetNo()override;
	void Init(void)override;
	void SetBombBool(int, bool)override;
private:
	void UpdateDef();
	void UpdateAuto();
	void UpdateNet();

	void KeyInit();

	bool CheckDeath(void);
	void CheckItem(void);

	void DirRight(Vector2,int);
	void DirLeft(Vector2,int);
	void DirUp(Vector2,int);
	void DirDown(Vector2,int);
	void DirDeath(Vector2, int) {};

	// 
	Vector2 centerpos_;		// posとは別　判定の中心座標
	DIR pldir_;				// playerのDIR
	std::map<std::string,std::vector<unsigned char>> Mapdata;	// Mapdata
	std::map<DIR,int> animationdir_;							// animationの番号　画像の左から何番目かを保存する
	int frame_;
	const int oneanimCnt = 10;
	std::function<void()> update_;
	std::map<DIR,std::function<void(Vector2,int)>> dirupdate_;
	static int countid_;
	int playerid_;
	std::array<std::pair<int, bool>, 4> bomb_;
	//Vector2 oldpos_;
	MesList meslist_;
	std::mutex mtx_;
	
	MOVE_TYPE type;						// movetype確認用
	
	//std::unique_ptr<BaseScene> scene_;
	BaseScene& scene_;
	Vector2 bombpos_;
	std::map<std::string, std::string> numstr;

	// 操作
	std::unique_ptr<Control>controller_;
	std::map < INPUT_ID, std::function<bool(DellistData& data, bool flag)>> keymove_;
	std::list<DellistData> keylist_;
	std::pair<std::list<std::pair<INPUT_ID, TrgBool>>,bool> keydellist_;
	int speed_;										// 何も触ってないスピード(アイテム無し)
	int layerchip_;									// 1レイヤーの総マス数 縦*横
	int bomblist;									// 現状出している爆弾の数

	std::map<Have,std::tuple<int,int,int>> plm;		// first 現在の所持数  second 持ってもよい最大数 third　画像の表番号
	int color_;
public:
	static int fallCount;
};

