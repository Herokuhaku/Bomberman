#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "Obj.h"
#include "../Wall.h"
#include "../TiledLoader.h"
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
	ME,
	MEYOU,
	YOUR,
	YOUYOUR
};

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
	int OkNum()override;
private:
	void UpdateDef();
	void UpdateAuto();
	void UpdateNet();
	void Init(void);

	void DirRight(Vector2,int);
	void DirLeft(Vector2,int);
	void DirUp(Vector2,int);
	void DirDown(Vector2,int);
	void DirDeath(Vector2, int) {};

	std::vector<int> animation_;
	int screen;
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
	
	const int width = 32;
	std::unique_ptr<BaseScene> scene_;
public:
	static int fallCount;
};

