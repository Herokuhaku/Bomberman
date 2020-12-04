#include <DxLib.h>
#include <time.h>
#include "SceneMng.h"
#include "../_debug/_DebugDispOut.h"
//#include "../_debug/_DebugConOut.h"
#include "LoginScene.h"
#include "GameScene.h"
#include "CrossOverScene.h"
#include "../Graphic/ImageMng.h"
#include "RotationScene.h"

 SceneMng* SceneMng::sInstance = nullptr;

 void SceneMng::Run(void)
 {
	 sceneNow_ = std::make_unique<LoginScene>();

	 _dbgSetDrawPosFps(FPS_SIDE::LEFT, FPS_VER::TOP);
	 while (!ProcessMessage())
	 {
		 now = std::chrono::system_clock::now();
		 _dbgStartDraw();
		 SetDrawScreen(DX_SCREEN_BACK);
		 ClsDrawScreen();

		 sceneNow_ = sceneNow_->Update(std::move(sceneNow_));
		 _dbgDrawFPS();
		 
		 _dbgAddDraw();
		 ScreenFlip();
	 }
 }

Vector2 SceneMng::GetScreenSize(void)
{
	return _screenSize;
}

std::chrono::system_clock::time_point SceneMng::GetNowTime()
{
	return now;
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
	SetAlwaysRunFlag(true);
	SetDoubleStartValidFlag(TRUE);
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
