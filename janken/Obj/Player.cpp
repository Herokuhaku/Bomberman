#include <DxLib.h>
#include "Player.h"
#include "../_debug/_DebugConOut.h"
Player::Player()
{
	pos_ = { 0,0 };
	size_ = { 0,0 };
	Init();
}

Player::Player(Vector2&& pos, Vector2&& size, TmxData tmxdata)
{
	pos_ = pos;
	size_ = size;
	tmxdata_ = tmxdata;
	Init();
}

Player::~Player()
{
}

void Player::Draw(void)
{
	DrawRotaGraph(pos_.x + size_.x / 2, pos_.y + size_.y / 2,1.0f,0.0f,animation_[0],true);
}

void Player::Update(void)
{
	Vector2 dirpos = { pos_.x+1,pos_.y+1};
	int width = 32;
	int height = 32;
	int a = 0;
	//dirpos.x += size_.x + 1;
	//a = (dirpos.x / 32) + ((dirpos.y / 32) * width);
	//if (a == 27)
	//{
	//	return;
	//}
	//if (tmxdata_.MapData["Obj"][(dirpos.x / width) + ((dirpos.y / height) * width) + 1] == 0)
	//{
	//	if (CheckHitKey(KEY_INPUT_D))
	//	{
	//		pos_.x++;
	//	}
	//}
	switch(pldir_)
	{

	case DIR::RIGHT:
		dirpos.x += size_.x;
		a = (dirpos.x / width) + ((dirpos.y / height) * width);
		if (tmxdata_.MapData["Obj"][(dirpos.x / width) + ((dirpos.y / height) * 21)] == 0)
		{
			pos_.x++;
		}
		else
		{
			pldir_ = DIR::DOWN;
		}
		break;
	case DIR::LEFT:
		dirpos.x --;
		if (tmxdata_.MapData["Obj"][(dirpos.x / width) + ((dirpos.y / height) * 21)] == 0)
		{
			pos_.x --;
		}
		else
		{
			pldir_ = DIR::UP;
		}
		break;
	case DIR::UP:
		dirpos.y --;
		if (tmxdata_.MapData["Obj"][(dirpos.x / width) + ((dirpos.y / height) * 21)] == 0)
		{
			pos_.y --;
		}
		else
		{
			pldir_ = DIR::RIGHT;
		}
		break;
	case DIR::DOWN:
		dirpos.y += size_.y;
		if (tmxdata_.MapData["Obj"][(dirpos.x / width) + ((dirpos.y / height) * 21)] == 0)
		{
			pos_.y ++;
		}
		else
		{
			pldir_ = DIR::LEFT;
		}
		break;
	case DIR::DEATH:
		break;
	default:
		TRACE("ë∂ç›ÇµÇ»Ç¢DIRÇå¸Ç¢ÇƒÇÈÇÊ");
		break;
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
	//screen = MakeScreen(size_.x,size_.y);
}
