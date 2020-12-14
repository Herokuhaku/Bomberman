#include <DxLib.h>
#include "RotationScene.h"
#include "SceneMng.h"

RotationScene::RotationScene(std::unique_ptr<BaseScene> old, std::unique_ptr<BaseScene> next)
{
	old_ = std::move(old);
	next_ = std::move(next);
	Init();
}

RotationScene::~RotationScene()
{
}

void RotationScene::Init(void)
{
	screenID = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y);
	flag = false;
	ex = 1.0f;
	rad = 0.0f;
	rad_ = 90;
}

std::unique_ptr<BaseScene> RotationScene::Update(std::unique_ptr<BaseScene> own)
{
	if (!Transition())
	{
		return std::move(next_);
	}
	return own;
}

void RotationScene::DrawOwnScreen(void)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();
	if (count_ > 128)
	{
		flag = true;
	}
	if (!flag)
	{
		ex -= 1.0 / 128.0;
		//ex -= 0.008;
		rad_ -= 1.0/180.0;
		rad -= cos(rad_);
		old_->Draw(ex,rad);
	}
	else
	{
		ex += 1.0 / 128.0;
		//ex += 0.008;
		rad_ += 1.0/180.0;
		rad += cos(rad_);
		next_->Draw(ex,rad);
	}
}

bool RotationScene::Transition(void)
{
	DrawOwnScreen();
	return ++count_ < 255;
}
