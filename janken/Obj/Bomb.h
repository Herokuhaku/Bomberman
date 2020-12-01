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
	Bomb(int ownerID,int selfID,Vector2 pos,std::chrono::system_clock::time_point now,double bombtime,int length,std::shared_ptr<Wall>& wall);
	~Bomb();
	void Draw(void)override;
	void Update(void)override;
	int GetNo()override;
	void Init(void)override;
	std::pair<ObjType, int> GetOwnerID(void)override;
private:
	int ownerID_;
	int selfID_;
	std::chrono::system_clock::time_point now_;
	std::chrono::system_clock::time_point end_;

	//std::array<std::pair<bool,std::chrono::system_clock::time_point>,3> wastime_;


	std::vector<std::pair<bool, std::chrono::system_clock::time_point>> wastime_;
	int length_;			// 何段階伸びるか。
	double lengthtime_;
	double bombtime_;
	int bombcount_;			// 爆弾アニメーション millseconds
	std::array<bool, 4> blockflag_;
	int maxcount_;			// 最大値
	Map wasMapData_;

	std::chrono::system_clock::time_point end;
};