#include "Wall.h"
#include "TiledLoader.h"

void Wall::SetMapData(Map map)
{
	mapdata_ = map;
}

Map Wall::GetMapData(void)
{
	return mapdata_;
}

void Wall::AddMapData(std::string name, std::vector<unsigned char> data)
{
	mapdata_.try_emplace(name, data);
}

void Wall::UpdateFire(FireData data)
{
	firemap_ = data;
}

void Wall::ChangeMapData(std::string name, Vector2 pos, int num)
{
	auto tmp = (pos.x / 32) + ((pos.y / 32) * width);
	if (0 <= tmp && tmp < (height*width))
	{
		mapdata_[name][tmp] = num;
	}
}

void Wall::ChangeFire(Vector2 pos, int num, DIR dir)
{
	auto tmp = (pos.x / 32) + ((pos.y / 32) * width);
	if (0 <= tmp && tmp < (height * width))
	{
		firemap_[tmp] = {num,dir};
	}

}

FireData Wall::GetFireData(void)
{
	return firemap_;
}

Wall::Wall()
{
	width = std::atoi(lpTiledLoader.GetTmx().num["width"].c_str());
	height = std::atoi(lpTiledLoader.GetTmx().num["height"].c_str());
}

Wall::~Wall()
{
}
