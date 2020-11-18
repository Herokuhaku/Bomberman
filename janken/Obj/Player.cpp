#include <DxLib.h>
#include "Player.h"
#include "../_debug/_DebugConOut.h"
#include "../NetWork/NetWork.h"

int Player::plid_ = 0;
int Player::fallCount = 0;

Player::Player()
{
	pos_ = { 0,0 };
	size_ = { 0,0 };
	Init();
}

Player::Player(Vector2 pos, Vector2 size, std::shared_ptr<Wall> wall)
{
	pos_ = pos;
	size_ = size;
	wall_ = wall;
	Init();
}

Player::~Player()
{
}

void Player::Draw(void)
{
	SetDrawScreen(screen);
	ClsDrawScreen();
	//DrawRotaGraph(pos_.x + size_.x /2, pos_.y + size_.y / 6,1.0f,0.0f,animation_[frame_++ / oneanimCnt * 5 + animationdir_[pldir_]],true);
	DrawGraph(0,0,animation_[frame_++ / oneanimCnt * 5 + animationdir_[pldir_]], true);
	if (frame_ > oneanimCnt * 4-1)frame_ = oneanimCnt*2;
	SetDrawScreen(DX_SCREEN_BACK);
	DrawRotaGraph(pos_.x + size_.x / 2, pos_.y + size_.y / 6,1.0f,0.0f,screen,true);
}

void Player::Update(void)
{
	update_();
}

int Player::GetNo()
{
	return plid_;
}

int Player::OkNum()
{
	return lpNetWork.TakeOutRevData(id_).size();
}

void Player::MeUpdate()
{
	dirupdate_[pldir_](pos_,width);

	lpNetWork.SendMesData(MesType::POS, {id_,pos_.x,pos_.y,static_cast<int>(pldir_)});
}

void Player::YouUpdate()
{
	MesData rev = lpNetWork.TakeOutRevData(id_);

	if (rev.size() == 4)
	{
		pos_.x = rev[1];
		pos_.y = rev[2];
		pldir_ = static_cast<DIR>(rev[3]);
	}
	else
	{
		TRACE("NotData  ¡¡%d¡¡ \n",id_);
		fallCount++;
	}
}

void Player::Init(void)
{
	animation_.resize(20);
	LoadDivGraph("Tiled/image/bomberman.png", 20, 5, 4, size_.x, size_.y, animation_.data());
	//if (animation_[0] = LoadGraph("Tiled/image/bomberman.png"),animation_[0] == -1)
	//{
	//	return;
	//}
	pldir_ = DIR::RIGHT;
	animationdir_[DIR::DOWN] = 0;
	animationdir_[DIR::LEFT] = 1;
	animationdir_[DIR::RIGHT] = 2;
	animationdir_[DIR::UP] = 3;
	animationdir_[DIR::DEATH] = 4;
	frame_ = oneanimCnt*2;

	if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST)
	{
		if (plid_ % 2 == 0)	{
			update_ = std::bind(&Player::MeUpdate, this);
		}
		else {
			update_ = std::bind(&Player::YouUpdate, this);
		}
	}
	else
	{
		if (plid_ % 2 == 0) {

			update_ = std::bind(&Player::YouUpdate, this);
		}
		else
		{
			update_ = std::bind(&Player::MeUpdate, this);
		}
	}
	dirupdate_[DIR::RIGHT] = [&](Vector2 pos,int width) {DirRight(pos,width);};
	dirupdate_[DIR::LEFT] = [&](Vector2 pos, int width) {DirLeft(pos, width);};
	dirupdate_[DIR::UP] = [&](Vector2 pos, int width) {DirUp(pos, width);};
	dirupdate_[DIR::DOWN] = [&](Vector2 pos, int width) {DirDown(pos, width);};
	dirupdate_[DIR::DEATH] = [&](Vector2 pos, int width) {DirDeath(pos, width);};
	Mapdata = wall_->GetMapData();
	//screen = MakeScreen(size_.x,size_.y);
	oldpos_ = pos_;
	id_ = plid_++;
	screen = MakeScreen(size_.x,size_.y,true);
}
void Player::DirRight(Vector2 pos, int width)
{
	pos.x += size_.x;
	if (wall_->GetMapData()["Obj"][(pos.x / width) + ((pos.y / width) * 21)] == 0)
	{
		pos_.x += 5;
	}
	else
	{
		pldir_ = DIR::DOWN;
		frame_ = oneanimCnt * 2;
	}
}
void Player::DirLeft(Vector2 pos, int width)
{
	pos.x -= 5;
	if (wall_->GetMapData()["Obj"][(pos.x / width) + ((pos.y / width) * 21)] == 0)
	{
		pos_.x -= 5;
	}
	else
	{
		pldir_ = DIR::UP;
		frame_ = oneanimCnt * 2;
	}
}

void Player::DirUp(Vector2 pos, int width)
{
	pos.y -= 5;
	if (wall_->GetMapData()["Obj"][(pos.x / width) + ((pos.y / width) * 21)] == 0)
	{
		pos_.y -= 5;
	}
	else
	{
		pldir_ = DIR::RIGHT;
		frame_ = oneanimCnt * 2;
	}
}

void Player::DirDown(Vector2 pos, int width)
{
	pos.y += 32;
	if (wall_->GetMapData()["Obj"][(pos.x / width) + ((pos.y / width) * 21)] == 0)
	{
		pos_.y += 5;
	}
	else
	{
		pldir_ = DIR::LEFT;
		frame_ = oneanimCnt * 2;
	}
}
