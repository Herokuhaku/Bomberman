#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <list>
#include "Obj.h"
#include "../Wall.h"
#include "../TiledLoader.h"
#include "../NetWork/NetWork.h"
#include "../AllControl/Control.h"
#include "../AllControl/INPUT_ID.h"
//#include "../Scene/BaseScene.h"
class BaseScene;

#define UNIT_ID_BASE 5

enum class DIR
{
	DOWN,
	LEFT,
	RIGHT,
	UP,
	DEATH
};

enum class MOVE_TYPE
{
	Def,
	Auto,
	Net
};

using DellistData = std::pair<std::pair<INPUT_ID, TrgBool>, bool>;

class Player :
	public Obj
{
public:
	Player();
	Player(Vector2 pos,Vector2 size,std::shared_ptr<Wall> wall,BaseScene& scene);
	~Player();
	void Draw(void)override;
	void Update(void)override;
	int GetNo()override;
	void Init(void)override;
private:
	void UpdateDef();
	void UpdateAuto();
	void UpdateNet();

	void KeyInit();

	void DirRight(Vector2,int);
	void DirLeft(Vector2,int);
	void DirUp(Vector2,int);
	void DirDown(Vector2,int);
	void DirDeath(Vector2, int) {};

	// 
	//std::vector<int> animation_;
	DIR pldir_;
	std::map<std::string,std::vector<unsigned char>> Mapdata;
	std::map<DIR,int> animationdir_;
	int frame_;
	const int oneanimCnt = 10;
	std::function<void()> update_;
	std::map<DIR,std::function<void(Vector2,int)>> dirupdate_;
	std::shared_ptr<Wall> wall_;
	static int countid_;
	int playerid_;
	Vector2 oldpos_;
	MesList meslist_;
	std::mutex mtx_;
	
	MOVE_TYPE type;						// movetypeämîFóp
	
	std::unique_ptr<BaseScene> scene_;
	// ëÄçÏ
	std::unique_ptr<Control>controller_;
	std::map < INPUT_ID,std::function<bool(TrgBool data,bool flag)>> keymove_;
	std::map < INPUT_ID, std::function<bool(DellistData& data, bool flag)>> keymove;
	std::list<DellistData> keylist_;
	std::pair<	std::list<std::pair<INPUT_ID, TrgBool>>,bool> keydellist_;
	const int width = 32;
	int speed_;
public:
	static int fallCount;
};

