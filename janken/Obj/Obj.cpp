#include "Obj.h"

Obj::Obj()
{
	pos_ = { 0,0 };
	size_ = { 0,0 };
	alive_ = true;
	animalive_ = true;
	deleteflag_ = false;
	objtype_ = ObjType::Obj;
}

Obj::Obj(Vector2 pos, Vector2 size)
{
	pos_ = pos;
	size_ = size;
}

Obj::~Obj()
{
}

bool Obj::GetAlive(void)
{
	return alive_;
}

std::pair<ObjType,int> Obj::GetOwnerID(void)
{
	return {objtype_,id_};
}

bool Obj::GetDeleteFlag(void)
{
	return deleteflag_;
}

int Obj::GetSelfID()
{
	return id_;
}

void Obj::SetBombBool(int, bool)
{
}
