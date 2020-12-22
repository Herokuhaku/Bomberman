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
	SetNet,
	SetUpdate,
	SetSaveIp,
	IpNotBeFound,
	MAX
};

struct Color
{
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
};

enum class ScreenChangeMode
{
	CrossOver,
	Rotation,
	OpenClose,
	OpenClose_ver2,
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
	void NoBackDraw(int scr)override;
	void Draw(double ex, double rad)override;

private:
	void KeyLoad(void);
	bool SetNetWorkMode(void);
	bool StartInit(void);
	bool inHostIp(void);
	bool GamePlay(void);
	bool Matching(void);
	bool SetNet(void);
	bool SetUpdate(void);
	bool SetSaveIp(void);
	bool IpNotBeFound(void);

	void SendData();
	void NumInput(void);
	bool Trg(int id);

	void ViewIP(Vector2& tmpos, std::array<IPDATA, 5>& ip,int fsize);
	bool InsertView(Vector2& tmpos);			// inputを描画する。Enterを押したらtrueを返す
	std::map<UpdateMode, std::function<bool(void)>> titleRun_;
	int screen_size_x_;
	int screen_size_y_;

	int plimage_;
	Vector2 pos_;
	Vector2 fpos_;
	int page_;
	const int pagecount_;
	bool sendpos_;
	UpdateMode updateMode_;

	bool savehostip;
	IPDATA hostip_;
	GuestMode haveip_;
	std::vector<std::string> inputKey;
	unsigned char oldbuf_[256];
	unsigned char nowbuf_[256];

	chronoi starttime_;
	std::chrono::system_clock::time_point end;

	bool netWorkRunflag_;
	Color col_;

	bool connect_;
	std::chrono::system_clock::time_point overtime_;

	int netno_;
	std::pair<bool,int> ipfirst_;
	
	int backframe_;
	ScreenChangeMode change_;
};

