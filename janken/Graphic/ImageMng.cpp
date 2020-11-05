#include <DxLib.h>
#include <string>
#include <mutex>
#include "../common/Vector2.h"
#include "ImageMng.h"
#include "../Scene/SceneMng.h"


std::unique_ptr<ImageMng,ImageMng::ImageMngDeleter> ImageMng::sInstance(new ImageMng());
// ÉÜÅ[ÉUÇ≈ä«óùÇ≈Ç´Ç»Ç¢ÇÊÇ§Ç…

const VecInt & ImageMng::GetID(const std::string & key)
{
	return GetID(key, key);
}

const VecInt & ImageMng::GetID(const std::string & key, const std::string & fileName)	// 1ñáäG
{
	if (ImageMap.find(key) == ImageMap.end())
	{
		ImageMap[key].resize(1);
		ImageMap[key][0] = LoadGraph(fileName.c_str());
	}
	return ImageMap[key];
}

const VecInt & ImageMng::GetID(const std::string & key, const std::string & fileName,
	const Vector2 & divSize, const Vector2 & divCnt)
{
	if (ImageMap.find(key) == ImageMap.end())//iterator
	{
		ImageMap[key].resize(divCnt.x * divCnt.y);
		LoadDivGraph(fileName.c_str(), divCnt.x * divCnt.y, divCnt.x, divCnt.y, divSize.x,
			divSize.y, &ImageMap[key][0]);
	}
	return ImageMap[key];
}

void ImageMng::Draw(void)
{
	SetDrawScreen(screen_);
	ClsDrawScreen();

	int id, x, y;
	double exRate, rad;
	LAYER layer;

	for (const auto& data : drawlist_)
	{
		std::tie(id, x, y, exRate, rad, layer, std::ignore) = data;
			DrawRotaGraph(x, y, exRate, rad, id, true);
	}

	SetDrawScreen(DX_SCREEN_BACK);
	ClsDrawScreen();
	DrawGraph(0, 0, screen_, true);

	ScreenFlip();

	drawlist_.clear();
}
void ImageMng::AddDraw(DrawData data)
{
	drawlist_.emplace_back(data);
}

void ImageMng::Init(void)
{
	screen_ = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y, false);
}

ImageMng::ImageMng()
{
	lpSceneMng.Create();
	Init();
}


ImageMng::~ImageMng()
{
}
