#pragma once
#include <map>
#include <string>
#include <vector>
#include "common/Vector2.h"
#define lpWall Wall::GetInstance()

using Map = std::map<std::string, std::vector<unsigned char>>;

class Wall
{
public:
	Wall();
	~Wall();
	void SetMapData(Map map);
	Map GetMapData(void);
	void AddMapData(std::string name,std::vector<unsigned char> data);
	void ChangeMapData(std::string name,Vector2 pos,int num);
private:
	Map mapdata_;
	int width;
	int height;
};

