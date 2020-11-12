#pragma once
#include <map>
#include <string>
#include <vector>
#define lpWall Wall::GetInstance()

using Map = std::map<std::string, std::vector<unsigned char>>;

class Wall
{
public:
	Wall();
	~Wall();
	void SetMapData(Map map);
	Map GetMapData(void);
private:
	Map mapdata_;
};

