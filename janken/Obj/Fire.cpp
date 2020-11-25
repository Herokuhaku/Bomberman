#include <DxLib.h>
#include "Fire.h"
#include "../Scene/SceneMng.h"

Fire::Fire( Vector2&& size, std::shared_ptr<Wall>& wall)
{
	size_ = size;
	wall_ = wall;
	Init();
}

Fire::~Fire()
{
}

void Fire::Draw(void)
{
	SetDrawScreen(screen);
	ClsDrawScreen();
	int x = 0, y = 0;
	auto tmpwall = wall_->GetMapData()["Fire"];
	for (auto& wall : tmpwall)
	{
		if (0 <= wall && wall < 12)
		{
			DrawRotaGraph(x * 32 + 16, y * 32 + 16, 1.0f, 0.0f, animation_[wall],true);
		}
		x++;
		if (x >= width) { y++; x = 0;}
	}
	SetDrawBlendMode(DX_BLENDMODE_ADD, 200);
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0,0,screen,true);
}

void Fire::Update(void)
{
	Draw();
}

int Fire::GetNo()
{
	return 0;
}

void Fire::Init(void)
{
	screen = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y,true);

	animation_.resize(12);
	LoadDivGraph("Tiled/image/fire.png", 12, 3, 4, size_.x, size_.y, animation_.data(), true);
	width = std::atoi(lpTiledLoader.GetTmx().num["width"].c_str());
}
