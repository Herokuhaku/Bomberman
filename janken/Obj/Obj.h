#pragma once
#include <vector>
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
	virtual void Init(void) = 0;
protected:
	Vector2 pos_;
	Vector2 size_;
	int id_;
	int screen;

	std::vector<int> animation_;
private:
};

