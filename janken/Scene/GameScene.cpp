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
	//time.now = std::chrono::system_clock::now();
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

	Image.resize(12);
	LoadDivGraph(tsxdata_.pass.c_str(),12,
		4,3,32,32,&Image[0]);

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
	num = lpTiledLoader.GetTmx().num;
	for (auto& map : mapdata_["Char"])
	{
		if (map != -1)
		{
			objlist_.emplace_back(std::make_shared<Player>(Vector2({i%std::atoi(num["width"].c_str())*32,i/ std::atoi(num["width"].c_str()) *32}),
				Vector2{ 32,51 }, wall_,*this));
		}
		i++;
	}
	begin = std::chrono::system_clock::now();
}

std::unique_ptr<BaseScene> GameScene::Update(std::unique_ptr<BaseScene> own)
{
	Draw();
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
			if (x >= std::atoi(num["width"].c_str())) { y++; x = 0; }
		}
	}
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0, 0, screenID, true);
}

void GameScene::SetBomb(int ownerID, int selfID, Vector2 pos,bool sendNet)
{
	if (sendNet)
	{
		MesData data;
		data.reserve(6);
		data.emplace_back(ownerID);
		data.emplace_back(selfID);
		data.emplace_back(pos.x);
		data.emplace_back(pos.y);

		//unionData uni[6];
		//time.now.now = std::chrono::system_clock::now();
		//uni[0].iData = ownerID;
		//uni[1].iData = selfID;
		//uni[2].iData = pos.x;
		//uni[3].iData = pos.y;
		//uni[4].uiData = time.inow[0];
		//uni[5].uiData = time.inow[1];
		lpNetWork.SendMesData(MesType::SET_BOMB, data);
	}
	objlist_.emplace_back(std::make_shared<Bomb>(ownerID,selfID,pos));
}