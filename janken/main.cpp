#include <DxLib.h>
#include "main.h"
#include "Scene/SceneMng.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	lpSceneMng.Run();
	DxLib::DxLib_End();
	return 0;
}