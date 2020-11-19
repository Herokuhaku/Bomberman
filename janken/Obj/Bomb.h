#pragma once
#include "Obj.h"
class Bomb :
	public Obj
{
public:
	Bomb();
	~Bomb();
	void Draw(void)override;
	void Update(void)override;
	int GetNo()override;
private:
};