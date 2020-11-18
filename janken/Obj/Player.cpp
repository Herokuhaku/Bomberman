#include <DxLib.h>
#include "Player.h"
#include "../_debug/_DebugConOut.h"
#include "../NetWork/NetWork.h"

int Player::plid_ = 0;
int Player::fallCount = 0;
int Player::nfallCount = 0;

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

void Player::MeUpdate()
{
	Vector2 dirpos = { pos_.x,pos_.y };
	int width = 32;
	switch (pldir_)
	{
	case DIR::RIGHT:
		dirpos.x += size_.x;
		if (wall_->GetMapData()["Obj"][(dirpos.x / width) + ((dirpos.y / width) * 21)] == 0)
		{
			pos_.x += 5;
		}
		else
		{
			pldir_ = DIR::DOWN;
			frame_ = oneanimCnt * 2;
		}
		break;
	case DIR::LEFT:
		dirpos.x -= 5;
		if (wall_->GetMapData()["Obj"][(dirpos.x / width) + ((dirpos.y / width) * 21)] == 0)
		{
			pos_.x -= 5;
		}
		else
		{
			pldir_ = DIR::UP;
			frame_ = oneanimCnt * 2;
		}
		break;
	case DIR::UP:
		dirpos.y -= 5;
		if (wall_->GetMapData()["Obj"][(dirpos.x / width) + ((dirpos.y / width) * 21)] == 0)
		{
			pos_.y -= 5;
		}
		else
		{
			pldir_ = DIR::RIGHT;
			frame_ = oneanimCnt * 2;
		}
		break;
	case DIR::DOWN:
		dirpos.y += 32;
		if (wall_->GetMapData()["Obj"][(dirpos.x / width) + ((dirpos.y / width) * 21)] == 0)
		{
			pos_.y += 5;
		}
		else
		{
			pldir_ = DIR::LEFT;
			frame_ = oneanimCnt * 2;
		}
		break;
	case DIR::DEATH:
		break;
	default:
		TRACE("ë∂ç›ÇµÇ»Ç¢DIRÇå¸Ç¢ÇƒÇÈÇÊ");
		break;
	}
	lpNetWork.SendMesData(MesType::POS, {id_,pos_.x,pos_.y,static_cast<int>(pldir_)});
}

void Player::YouUpdate()
{
	MesData rev = lpNetWork.TakeOutRevData(id_);
	int id = 0;

	if (rev.size() == 4)
	{
		id = rev[0];
		pos_.x = rev[1];
		pos_.y = rev[2];
		pldir_ = static_cast<DIR>(rev[3]);
		nfallCount++;
	}
	else
	{
		TRACE("PosDataÇ»Çµ\n");
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
	Mapdata = wall_->GetMapData();
	//screen = MakeScreen(size_.x,size_.y);
	oldpos_ = pos_;
	id_ = plid_++;
	screen = MakeScreen(size_.x,size_.y,true);
}
