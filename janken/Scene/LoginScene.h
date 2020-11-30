#pragma once
#include <DxLib.h>
#include <map>
#include <functional>
#include <chrono>
#include <array>
#include "../TiledLoader.h"
#include "BaseScene.h"
#include "../common/Vector2.h"
#include "../Graphic/LAYER.h"

#define COUNT_LIMIT 3000

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
	Matching,
	GamePlay,
	MAX
};

class LoginScene :
	public BaseScene
{
public:
	LoginScene();
	~LoginScene();
	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own) override;
	void Draw(void)override;
	void Draw(float ex, float rad)override;

private:
	void KeyLoad(void);
	void SetNetWorkMode(void);
	void StartInit(void);
	void inHostIp(void);
	void GamePlay(void);
	void Matching(void);

	void SendData();
	void NumPadInput(void);
	bool Trg(int id);

	void ViewIP(Vector2& tmpos, std::array<IPDATA, 5>& ip,int fsize);
	std::map<UpdateMode, std::function<void(void)>> titleRun_;
	int screen_size_x_;
	int screen_size_y_;

	int plimage_;
	Vector2 pos_;
	Vector2 fpos_;
	bool sendpos_;
	UpdateMode updateMode_;

	bool savehostip;
	IPDATA hostip_;
	GuestMode haveip_;
	std::vector<std::string> inputKey;
	char oldbuf[256];
	char nowbuf[256];

	chronoi starttime_;
	std::chrono::system_clock::time_point end;
	/*std::map<std::string,LAYER> layer_;
	std::map<std::string,std::vector<int>> mapdata_;*/
};

