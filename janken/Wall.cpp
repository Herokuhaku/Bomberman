#include "Wall.h"

void Wall::SetMapData(Map map)
{
	mapdata_ = map;
}

Map Wall::GetMapData(void)
{
	return mapdata_;
}

Wall::Wall()
{
}

Wall::~Wall()
{
}
