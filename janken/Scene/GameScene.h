#pragma once
#include <vector>
#include <chrono>
#include <list>
#include "BaseScene.h"
#include "SceneMng.h"
#include "../Obj/Obj.h"
#include "../Wall.h"

using UniObj = std::unique_ptr<Obj>;
using ShareObj = std::shared_ptr<Obj>;

using VecObj = std::vector<UniObj>;
using ListObj = std::list<ShareObj>;

using TimeP = std::chrono::system_clock::time_point;



class GameScene :
	public BaseScene
{
public:
	GameScene();
	~GameScene();

	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own) override;
	void Draw(void)override;
	void Draw(double ex, double rad)override;
	void SetBomb(int ownerID, int selfID, Vector2 pos,TimeP nowbool,double bombtime,int length,bool sendNet);
	int BombCount(int ownerID);
private:
	std::vector<int> Image;
	ListObj objlist_;
	ShareObj fire_;
	std::shared_ptr<Wall> wall_;		// mapdata‚É‚ÍFire‚Í“ü‚ê‚È‚¢Bwall‚ª‚Â
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;
	int fpsCnt_;
	chronoi time;
	std::map<std::string, std::string> num;
	std::pair<int, int> playerID;
};