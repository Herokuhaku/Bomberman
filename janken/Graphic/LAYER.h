#pragma once


// レイヤー
enum class LAYER
{
	BG,				// 背景
	ITEM,			// キャラクター
	OBJ,			// アイテム
	CHAR,			// オブジェ
	MAX
};

LAYER begin(LAYER);
LAYER end(LAYER);

LAYER operator++(LAYER& key);
LAYER operator*(LAYER key);