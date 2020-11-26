#pragma once
#include <memory>
#include <chrono>
#include <array>
#include <mutex>
#include "../Wall.h"
#include "Obj.h"
#include "../common/Vector2.h"
class Bomb :
	public Obj
{
public:
	Bomb(int ownerID,int selfID,Vector2 pos,std::chrono::system_clock::time_point now,std::shared_ptr<Wall>& wall);
	~Bomb();
	void Draw(void)override;
	void Update(void)override;
	int GetNo()override;
	void Init(void)override;
private:
	int ownerID_;
	int selfID_;
	std::chrono::system_clock::time_point now_;
	std::chrono::system_clock::time_point end_;

	//std::array<std::pair<bool,std::chrono::system_clock::time_point>,3> wastime_;


	std::vector<std::pair<bool, std::chrono::system_clock::time_point>> wastime_;
	int length_;			// 何段階伸びるか。
	float lengthtime_;
	float bombtime_;
	int bombcount_;			// 爆弾アニメーション millseconds
};