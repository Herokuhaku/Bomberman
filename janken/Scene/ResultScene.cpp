#include <DxLib.h>
#include "ResultScene.h"
#include "SceneMng.h"
#include "../NetWork/NetWork.h"
#include "LoginScene.h"
#include "RotationScene.h"

ResultScene::ResultScene(int screen)
{
	gameScreen_ = screen;
	Init();
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init(void)
{
	screenID = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y, true);
	pos_ = { 0,0 };
}

std::unique_ptr<BaseScene> ResultScene::Update(std::unique_ptr<BaseScene> own)
{
	Draw();


	if (CheckHitKey(KEY_INPUT_SPACE))
	{
		lpNetWork.SetNetWorkMode(NetWorkMode::NON);
		lpTiledLoader.Destroy();
		lpNetWork.Destroy();
		return std::make_unique<RotationScene>(std::move(own), std::make_unique<LoginScene>());
	}
	return own;
}

void ResultScene::Draw(void)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();
	pos_ = { 0,0 };
	
	DrawString(pos_.x, pos_.y += 50,"èáà  : ÉLÉÉÉâID",0xffffff);
	int rank = 1;
	for (auto& id : lpNetWork.GetResult()) {
		if (id == -1) {
			break;
		}
		DrawFormatString(pos_.x,pos_.y+= 50,0xffffff,"%dà   :  %dP",rank++,id/5+1);
	}

	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0,0,screenID,true);
}
