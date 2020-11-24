#include "Obj.h"

Obj::Obj()
{
	pos_ = { 0,0 };
	size_ = { 0,0 };
	alive_ = true;
}

Obj::Obj(Vector2 pos, Vector2 size)
{
	pos_ = pos;
	size_ = size;
}

Obj::~Obj()
{
}