#include <Dxlib.h>
#include <functional>
#include "Bomb.h"

Bomb::Bomb(int ownerID, int selfID, Vector2 pos, std::chrono::system_clock::time_point now,std::shared_ptr<Wall>& wall)
{
	ownerID_ = ownerID;
	selfID_ = selfID;
	pos_ = pos;
	size_ = {32,32};
	wall_ = wall;
	now_ = now;
	Init();
}

Bomb::~Bomb()
{
}

void Bomb::Draw(void)
{
	SetDrawScreen(screen);
	ClsDrawScreen();
	DrawGraph(0, 0, animation_[1], true);
	//if (frame_ > oneanimCnt * 4 - 1)frame_ = oneanimCnt * 2;
	SetDrawScreen(DX_SCREEN_BACK);
	DrawRotaGraph(pos_.x, pos_.y, 1.0f, 0.0f, screen, true);
}

void Bomb::Update(void)
{
	end_ = std::chrono::system_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(end_ - now_).count() >= 3000 && alive_)
	{
		alive_ = false;
		now_ = end_;
	}
	if (!alive_)
	{
		float no = 0;
		Vector2 tmpos = pos_;
		std::function<void(Vector2, Vector2, int)> longfire = [&](Vector2 tmp, Vector2 plus, int num) {
			tmp += plus;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(end_ - now_).count() >= lengthtime_ * num)
			{
				wall_->ChangeMapData("Fire", tmp, 1);
				if (num < length_ - 1)
				{
					longfire(tmp, plus, num + 1);
				}
			}
		};

		std::function<void(Vector2, int, std::chrono::system_clock::time_point)>crossfire =
			[&](Vector2 tmp, int num, std::chrono::system_clock::time_point time) {
			wall_->ChangeMapData("Fire", tmp, num);
			if (std::chrono::duration_cast<std::chrono::milliseconds>(end_ - now_).count() >= lengthtime_ * num)
			{
				longfire({ tmp.x,tmp.y }, { size_.x,0 }, num + 1);
				longfire({ tmp.x,tmp.y }, { -size_.x,0 }, num + 1);
				longfire({ tmp.x,tmp.y }, { 0, size_.y }, num + 1);
				longfire({ tmp.x,tmp.y }, { 0,-size_.y }, num + 1);
			}
		};
		crossfire(tmpos, no, end_);
	}
}

int Bomb::GetNo()
{
	return id_;
}

void Bomb::Init(void)
{
	screen = MakeScreen(size_.x, size_.y,true);
	// �����p size bomb ���e�̃T�C�Y 
	animation_.resize(14);
	LoadDivGraph("Tiled/image/bomb.png", 14,2,7, size_.x, size_.y, animation_.data());
	length_ = 3;
	//lengthtime_ = 166.667;
	lengthtime_ = 2000;
}

