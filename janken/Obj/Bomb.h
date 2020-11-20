#pragma once
#include "Obj.h"
#include "../common/Vector2.h"
class Bomb :
	public Obj
{
public:
	Bomb(int ownerID,int selfID,Vector2 pos);
	~Bomb();
	void Draw(void)override;
	void Update(void)override;
	int GetNo()override;
	void Init(void)override;
private:
	int ownerID_;
	int selfID_;
};