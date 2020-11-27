#pragma once
#include <map>
#include <string>
#include <vector>
#include "common/Vector2.h"

#define lpWall Wall::GetInstance()

enum class DIR
{
	DOWN,
	LEFT,
	RIGHT,
	UP,
	DEATH,
	NON
};

using Map = std::map<std::string, std::vector<unsigned char>>;
using FireData = std::vector<std::pair<unsigned char, DIR>>;
class Wall
{
public:
	Wall();
	~Wall();
	void Init(void);
	void SetMapData(Map map);
	Map GetMapData(void);
	void AddMapData(std::string name,std::vector<unsigned char> data);
	void UpdateFire(FireData data);
	void ChangeMapData(std::string name,Vector2 pos,int num);
	void ChangeFire(Vector2 pos,int num,DIR dir);
	FireData GetFireData(void);
private:
	Map mapdata_;
	int width;
	int height;
	FireData firemap_;
	std::map<DIR,int> crossflag_;
};

