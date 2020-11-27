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
		int time = (std::chrono::duration_cast<std::chrono::milliseconds>(end_ - now_).count());
		SetDrawScreen(screen);
		ClsDrawScreen();
		DrawGraph(0, 0, animation_[time/bombcount_ % 2], true);
		if (bombcount_ -10 > 0) { bombcount_ -=2; }
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
		wall_->ChangeMapData("Obj",pos_,0);
	}
	if (!alive_)
	{
		float no = 0;
		Vector2 tmpos = pos_;
		auto clock = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - now_).count();
		std::function<void(Vector2, Vector2, int,DIR,int)> longfire = [&](Vector2 tmp, Vector2 plus, int num,DIR dir,int blockf) {
			tmp += plus;
			if (clock >= lengthtime_ * num)
			{
				int nowblock = (tmp.x / width) + ((tmp.y / width) * numint["width"]);
				blockflag_.fill(false);
				if ((wasMapData_["Obj"][nowblock] == 0 || wasMapData_["Obj"][nowblock] == 8 || wasMapData_["Obj"][nowblock] == 255))
				{
					double frame_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - wastime_[num].second).count() / lengthtime_;
					int anim = static_cast<int>(abs(abs(4 - frame_) - 4))*3;
					// 最大カウント数(4方向合わせて最大どこまで伸びたか)
					maxcount_ = max(num, maxcount_);
					// ブロックがあればブロックを消してフラグを立てる
					if (wasMapData_["Obj"][nowblock] == 8)
					{
						blockflag_[blockf] = true;
						wall_->ChangeMapData("Obj", tmp, 0);
					}
					// 最初に炎を作った時間を記録
					if (num < length_ && !wastime_[num].first)wastime_[num].second = end_; wastime_[num].first = true;
					// 角を丸める
					int next = ((tmp.x + plus.x) / width) + (((tmp.y + plus.y) / width) * numint["width"]);
					if (num >= length_ - 1 || blockflag_[blockf] || 
						wasMapData_["Obj"][next] == 6 || wasMapData_["Obj"][next] == 7)
					{
						anim++;
					}
					// 炎のアニメーション指定
					wall_->ChangeFire(tmp,1+anim, dir);
					// 次の炎を出していいか判定
					if (num < length_ - 1 && !blockflag_[blockf])
					{
						longfire(tmp, plus, num + 1, dir, blockf);
					}
				}
			}
			// 既定のコマ数(7)*時間分立ったら炎を消す
			if (wastime_[num].first && std::chrono::duration_cast<std::chrono::milliseconds>(end_ - wastime_[num].second).count() >= lengthtime_ * 7)
			{
				wall_->ChangeFire(tmp, -1, DIR::NON);
				// 消しているのが中心から一番遠い炎だったらフラグを立てておく
				if (num == maxcount_)
				{
					deleteflag_ = true;
				}
			}
		};

		std::function<void(Vector2, int, std::chrono::system_clock::time_point)>crossfire =
			[&](Vector2 tmp, int num, std::chrono::system_clock::time_point time) {
			double frame_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_ - wastime_[num].second).count() / lengthtime_;
			int anim = abs(abs(4 - frame_) - 4);
			anim *= 3;
			wall_->ChangeFire(tmp,num + anim, DIR::RIGHT);
			if (clock >= lengthtime_ * num)
			{
				if(!wastime_[num].first)wastime_[num].second = end_; wastime_[num].first = true;
				longfire({ tmp.x,tmp.y }, { size_.x,0 }, num + 1,DIR::RIGHT,0);
				longfire({ tmp.x,tmp.y }, { -size_.x,0 }, num + 1,DIR::LEFT,1);
				longfire({ tmp.x,tmp.y }, { 0, size_.y }, num + 1,DIR::DOWN,2);
				longfire({ tmp.x,tmp.y }, { 0,-size_.y }, num + 1,DIR::UP,3);
			}
			if (wastime_[num].first && std::chrono::duration_cast<std::chrono::milliseconds>(end_ - wastime_[num].second).count() >= lengthtime_ * 7)
			{
				wall_->ChangeFire(tmp, -1, DIR::NON);
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
	width = size_.x;
	numint["width"] = std::atoi(lpTiledLoader.GetTmx().num["width"].c_str());

	for (int i = 0;i < length_;i++)
	{
		wastime_[i] = {false,std::chrono::system_clock::now()};
	}
	bombcount_ = 500;
	now_ = lpSceneMng.GetNowTime();
	wasMapData_ = wall_->GetMapData();
	blockflag_.fill(false);
	//lengthtime_ = 1000;
}

