#include "TitleScene.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include"../NetWork/NetWork.h"
#include "../_debug/_DebugConOut.h"
#include "../AllControl/ButtonMng.h"
#include "../Graphic/ImageMng.h"
TitleScene::TitleScene()
{
	Init();
	titleRun_[UpdateMode::SetNetWorkMode] = std::bind(&TitleScene::SetNetWorkMode, this);
	titleRun_[UpdateMode::StartInit] = std::bind(&TitleScene::StartInit, this);
	titleRun_[UpdateMode::inHostIp] = std::bind(&TitleScene::inHostIp, this);
	titleRun_[UpdateMode::GamePlay] = std::bind(&TitleScene::GamePlay, this);
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init(void)
{
	//plimage_ = LoadGraph("image/謎のにこちゃん.png");
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
			if (d == '\r' || d == '\n') {continue;}
			mapdata_[data.first].emplace_back(d-1);
			//mapdata_[data.first].emplace_back(',');
		}
	}
	tsxdata_ = lpTiledLoader.ReadTsx("Tiled/mapdata/map");
	lpTiledLoader.TmxCsv();

	lpImageMng.GetID("Block", tsxdata_.pass.c_str(),
		{std::atoi(tsxdata_.num["tilewidth"].c_str()),std::atoi(tsxdata_.num["tileheight"].c_str()) },
		{ std::atoi(tsxdata_.num["columns"].c_str()),
		std::atoi(tsxdata_.num["tilecount"].c_str()) / std::atoi(tsxdata_.num["columns"].c_str()) })[0];
	lpImageMng.GetID("Block", "Tiled/imagedata/image/map.png", { 32,32 }, { 4,3 })[0];
	//mapdata_ = lpTiledLoader.ReadTmx("Tiled/mapdata/map.tmx");

	layer_.try_emplace("Bg"		,LAYER::BG);
	layer_.try_emplace("Char"	,LAYER::CHAR);
	layer_.try_emplace("Obj"	,LAYER::OBJ);
	layer_.try_emplace("Item"	,LAYER::ITEM);


	lpNetWork.Update();
}

std::unique_ptr<BaseScene> TitleScene::Update(std::unique_ptr<BaseScene> own)
{
	if (updateMode_ != UpdateMode::SetNetWorkMode && lpNetWork.GetNetWorkMode() == NetWorkMode::NON)
	{
		updateMode_ = UpdateMode::SetNetWorkMode;
	}
	titleRun_[updateMode_]();
	lpButtonMng.Run();
	//lpNetWork.newUpdate();
	Draw();
	return own;
}

void TitleScene::Draw(void)
{
	for (auto& data : mapdata_)
	{
		int x = 0 ,y = 0;
		for (auto& no : data.second)
		{
			if (0 <= no && 12 > no) {
				lpImageMng.AddDraw({IMAGE_ID("Block")[no],x * 32+16,y*32+16,1.0f,0.0f,layer_[data.first],100 });
			}
			x++;
			if (x >= 21) { y++; x = 0; }
		}
	}
	//lpImageMng.AddDraw({ lpImageMng.GetID("image/謎のにこちゃん.png")[0],pos_.x,pos_.y,1.0f,0.0f,LAYER::BG,100 });
}

void TitleScene::SetNetWorkMode(void)
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

void TitleScene::StartInit(void)
{
	if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST && lpNetWork.GetActive() == ActiveState::Init){
	pos_ = { 250,250 };

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
		lpNetWork.SendStart();
		pos_ = { 250,250 };
	}
	if (lpNetWork.GetActive() == ActiveState::Play)
	{
		updateMode_ = UpdateMode::GamePlay;
		TRACE("ゲームを開始します！\n");
	}
}

void TitleScene::inHostIp(void)
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

void TitleScene::GamePlay(void)
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
		sendpos_ = !sendpos_;
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

void TitleScene::SendData()
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
	int count = -1;
	for(auto& chip : _data)
	{
		//tmp |= data << (co % 2)*4;
	//	tmp |= test[co - 1] << co % 2 * 4;

		tmp |= chip << co % 2 * 4;
		if (++co % 2 == 0 && co != 0) {
			c[i++] = tmp;
			tmp = 0;
		}
		if (co == 16) {
			lpNetWork.SendMes({ MesType::TMX_DATA,id++,0, j[0],j[1] });
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
		lpNetWork.SendMes({ MesType::TMX_DATA,id++,0, j[0],j[1] });
	}
}
