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

using VecObj = std::vector<ShareObj>;
using ListObj = std::list<ShareObj>;

using TimeP = std::chrono::system_clock::time_point;
union chronoi
{
	std::chrono::system_clock::time_point now;
	unsigned int inow[2];
};
class GameScene :
	public BaseScene
{
public:
	GameScene();
	~GameScene();

	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own) override;
	void Draw(void)override;
	void Draw(float ex, float rad)override;
	void SetBomb(int ownerID, int selfID, Vector2 pos, bool sendNet, TimeP now);
private:
	std::vector<int> Image;
	ListObj objlist_;
	ShareObj fire_;
	std::shared_ptr<Wall> wall_;		// mapdataÇ…ÇÕFireÇÕì¸ÇÍÇ»Ç¢ÅBwallÇ™éùÇ¬
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;
	int fpsCnt_;
	chronoi time;
	std::map<std::string, std::string> num;
};