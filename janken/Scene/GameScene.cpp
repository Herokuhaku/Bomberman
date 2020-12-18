#include <DxLib.h>
#include <algorithm>
#include <array>
#include "GameScene.h"
#include "../Graphic/ImageMng.h"
#include "../NetWork/NetWork.h"
#include "../Obj/Player.h"
#include "../Obj/Bomb.h"
#include "../Obj/Fire.h"
#include "RotationScene.h"
#include "LoginScene.h"
#include "CrossOverScene.h"
#include "ResultScene.h"
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

	if (lpNetWork.GetNetWorkMode() != NetWorkMode::OFFLINE)
	{
		playerID = lpNetWork.PlayerID();
	}
	int i = 0;
	num = lpTiledLoader.GetTmx().num;
	int id = 0;
	for (auto& map : mapdata_["Char"])
	{
		if (map != 0)
		{
			if (playerID.second * 5 > id || lpNetWork.GetNetWorkMode() == NetWorkMode::OFFLINE)
			{
				objlist_.emplace_back(std::make_unique<Player>(Vector2({ i % std::atoi(num["width"].c_str()) * 32,i / std::atoi(num["width"].c_str()) * 32}),
					Vector2{ 32,51 }, wall_,*this));
				id += 5;
			}
		}
		i++;
	}

	FireData fire;
	for (int i = 0; i < std::atoi(num["width"].c_str()) * std::atoi(num["height"].c_str()); i++)
	{
		fire.emplace_back(255,DIR::NON);
	}
	wall_->UpdateFire(fire);
	fire_ = std::make_shared<Fire>(Vector2(32,32),wall_);
	begin = lpSceneMng.GetNowTime();
}

std::unique_ptr<BaseScene> GameScene::Update(std::unique_ptr<BaseScene> own)
{
	end = lpSceneMng.GetNowTime();
	Draw();
	ImageDraw();
	//if (std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() >= 5)
	//{
	//	begin = end;
	//	fpsCnt_++;
	//}
	//_dbgDrawFormatString(100, 0, 0x000000, "%d",Player::fallCount/fpsCnt_);
	objlist_.remove_if([&](std::shared_ptr<Obj>obj) {
		bool flag = obj->GetDeleteFlag();
		if (flag)
		{
			if (obj->GetOwnerID().first == ObjType::Bomb &&obj->GetOwnerID().second == lpNetWork.PlayerID().first)
			{
				for (auto& list : objlist_)
				{
					if (list->GetOwnerID().first == ObjType::Player && list->GetOwnerID().second == lpNetWork.PlayerID().first)
					{
						list->SetBombBool(obj->GetSelfID() - lpNetWork.PlayerID().first-1,false);
					}
				}
			}
		}
		return  flag;
		
		});
	
	ResultCheck();
	
	if (lpNetWork.GetActive() == ActiveState::Lost || (CheckHitKey(KEY_INPUT_ESCAPE) && lpNetWork.GetNetWorkMode() == NetWorkMode::OFFLINE)){
		lpTiledLoader.Destroy();
		lpNetWork.Destroy();
		return std::make_unique<RotationScene>(std::move(own), std::make_unique<LoginScene>());
	}
	else if (lpNetWork.GetActive() == ActiveState::Result){
		return std::make_unique<CrossOverScene>(std::move(own),std::make_unique<ResultScene>(screenID,playerID.first));
	}

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

void GameScene::NoBackDraw(void)
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
				DrawRotaGraph(x * 32 + 16, y * 32 + 16, 1.0f, 0.0f, Image[no], true);
			}
			x++;
			if (x >= std::atoi(num["width"].c_str())) { y++; x = 0; }
		}
	}
}

void GameScene::Draw(double ex, double rad)
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

void GameScene::SetBomb(int ownerID, int selfID, Vector2 pos,TimeP now, double bombtime,int length,bool sendNet)
{
	int tile = std::atoi(num["tilewidth"].c_str());
	if (sendNet)
	{
		MesPacket data;
		data.resize(7);
		data[0].iData = ownerID;
		data[1].iData = selfID;
		data[2].iData = pos.x;
		data[3].iData = pos.y;
		data[4].iData = length;
		time.now = lpSceneMng.GetNowTime();
		data[5].uiData = time.inow[0];
		data[6].uiData = time.inow[1];

		lpNetWork.SendMesAll(MesType::SET_BOMB, data);
	}
	else
	{
		pos = pos / tile * tile + (tile/2);
	}
	objlist_.emplace_back(std::make_shared<Bomb>(ownerID,selfID,pos,now,bombtime,length,wall_));
}

int GameScene::BombCount(int ownerID)
{
	int count = 0;
	for (auto& obj : objlist_)
	{
		if (obj->GetOwnerID().first == ObjType::Bomb && obj->GetOwnerID().second == ownerID)
		{
			count++;
		}
	}
	return count;
}

void GameScene::ImageDraw()
{
	__int64 seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - lpNetWork.TimeStart().now).count();
	if (seconds <= START_LIMIT)
	{
		SetFontSize(32);
		DrawFormatString(lpSceneMng.GetScreenSize().x / 2 - GetFontSize() * 4 + 5, lpSceneMng.GetScreenSize().y / 2 + 5, 0x000000, "開始まで　%d　秒", (START_LIMIT - seconds) / 1000);
		DrawFormatString(lpSceneMng.GetScreenSize().x / 2 - GetFontSize() * 4, lpSceneMng.GetScreenSize().y / 2, 0xff00ff, "開始まで　%d　秒", (START_LIMIT - seconds) / 1000);
		SetFontSize(16);
	}
	else {
		for (auto& obj : objlist_)
		{
			obj->Update();
		}
	}
	for (auto& obj : objlist_)
	{
		obj->Draw();
	}
	fire_->Update();
}

void GameScene::ResultCheck()
{
	if (lpNetWork.GetNetWorkMode() != NetWorkMode::GUEST)
	{
		MesPacket result;
		result.resize(5);
		int i = 0;
		int count_ = 0;
		bool end_ = true;
		for (auto& obj : objlist_)
		{
			if (obj->GetAlive()) {
				i++;
				result[0].iData = obj->GetNo();
				count_++;
			}
			if (obj->GetOwnerID().first == ObjType::Bomb)
			{
				end_ = false;
			}
		}
		if (2 > i && end_) {
			lpNetWork.SetActive(ActiveState::Result);
			if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST) {
				auto list = lpNetWork.GetDeathNote();
				list.reverse();
				for (auto& data : list) {
					result[count_++].iData = data;
					if (count_ == 5) {
						break;
					}
				}
				for (int i = count_; i < 5; i++)
				{
					result[i].iData = -1;
				}
				std::array<int, 5> tmp;
				int i = 0;
				for (auto& res : tmp)
				{
					res = result[i++].iData;
				}
				lpNetWork.SetResult(tmp);
				lpNetWork.SendMesAll(MesType::RESULT,result);
			}
		}
	}
}
