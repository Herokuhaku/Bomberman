#include <Dxlib.h>
#include <functional>
#include "Bomb.h"
#include "../Scene/SceneMng.h"

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
	if (alive_) {
		SetDrawScreen(screen);
		ClsDrawScreen();
		DrawGraph(0, 0, animation_[1], true);
		SetDrawScreen(DX_SCREEN_BACK);
		DrawRotaGraph(pos_.x, pos_.y, 1.0f, 0.0f, screen, true);
	}
}

void Bomb::Update(void)
{
	end_ = lpSceneMng.GetNowTime();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(end_ - now_).count() >= bombtime_ && alive_)
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

				if (num < length_ && !wastime_[num].first)wastime_[num].second = end_; wastime_[num].first = true;
				if (num < length_ - 1)
				{
					longfire(tmp, plus, num + 1);
				}
			}
			if (wastime_[num].first && std::chrono::duration_cast<std::chrono::milliseconds>(end_ - wastime_[num].second).count() >= lengthtime_ * 7)
			{
				wall_->ChangeMapData("Fire", tmp, -1);
				if(num >= length_-1){ 
					deleteflag_ = true; 
				}
			}
		};

		std::function<void(Vector2, int, std::chrono::system_clock::time_point)>crossfire =
			[&](Vector2 tmp, int num, std::chrono::system_clock::time_point time) {
			wall_->ChangeMapData("Fire", tmp, num);
			if (std::chrono::duration_cast<std::chrono::milliseconds>(end_ - now_).count() >= lengthtime_ * num)
			{
				if(!wastime_[num].first)wastime_[num].second = end_; wastime_[num].first = true;
				longfire({ tmp.x,tmp.y }, { size_.x,0 }, num + 1);
				longfire({ tmp.x,tmp.y }, { -size_.x,0 }, num + 1);
				longfire({ tmp.x,tmp.y }, { 0, size_.y }, num + 1);
				longfire({ tmp.x,tmp.y }, { 0,-size_.y }, num + 1);
			}
			auto clock = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - wastime_[num].second).count();
			if (wastime_[num].first && std::chrono::duration_cast<std::chrono::milliseconds>(end_ - wastime_[num].second).count() >= lengthtime_ * 7)
			{
				wall_->ChangeMapData("Fire", tmp, -1);
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
	// 検索用 size bomb 爆弾のサイズ 
	animation_.resize(14);
	LoadDivGraph("Tiled/image/bomb.png", 14,2,7, size_.x, size_.y, animation_.data());
	bombtime_ = 3000;
	length_ = 3;
	wastime_.resize(length_);
	lengthtime_ = 1000.0 / 6.0;
	//lengthtime_ = 1000;
}

