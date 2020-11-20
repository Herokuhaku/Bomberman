#pragma once
#include <vector>
#include <chrono>
#include <list>
#include "BaseScene.h"
#include "../Obj/Obj.h"
#include "../Wall.h"

using UniObj = std::unique_ptr<Obj>;
using ShareObj = std::shared_ptr<Obj>;

using VecObj = std::vector<ShareObj>;
using ListObj = std::list<ShareObj>;

//union chronoi
//{
//	std::chrono::system_clock::time_point now;
//	unsigned int inow[2];
//};
class GameScene :
	public BaseScene
{
public:
	GameScene();
	~GameScene();

	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own) override;
	void Draw(void)override;
	void SetBomb(int ownerID, int selfID, Vector2 pos, bool sendNet);
private:
	std::vector<int> Image;
	ListObj objlist_;
	std::shared_ptr<Wall> wall_;
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;
	int fpsCnt_;
	//chronoi time;
};