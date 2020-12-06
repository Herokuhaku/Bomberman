#include <DxLib.h>
#include "CrossOverScene.h"
#include "SceneMng.h"
#include "../Graphic/ImageMng.h"

CrossOverScene::CrossOverScene(std::unique_ptr<BaseScene> old, std::unique_ptr<BaseScene> next)
{
	old_ = std::move(old);
	next_ = std::move(next);
}

CrossOverScene::~CrossOverScene()
{
}

void CrossOverScene::Init(void)
{
	screenID = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y);
}

std::unique_ptr<BaseScene> CrossOverScene::Update(std::unique_ptr<BaseScene> own)
{
	if (!Transition())
	{
		return std::move(next_);
	}
	return own;
}

void CrossOverScene::DrawOwnScreen(void)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA,255-count_);
	old_->Draw();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA,count_);
	next_->Draw();
}

bool CrossOverScene::Transition(void)
{
	DrawOwnScreen();
	return ++count_ < 255;
}
