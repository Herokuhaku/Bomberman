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
		if (dir != DIR::NON) {
			if (firemap_[tmp].second != dir && firemap_[tmp].first != 255 &&
				crossflag_[firemap_[tmp].second] == crossflag_[dir] &&
				(num % 3 == 2 || num % 3 == 1))
			{
				firemap_[tmp] = { num / 3 * 3 + 1,dir };
				return;
			}
			else if (firemap_[tmp].second != dir && firemap_[tmp].first != 255 &&
				crossflag_[firemap_[tmp].second] != crossflag_[dir])
			{
				firemap_[tmp] = { num / 3 * 3,dir };
				return;
			}
		}
		firemap_[tmp] = {num,dir};
	}

}

FireData Wall::GetFireData(void)
{
	return firemap_;
}

Wall::Wall()
{
	Init();
}

Wall::~Wall()
{
}

void Wall::Init(void)
{
	width = std::atoi(lpTiledLoader.GetTmx().num["width"].c_str());
	height = std::atoi(lpTiledLoader.GetTmx().num["height"].c_str());
	crossflag_[DIR::LEFT] = 0;
	crossflag_[DIR::RIGHT] = 0;
	crossflag_[DIR::UP] = 1;
	crossflag_[DIR::DOWN] = 1;
	crossflag_[DIR::NON] = 10;
	crossflag_[DIR::DEATH] = 10;
}
