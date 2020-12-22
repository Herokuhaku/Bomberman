#include "OpenCloseScene.h"
#include <DxLib.h>
#include "SceneMng.h"

OpenCloseScene::OpenCloseScene(std::unique_ptr<BaseScene> old, std::unique_ptr<BaseScene> next)
{
	old_ = std::move(old);
	next_ = std::move(next);
	Init();
}

OpenCloseScene::~OpenCloseScene()
{
}

void OpenCloseScene::Init(void)
{
	screenID = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y);
	tmpscreen = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y);
	pos_[0][0] = Vector2d(0,0);
	pos_[0][1] = Vector2d(lpSceneMng.GetScreenSize().x, 0);
	pos_[1][0] = Vector2d(0, lpSceneMng.GetScreenSize().y);
	pos_[1][1] = Vector2d(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y);

}

std::unique_ptr<BaseScene> OpenCloseScene::Update(std::unique_ptr<BaseScene> own)
{
	if (!Transition())
	{
		return std::move(next_);
	}
	return own;
}

void OpenCloseScene::DrawOwnScreen(void)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();
	DrawBox(0,0,lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y,0xffffff,true);
	if (count_ < 128)
	{
		old_->NoBackDraw(tmpscreen);
		DrawModiGraph(pos_[0][0].x, pos_[0][0].y,
			pos_[0][1].x, pos_[0][1].y,
			pos_[1][1].x, pos_[1][1].y,
			pos_[1][0].x, pos_[1][0].y,tmpscreen, true);

		pos_[0][0].y += (lpSceneMng.GetScreenSize().y / 2.0) /128.0;
		pos_[0][1].y += (lpSceneMng.GetScreenSize().y / 2.0) /128.0;

		pos_[1][0].y -= (lpSceneMng.GetScreenSize().y / 2.0) /128.0;
		pos_[1][1].y -= (lpSceneMng.GetScreenSize().y / 2.0) /128.0;
	}
	else
	{
		next_->NoBackDraw(tmpscreen);
		DrawModiGraph(pos_[0][0].x, pos_[0][0].y,
			pos_[0][1].x, pos_[0][1].y,
			pos_[1][1].x, pos_[1][1].y,
			pos_[1][0].x, pos_[1][0].y, tmpscreen, true);
		pos_[0][0].y -= (lpSceneMng.GetScreenSize().y / 2.0) / 128.0;
		pos_[0][1].y -= (lpSceneMng.GetScreenSize().y / 2.0) / 128.0;

		pos_[1][0].y += (lpSceneMng.GetScreenSize().y / 2.0) / 128.0;
		pos_[1][1].y += (lpSceneMng.GetScreenSize().y / 2.0) / 128.0;
	}
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0,0,screenID,true);
}

bool OpenCloseScene::Transition(void)
{
	DrawOwnScreen();
	return ++count_ < 255;
}
