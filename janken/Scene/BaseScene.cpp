#include <DxLib.h>
#include "BaseScene.h"
#include "SceneMng.h"
#include "../Graphic/ImageMng.h"
BaseScene::BaseScene()
{
	screenID = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y,true);
}

BaseScene::~BaseScene()
{
}

void BaseScene::Draw(void)
{
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0, 0, screenID, true);
}


void BaseScene::DrawOwnScreen(void)
{

}

void BaseScene::Draw(double ex, double rad)
{
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0, 0, screenID, true);
}
