#include <DxLib.h>
#include "CrossOverScene.h"
#include "SceneMng.h"
#include "../Graphic/ImageMng.h"

CrossOverScene::CrossOverScene(std::unique_ptr<BaseScene> old, std::unique_ptr<BaseScene> next)
{
	old_ = std::move(old);
	next_ = std::move(next);
}

CrossOverScene::~CrossOverScene()
{
}

void CrossOverScene::Init(void)
{
	screenID = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y);

	tmxdata_ = lpTiledLoader.ReadTmx("Tiled/mapdata/map");
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
	lpTiledLoader.TmxCsv();

	//lpImageMng.GetID("Block", tsxdata_.pass.c_str(),
	//	{ std::atoi(tsxdata_.num["tilewidth"].c_str()),std::atoi(tsxdata_.num["tileheight"].c_str()) },
	//	{ std::atoi(tsxdata_.num["columns"].c_str()),
	//	std::atoi(tsxdata_.num["tilecount"].c_str()) / std::atoi(tsxdata_.num["columns"].c_str()) })[0];
	//lpImageMng.GetID("Block", "Tiled/imagedata/image/map.png", { 32,32 }, { 4,3 })[0];
	//mapdata_ = lpTiledLoader.ReadTmx("Tiled/mapdata/map.tmx");

	//layer_.try_emplace("Bg", LAYER::BG);
	//layer_.try_emplace("Char", LAYER::CHAR);
	//layer_.try_emplace("Obj", LAYER::OBJ);
	//layer_.try_emplace("Item", LAYER::ITEM);
}

std::unique_ptr<BaseScene> CrossOverScene::Update(std::unique_ptr<BaseScene> own)
{
	if (!Transition())
	{
		return std::move(next_);
	}
	BaseScene::Draw();
	return own;
}

void CrossOverScene::DrawOwnScreen(void)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();

	SetDrawBlendMode(DX_BLENDMODE_ALPHA,255-count_);
	old_->Draw();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA,count_);
	next_->Draw();
}

bool CrossOverScene::Transition(void)
{
	DrawOwnScreen();
	return ++count_ < 255;
}
