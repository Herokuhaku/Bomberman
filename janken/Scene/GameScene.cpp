#include <DxLib.h>
#include "GameScene.h"
#include "SceneMng.h"
#include "../Graphic/ImageMng.h"
#include "../NetWork/NetWork.h"
#include "../Obj/Player.h"

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
	//if (lpNetWork.GetNetWorkMode() == NetWorkMode::GUEST)
	{
		int i = 0;
		for (auto& map : mapdata_["Char"])
		{
			if (map != -1)
			{
				objlist_.emplace_back(std::make_shared<Player>(Vector2({i%21*32,i/21*32}), Vector2{ 32,51 }, wall_));
			}
			i++;
		}
		//int size = lpNetWork.RevPosSize();
		//for (int c = 0;c < size;c++)
		//{
		//	std::vector<unionData> rev = lpNetWork.TakeOutRevData(c);
		//	Vector2 tmp = {};
		//	int i = 0;
		//	int id = 0;
		//	bool flag = false;
		//	for (auto& d : rev)
		//	{
		//		if (i % 3 == 0)
		//		{
		//			id = d.iData;
		//		}
		//		else if (i % 3 == 1)
		//		{
		//			tmp.x = d.iData;
		//		}
		//		else if (i % 3 == 2)
		//		{
		//			tmp.y = d.iData;
		//			objlist_.emplace_back(std::make_shared<Player>(tmp, Vector2{ 32,51 }, wall_));
		//		}
		//		i++;
		//	}
		//}
	}
//	else
	//{
	//	Vector2 tmp[2] = {{32,32},{ 32 * 19, 32 * 15 }};
	//	objlist_.emplace_back(std::make_shared<Player>(tmp[0], Vector2{ 32,51 }, wall_));
	//	objlist_.emplace_back(std::make_shared<Player>(tmp[1], Vector2{ 32,51 }, wall_));
	//	if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST)
	//	{
	//		MesData data;
	//		for (int i = 0; i < 2; i++)
	//		{
	//			data.emplace_back(i);
	//			data.emplace_back(tmp[i].x);
	//			data.emplace_back(tmp[i].y);
	//		}
	//		//lpNetWork.SendMesData(MesType::POS,data);
	//	}
	//}
}

std::unique_ptr<BaseScene> GameScene::Update(std::unique_ptr<BaseScene> own)
{
	Draw();
	for (auto& obj : objlist_)
	{
		obj->Update();
		obj->Draw();
	}
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
