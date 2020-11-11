#include <DxLib.h>
#include "SceneMng.h"
#include "../_debug/_DebugDispOut.h"
#include "LoginScene.h"
#include "../Graphic/ImageMng.h"
#include <time.h>
 SceneMng* SceneMng::sInstance = nullptr;

void SceneMng::Run(void)
{
	sceneNow_ = std::make_unique<LoginScene>();
	while (!ProcessMessage() && !CheckHitKey(KEY_INPUT_ESCAPE))
	{
		SetDrawScreen(DX_SCREEN_BACK);
		ClsDrawScreen();
		
		sceneNow_ = sceneNow_->Update(std::move(sceneNow_));

		ScreenFlip();
	}
}

Vector2 SceneMng::GetScreenSize(void)
{
	return _screenSize;
}

bool SceneMng::SysInit(void)
{
	bool rtnFlag = true;

	SetWindowText("ボンバーマーン");
	SetGraphMode(_screenSize.x, _screenSize.y, 16);
	ChangeWindowMode(true);
	if (DxLib::DxLib_Init() == -1)
	{
		return false;
	}
	srand((unsigned int)time(NULL));

	SetDrawScreen(DX_SCREEN_BACK);
	return true;
}

SceneMng::SceneMng() :_screenSize(32*21,32*17)
{
	SysInit();
}

SceneMng::~SceneMng()
{
}
