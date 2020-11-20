#include <Dxlib.h>
#include "Bomb.h"

Bomb::Bomb(int ownerID, int selfID, Vector2 pos)
{
	ownerID_ = ownerID;
	selfID_ = selfID;
	pos_ = pos;
	size_ = {20,20};
	Init();
}

Bomb::~Bomb()
{

}

void Bomb::Draw(void)
{
	SetDrawScreen(screen);
	ClsDrawScreen();
	DrawGraph(0, 0, animation_[0], true);
	//if (frame_ > oneanimCnt * 4 - 1)frame_ = oneanimCnt * 2;
	SetDrawScreen(DX_SCREEN_BACK);
	DrawRotaGraph(pos_.x + size_.x / 2, pos_.y + size_.y / 6, 1.0f, 0.0f, screen, true);
}

void Bomb::Update(void)
{

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
}

