#pragma once
#include <vector>
#include "Obj.h"
#include "../TiledLoader.h"
enum class DIR
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	DEATH
};

class Player :
	public Obj
{
public:
	Player();
	Player(Vector2&& pos,Vector2&& size,TmxData tmxdata);
	~Player();
	void Draw(void)override;
	void Update(void)override;
private:
	void Init(void);
	std::vector<int> animation_;
	int screen;
	DIR pldir_;
	TmxData tmxdata_;
	int i;
};

