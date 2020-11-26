#include <DxLib.h>
#include <algorithm>
#include "GameScene.h"
#include "../Graphic/ImageMng.h"
#include "../NetWork/NetWork.h"
#include "../Obj/Player.h"
#include "../Obj/Bomb.h"
#include "../Obj/Fire.h"
#include "../_debug/_DebugDispOut.h"

GameScene::GameScene():time{lpSceneMng.GetNowTime()}
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
			mapdata_[data.first].emplace_back(d);
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
		if (map != 0)
		{
			objlist_.emplace_back(std::make_shared<Player>(Vector2({i%std::atoi(num["width"].c_str())*32,i/ std::atoi(num["width"].c_str()) *32}),
				Vector2{ 32,51 }, wall_,*this));
		}
		i++;
	}

	FireData fire;
	for (int i = 0; i < std::atoi(num["width"].c_str()) * std::atoi(num["height"].c_str()); i++)
	{
		fire.emplace_back(255,DIR::RIGHT);
	}
	wall_->UpdateFire(fire);
	fire_ = std::make_shared<Fire>(Vector2(32,32),wall_);
	begin = lpSceneMng.GetNowTime();
}

std::unique_ptr<BaseScene> GameScene::Update(std::unique_ptr<BaseScene> own)
{
	Draw();
	for (auto& obj : objlist_)
	{
		obj->Update();
		obj->Draw();
	}
	fire_->Update();
	end = lpSceneMng.GetNowTime();
	if (std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() >= 1)
	{
		begin = end;
		fpsCnt_++;
	}
	_dbgDrawFormatString(100, 0, 0x000000, "%d",Player::fallCount/fpsCnt_);

	objlist_.remove_if([&](std::shared_ptr<Obj>obj) {return obj->GetDeleteFlag(); });

	return own;
}

void GameScene::Draw(void)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();
	auto map = wall_->GetMapData();
	mapdata_ = map;
	for (auto& data : mapdata_) {
		int x = 0, y = 0;
		for (auto& no : data.second)
		{
			no--;
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

void GameScene::Draw(float ex, float rad)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();
	for (auto& data : wall_->GetMapData())
	{
		int x = 0, y = 0;
		for (auto& no : data.second)
		{
			no--;
			if (0 <= no && 12 > no) {
				DrawRotaGraph(x * 32 + 16, y * 32 + 16, 1.0f, 0.0f, Image[no], true);
			}
			x++;
			if (x >= std::atoi(num["width"].c_str())) { y++; x = 0; }
		}
	}
	SetDrawScreen(DX_SCREEN_BACK);
	DrawRotaGraph(lpSceneMng.GetScreenSize().x/2, lpSceneMng.GetScreenSize().y / 2,ex,rad, screenID, true);
}

void GameScene::SetBomb(int ownerID, int selfID, Vector2 pos,bool sendNet,TimeP now)
{
	if (sendNet)
	{
		MesData data;
		data.resize(7);
		data[0].iData = ownerID;
		data[1].iData = selfID;
		data[2].iData = pos.x;
		data[3].iData = pos.y;
		data[4].iData = 3;
		time.now = lpSceneMng.GetNowTime();
		data[5].uiData = time.inow[0];
		data[6].uiData = time.inow[1];

		//unionData uni[6];

		//uni[0].iData = ownerID;
		//uni[1].iData = selfID;
		//uni[2].iData = pos.x;
		//uni[3].iData = pos.y;
		//uni[4].uiData = time.inow[0];
		//uni[5].uiData = time.inow[1];
		//lpNetWork.SendMesData(MesType::SET_BOMB, {uni[0].iData,uni[1].iData ,uni[2].iData ,uni[3].iData ,uni[4].iData,uni[5].iData });
		lpNetWork.SendMesData(MesType::SET_BOMB, data);
	}
	objlist_.emplace_back(std::make_shared<Bomb>(ownerID,selfID,pos,now,wall_));
}