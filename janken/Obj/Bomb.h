#pragma once
#include <memory>
#include <chrono>
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

	int length_;
	float lengthtime_;
};