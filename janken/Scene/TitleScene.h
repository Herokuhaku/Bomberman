#pragma once
#include <DxLib.h>
#include <map>
#include <functional>
#include <chrono>
#include "../TiledLoader.h"
#include "BaseScene.h"
#include "../common/Vector2.h"
#include "../Graphic/LAYER.h"

enum class GuestMode
{
	NOIP,
	IP
};

enum class UpdateMode
{
	SetNetWorkMode,
	inHostIp,
	StartInit,
	GamePlay,
	MAX
};

class TitleScene :
	public BaseScene
{
public:
	TitleScene();
	~TitleScene();
	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own) override;
	void Draw(void)override;
private:
	void SetNetWorkMode(void);
	void StartInit(void);
	void inHostIp(void);
	void GamePlay(void);

	void SendData();
	std::map<UpdateMode, std::function<void(void)>> titleRun_;
	int screen_size_x_;
	int screen_size_y_;

	int plimage_;
	Vector2 pos_;
	bool sendpos_;
	UpdateMode updateMode_;

	bool savehostip;
	IPDATA hostip_;
	GuestMode haveip_;
	TmxData tmxdata_;
	TsxData tsxdata_;
	std::map<std::string,LAYER> layer_;

	std::map<std::string,std::vector<int>> mapdata_;
};

