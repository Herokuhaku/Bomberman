#pragma once
#include "../common/Vector2.h"
class Obj
{
public:
	Obj();
	Obj(Vector2 pos,Vector2 size);
	~Obj();
	virtual void Draw(void) = 0;
	virtual void Update(void) = 0;
	virtual int GetNo() = 0;
	virtual int OkNum(void) = 0;
protected:
	Vector2 pos_;
	Vector2 size_;
	int id_;
private:
};

