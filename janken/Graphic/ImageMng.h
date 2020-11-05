#pragma once
#include <memory>
#include <map>
#include <vector>
#include "ImageMng.h"
#include "LAYER.h"
#include "../common/Vector2.h"

using VecInt = std::vector<int>;
using DrawData = std::tuple<int, int, int, double, double, LAYER, int>;	// 描画用データ　画像ID, 座標x, y, 拡大率、角度, レイヤー, zオーダー

#define lpImageMng ImageMng::GetInstance()
#define IMAGE_ID(KEY)(ImageMng::GetInstance().GetID(KEY))

class ImageMng
{
public:
	static ImageMng &GetInstance(void)
	{
		return *sInstance;
	}

	const VecInt & GetID(const std::string& key);
	const VecInt & GetID(const std::string& key, const std::string& fileName);
	const VecInt & GetID(const std::string& key, const std::string& fileName,
		const Vector2& divSize,const Vector2& divCnt);
	void Draw(void);
	// 描画用データ　画像ID, 座標x, y, 拡大率、角度, レイヤー, zオーダー
	void AddDraw(DrawData data);
	void Init(void);
private:
	struct ImageMngDeleter
	{
		void operator()(ImageMng * imageMng)const // 最初のカッコはオペレータのオーバーロードしたいための()
		{
			delete imageMng;
		}
	};
	ImageMng();
	~ImageMng();
	static std::unique_ptr<ImageMng,ImageMngDeleter>sInstance;

	std::map<std::string,VecInt> ImageMap;
	std::vector<DrawData> drawlist_;
	int screen_;
};
//	static void Destroy()
//
//	if (sInstance != nullptr)
//	{
//		delete sInstance;
//	}
//	sInstance = nullptr;
//}
//
