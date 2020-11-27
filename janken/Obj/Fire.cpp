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

	auto tmpwall = wall_->GetFireData();

	SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
	for (auto& wall : tmpwall)
	{
		if (0 <= wall.first && wall.first < 12)
		{
			DrawRotaGraph(x * 32 + 16, y * 32 + 16, 1.0f,dirRad_[wall.second], animation_[wall.first],true);
		}
		x++;
		if (x >= numint["width"]) { y++; x = 0;}
	}
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0,0,screen,false);
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
	objtype_ = ObjType::Fire;
	animation_.resize(12);
	LoadDivGraph("Tiled/image/fire.png", 12, 3, 4, size_.x, size_.y, animation_.data(), true);
	numint["width"] = std::atoi(lpTiledLoader.GetTmx().num["width"].c_str());

	dirRad_[DIR::NON] = 0.0f;
	dirRad_[DIR::DEATH] = 0.0f;
	dirRad_[DIR::RIGHT] = RAD(360);
	dirRad_[DIR::DOWN] = RAD(90);
	dirRad_[DIR::LEFT] = RAD(180);
	dirRad_[DIR::UP] = RAD(270);
}
