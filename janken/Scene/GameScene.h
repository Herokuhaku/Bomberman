#pragma once
#include <vector>
#include <chrono>
#include "BaseScene.h"
#include "../Obj/Obj.h"
#include "../Wall.h"

class GameScene :
	public BaseScene
{
public:
	GameScene();
	~GameScene();

	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own) override;
	void Draw(void)override;
private:
	std::vector<int> Image;
	std::vector<std::shared_ptr<Obj>> objlist_;
	std::shared_ptr<Wall> wall_;
	std::chrono::system_clock::time_point begin;
	std::chrono::system_clock::time_point end;
	int fpsCnt_;
};