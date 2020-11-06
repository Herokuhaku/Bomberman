#include "LoginScene.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include"../NetWork/NetWork.h"
#include "../_debug/_DebugConOut.h"
#include "../AllControl/ButtonMng.h"
#include "../Graphic/ImageMng.h"
#include "CrossOverScene.h"
#include "GameScene.h"
LoginScene::LoginScene()
{
	Init();
	titleRun_[UpdateMode::SetNetWorkMode] = std::bind(&LoginScene::SetNetWorkMode, this);
	titleRun_[UpdateMode::StartInit] = std::bind(&LoginScene::StartInit, this);
	titleRun_[UpdateMode::inHostIp] = std::bind(&LoginScene::inHostIp, this);
	titleRun_[UpdateMode::GamePlay] = std::bind(&LoginScene::GamePlay, this);
}

LoginScene::~LoginScene()
{
}

void LoginScene::Init(void)
{
	plimage_ = LoadGraph("image/謎のにこちゃん.png");
	pos_ = { 100,100 };

	sendpos_ = false;
	savehostip = false;

	screen_size_x_ = 0;
	screen_size_y_ = 0;
	GetDrawScreenSize(&screen_size_x_, &screen_size_y_);
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

	lpNetWork.Update();
}

std::unique_ptr<BaseScene> LoginScene::Update(std::unique_ptr<BaseScene> own)
{
	if (updateMode_ != UpdateMode::SetNetWorkMode && lpNetWork.GetNetWorkMode() == NetWorkMode::NON)
	{
		updateMode_ = UpdateMode::SetNetWorkMode;
	}
	titleRun_[updateMode_]();
	lpButtonMng.Run();
	Draw();
	if (updateMode_ == UpdateMode::GamePlay)
	{
		return std::make_unique<CrossOverScene>(std::move(own), std::make_unique<GameScene>());
	}
	//lpNetWork.newUpdate();
	return own;
}

void LoginScene::Draw(void)
{
	DrawGraph(0, 0, plimage_, true);
	//lpImageMng.AddDraw({ lpImageMng.GetID("image/謎のにこちゃん.png")[0],pos_.x,pos_.y,1.0f,0.0f,LAYER::BG,100 });
}

void LoginScene::OnlyDraw(void)
{
	DrawGraph(0, 0, plimage_, true);
}

void LoginScene::SetNetWorkMode(void)
{
	auto tmpip = lpNetWork.GetIP();
	for (auto& ip : tmpip)
	{
		if (ip.d1 == 0)
		{
			continue;
		}
		if (ip.d1 != 192)
		{
			TRACE("ローカル  :IPアドレス : %d.%d.%d.%d\n", ip.d1, ip.d2, ip.d3, ip.d4);
		}
		else
		{
			TRACE("グローバル :IPアドレス : %d.%d.%d.%d\n", ip.d1, ip.d2, ip.d3, ip.d4);
		}
	}
	
	std::ifstream ifs("HostIp/Ip.txt");
	std::string str;
	if (!ifs)
	{
		savehostip = false;
	}
	else
	{
		std::getline(ifs, str);
		if (str.size() <= 0)
		{
			savehostip = false;
		}
		else
		{
			std::string save;
			std::istringstream stream(str);
			
			auto oneip = [&]() {std::getline(stream, save, '.');
			return atoi(save.c_str());
			};
			hostip_.d1 = oneip();
			hostip_.d2 = oneip();
			hostip_.d3 = oneip();
			hostip_.d4 = oneip();

			savehostip = true;
		}
	}

	TRACE("自分の選択するモードの値を入力してください\n");
	TRACE("HOST			:0\n");
	TRACE("GUEST			:1\n");
	if (savehostip)
	{
		TRACE("GUEST【前回の接続先】	:2\n");
	}
	else { TRACE("\n") };
	TRACE("OFFLINE			:3\n\n");
	// ファイルが読み込めなかったら2番を表示しない

	int num = -1;

	while (0 > num || num > 3)
	{
		std::cin >> num;
		//scanf_s("%d", &num);
		if (num == 0)
		{
			lpNetWork.SetNetWorkMode(NetWorkMode::HOST);
		}
		else if (num == 1)
		{
			lpNetWork.SetNetWorkMode(NetWorkMode::GUEST);
			haveip_ = GuestMode::NOIP;
		}
		else if (num == 2 && savehostip)
		{
			lpNetWork.SetNetWorkMode(NetWorkMode::GUEST);
			haveip_ = GuestMode::IP;
		}
		else if (num == 3)
		{
			lpNetWork.SetNetWorkMode(NetWorkMode::OFFLINE);
		}
		else if (num != -1)
		{
			TRACE("選択できないモードです\n");
			num = -1;
		}
	}
	switch (lpNetWork.GetNetWorkMode())
	{
	case NetWorkMode::HOST:
		TRACE("ホストになりました\n");
		updateMode_ = UpdateMode::StartInit;
		break;
	case NetWorkMode::GUEST:
		if (haveip_ == GuestMode::NOIP)
		{
			TRACE("ゲストになりました\n");
			TRACE("IPを入力してください\n");
		}
		else {
			TRACE("前回の接続先へ繋ぎます\n");
			TRACE("検索中...\n");
		}
		updateMode_ = UpdateMode::inHostIp;
		break;
	case NetWorkMode::OFFLINE:
		TRACE("オフラインモードです\n");
		updateMode_ = UpdateMode::StartInit;
		break;
	default:
		TRACE("\n\n存在しないモードです\n");
		break;
	}
}

void LoginScene::StartInit(void)
{
	if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST && lpNetWork.GetActive() == ActiveState::Init){
	pos_ = { 250,250 };
	MesData tmp;
	lpNetWork.SendTmxSize();
	SendData();
	lpNetWork.SendStandby();
	}
	if (lpNetWork.GetNetWorkMode() == NetWorkMode::OFFLINE)
	{
		updateMode_ = UpdateMode::GamePlay;
	}
	if (lpNetWork.GetRevStandby())
	{

		TRACE("送られてきた初期情報で初期化したよ\n\n\n");
		tmxdata_ = lpTiledLoader.ReadTmx("Tiled/mapdata/tmp");
		lpNetWork.SendStart();
		pos_ = { 250,250 };
	}
	if (lpNetWork.GetActive() == ActiveState::Play)
	{
		updateMode_ = UpdateMode::GamePlay;
		TRACE("ゲームを開始します！\n");
	}
}

void LoginScene::inHostIp(void)
{
	IPDATA hostip = {};
	bool state = false;

	if (haveip_ == GuestMode::IP)
	{
		while (!state && !CheckHitKey(KEY_INPUT_END))
		{
			lpNetWork.ConnectHost(hostip_);
			TRACE("IP : %d.%d.%d.%d", hostip_.d1, hostip_.d2, hostip_.d3, hostip_.d4);
			state = lpNetWork.GetActive() == ActiveState::Init;
			TRACE("状態は %d です\n", state);
		}
	}
	else if (haveip_ == GuestMode::NOIP)
	{
		do
		{
			std::string _ip;
			std::string save;

			std::cin >> _ip;

			std::istringstream stream(_ip);
			auto oneip = [&]() {std::getline(stream, save, '.');
			return atoi(save.c_str());
			};
			hostip.d1 = oneip();
			hostip.d2 = oneip();
			hostip.d3 = oneip();
			hostip.d4 = oneip();

			lpNetWork.ConnectHost(hostip);
			TRACE("IP : %d.%d.%d.%d", hostip.d1, hostip.d2, hostip.d3, hostip.d4);
			state = lpNetWork.GetActive() == ActiveState::Init;
			TRACE("状態は %d です\n", state);
			if (!state) {
				TRACE("IPを入力してください\n");
			}
		} while (!state);
		remove("HostIp/Ip.txt");
		std::ofstream("HostIp/Ip.txt");
		std::ofstream ofs("HostIp/Ip.txt");

		ofs << std::to_string(hostip.d1) << " ." <<
			std::to_string(hostip.d2) << "." <<
			std::to_string(hostip.d3) << "." <<
			std::to_string(hostip.d4) << std::endl;
	}
	if (state == 1) {
		updateMode_ = UpdateMode::StartInit;
	}
	else
	{
		TRACE("接続をキャンセルしてモード選択に戻ります\n");
		updateMode_ = UpdateMode::SetNetWorkMode;
	}
}

void LoginScene::GamePlay(void)
{
	if (lpButtonMng.Thumbf(0, XINPUT_THUMBL_X).first == 2)
	{
		pos_.x-=5;
	}
	if (lpButtonMng.Thumbf(0, XINPUT_THUMBL_X).first == 1)
	{
		pos_.x+=5;
	}
	if (lpButtonMng.Thumbf(0, XINPUT_THUMBL_Y).first == 2)
	{
		pos_.y-=5;
	}
	if (lpButtonMng.Thumbf(0, XINPUT_THUMBL_Y).first == 1)
	{
		pos_.y+=5;
	}

	if (lpButtonMng.Buttonf(0,XINPUT_BUTTON_B).first &&
		!lpButtonMng.Buttonf(0, XINPUT_BUTTON_B).second)
	{
		//sendpos_ = !sendpos_;

	}

	int buf = 0;
	int num = 0;
	buf = 1234;

	//if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) >= sizeof(MesData))
	//{
	//	MesData tmp = {MesType::NON,0,0};
	//	NetWorkRecv(lpNetWork.GetNetWorkHandle(), &tmp, sizeof(MesData));
	//	if (tmp.type == MesType::POS)
	//	{
	//		TRACE("X : %d   Y : %d  が送られてきたよ\n", tmp.data[0],tmp.data[1]);
	//	}
	//}

	//if (sendpos_)
	//{
	//	MesData tmp;
	//	tmp = { MesType::POS,pos_.x,pos_.y };
	//	lpNetWork.SendMes(tmp);
	//	TRACE("X : %d   Y : %d  を送ったよ\n", pos_.x, pos_.y);
	//}
}

void LoginScene::SendData()
{
	int j[2] = {0,0};
	unsigned char* c = reinterpret_cast<unsigned char*>(&j);
	unsigned short id = 0;
	int i = 0;
	int co = 0;
	unsigned char tmp = 0;
	std::vector<unsigned char> _data;
	{
		for (auto& data : tmxdata_.MapData["Bg"])
		{
			_data.emplace_back(data);
		}
		for (auto& data : tmxdata_.MapData["Item"])
		{
			_data.emplace_back(data);
		}
		for (auto& data : tmxdata_.MapData["Obj"])
		{
			_data.emplace_back(data);
		}
		for (auto& data : tmxdata_.MapData["Char"])
		{
			_data.emplace_back(data);
		}
	}
	int count = 0;
	std::vector<int> senddata;
	//for (auto& chip : _data)
	//{
	//	senddata.emplace_back(chip);
	//}
	for(auto& chip : _data)
	{
		tmp |= chip << co % 2 * 4;
		if (++co % 2 == 0 && co != 0) {
			c[i++] = tmp;
			tmp = 0;
		}
		if (co == 16) {
			senddata.emplace_back(j[0]);
			senddata.emplace_back(j[1]);
			i = 0;
			co = 0;
		}
	}
	if (co != 0)
	{
		while (i < 8) {
			tmp |= 0 << (co++ % 2) * 4;
			c[i++] = tmp;
		}
		senddata.emplace_back(j[0]);
		senddata.emplace_back(j[1]);
	}

	while (senddata.size() > MAXSENDBYTE/sizeof(int))
	{
		//MesHeader data = { MesType::TMX_DATA,0,0,MAXSENDBYTE };
		MesData mesdata = lpNetWork.SendMesHeader({ MesType::TMX_DATA,0,0,MAXSENDBYTE/sizeof(int)});
		for (int i = 0; i < MAXSENDBYTE/4; i++)
		{
			mesdata.emplace_back(senddata[i]);
		}
		senddata.erase(senddata.begin(), senddata.begin() + MAXSENDBYTE/4);
		lpNetWork.SendMesData(mesdata);
	}
	if (senddata.size() > 0)
	{
		MesData mesdata = lpNetWork.SendMesHeader({ MesType::TMX_DATA,0,0,static_cast<unsigned int>(senddata.size())});
		int c = 0;
		for (auto d : senddata)
		{
			mesdata.emplace_back(d);
			c++;
		}
		senddata.erase(senddata.begin(), senddata.begin() + c);
		lpNetWork.SendMesData(mesdata);
	}
}
