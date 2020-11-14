#pragma once
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "Obj.h"
#include "../Wall.h"
#include "../TiledLoader.h"
enum class DIR
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	DEATH
};

enum class MOVE_TYPE
{
	ME,
	MEYOU,
	YOUR,
	YOUYOUR
};

class Player :
	public Obj
{
public:
	Player();
	Player(Vector2 pos,Vector2 size,std::shared_ptr<Wall> wall);
	~Player();
	void Draw(void)override;
	void Update(void)override;
	int GetNo()override;
private:
	void MeUpdate();
	void YouUpdate();
	void Init(void);
	std::vector<int> animation_;
	int screen;
	DIR pldir_;
	std::map<std::string,std::vector<unsigned char>> Mapdata;
	std::map<DIR,int> animationdir_;
	int frame_;
	const int oneanimCnt = 10;
	std::function<void()> update_;
	std::shared_ptr<Wall> wall_;
	static int plid_;
	int id_;
	Vector2 oldpos_;
};

