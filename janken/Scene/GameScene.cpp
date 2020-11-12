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
	else {
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
	while (ProcessMessage() == 0)
	{
		if (lpNetWork.GetActive() == ActiveState::Play)
		{
			break;
		}
	}
	if (lpNetWork.GetNetWorkMode() == NetWorkMode::GUEST)
	{
		std::vector<unionData> rev = lpNetWork.TakeOutRevData();
		Vector2 tmp = {};
		int i = 0;
		for (auto& d : rev)
		{
			if (++i % 2 == 0) {
				tmp.y = d.iData;
				objlist_.emplace_back(std::make_shared<Player>(tmp, Vector2{ 32,51 },wall_));
			}
			else
			{
				tmp.x = d.iData;
			}
		}

	}
	//objlist_.emplace_back(std::make_shared<Player>(Vector2{32,32}, Vector2{32,51},MOVE_TYPE::ME));
	//objlist_.emplace_back(std::make_shared<Player>(Vector2{21*32 - (32*2), 17*32 - (32*2) }, Vector2{ 32,51 },MOVE_TYPE::YOUR));
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
	for (auto& data : mapdata_) {
		int x = 0, y = 0;
		for (auto& no : data.second)
		{
			if (0 <= no && 12 > no) {
				//lpImageMng.AddDraw({ IMAGE_ID("Block")[no],x * 32 + 16,y * 32 + 16,1.0f,0.0f,layer_[data.first],100 });
				DrawRotaGraph(x * 32 + 16, y * 32 + 16, 1.0f, 0.0f,Image[no], true);
			}
			x++;
			if (x >= 21) { y++; x = 0; }
		}
	}
}
