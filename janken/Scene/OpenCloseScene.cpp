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
	pos_[0][0] = Vector2(0,0);
	pos_[0][1] = Vector2(lpSceneMng.GetScreenSize().x, 0);
	pos_[1][0] = Vector2(0, lpSceneMng.GetScreenSize().y-400);
	pos_[1][1] = Vector2(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y-400);

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

	if (count_ < 128)
	{
		old_->NoBackDraw();
	}
	else
	{
		next_->NoBackDraw();
		DrawModiGraph(pos_[0][0].x, pos_[0][0].y,
			pos_[0][1].x, pos_[0][1].y,
			pos_[1][1].x, pos_[1][1].y,
			pos_[1][0].x, pos_[1][0].y, screenID, true);
	}
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0,0,screenID,true);
}

bool OpenCloseScene::Transition(void)
{
	DrawOwnScreen();
	return ++count_ < 255;
}
