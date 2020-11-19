#include <DxLib.h>
#include <algorithm>
#include "GameScene.h"
#include "SceneMng.h"
#include "../Graphic/ImageMng.h"
#include "../NetWork/NetWork.h"
#include "../Obj/Player.h"
#include "../Obj/Bomb.h"
#include "../_debug/_DebugDispOut.h"

GameScene::GameScene()
{
	Init();
}

GameScene::~GameScene()
{
}

void GameScene::Init(void)
{
	screenID = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y);

	if (lpNetWork.GetNetWorkMode() != NetWorkMode::GUEST) {
		tmxdata_ = lpTiledLoader.ReadTmx("Tiled/mapdata/map");
	}
	else 
	{
		tmxdata_ = lpTiledLoader.ReadTmx("Tiled/mapdata/tmp");
	}

	for (auto data : tmxdata_.MapData)
	{
		mapdata_.try_emplace(data.first);
	}
	for (auto data : tmxdata_.MapData)
	{
		for (auto d : data.second)
		{
			if (d == '\r' || d == '\n') { continue; }
			mapdata_[data.first].emplace_back(d - 1);
		}
	}
	tsxdata_ = lpTiledLoader.ReadTsx("Tiled/mapdata/map");
	//lpTiledLoader.TmxCsv();

	Image.resize(12);
	LoadDivGraph(tsxdata_.pass.c_str(),12,
		4,3,32,32,&Image[0]);

	// pos , size
	wall_ = std::make_shared<Wall>();
	wall_->SetMapData(tmxdata_.MapData);
	while (ProcessMessage() == 0 && lpNetWork.GetNetWorkMode() == NetWorkMode::GUEST)
	{
		if (lpNetWork.GetActive() == ActiveState::Play)
		{
			break;
		}
	}
	int i = 0;
	for (auto& map : mapdata_["Char"])
	{
		if (map != -1)
		{
			objlist_.emplace_back(std::make_shared<Player>(Vector2({i%21*32,i/21*32}), Vector2{ 32,51 }, wall_,*this));
		}
		i++;
	}
	begin = std::chrono::system_clock::now();
}

std::unique_ptr<BaseScene> GameScene::Update(std::unique_ptr<BaseScene> own)
{
	Draw();
	//std::sort(objlist_.begin(), objlist_.end(), [](ShareObj obj1, ShareObj obj2) {return obj1->OkNum() > obj2->OkNum();});
	for (auto& obj : objlist_)
	{
		obj->Update();
		obj->Draw();
	}
	end = std::chrono::system_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() >= 1)
	{
		begin = end;
		fpsCnt_++;
	}
	_dbgDrawFormatString(100, 0, 0x000000, "%d",Player::fallCount/fpsCnt_);

	return own;
}

void GameScene::Draw(void)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();
	for (auto& data : mapdata_) {
		int x = 0, y = 0;
		for (auto& no : data.second)
		{
			if (0 <= no && 12 > no) {
				DrawRotaGraph(x * 32 + 16, y * 32 + 16, 1.0f, 0.0f,Image[no], true);
			}
			x++;
			if (x >= 21) { y++; x = 0; }
		}
	}
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0, 0, screenID, true);
}

void GameScene::SetBomb()
{
	objlist_.emplace_back(std::make_shared<Bomb>());
}