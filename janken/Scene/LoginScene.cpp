#include "LoginScene.h"
#include <winuser.h>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <mutex>
#include"../NetWork/NetWork.h"
#include "../_debug/_DebugConOut.h"
#include "../Graphic/ImageMng.h"
#include "CrossOverScene.h"
#include "RotationScene.h"
#include "OpenCloseScene.h"
#include "GameScene.h"
#include "SceneMng.h"

LoginScene::LoginScene() :starttime_{ std::chrono::system_clock::now() },pagecount_(10)
{
	Init();
	titleRun_[UpdateMode::SetNetWorkMode] = std::bind(&LoginScene::SetNetWorkMode, this);
	titleRun_[UpdateMode::StartInit] = std::bind(&LoginScene::StartInit, this);
	titleRun_[UpdateMode::inHostIp] = std::bind(&LoginScene::inHostIp, this);
	titleRun_[UpdateMode::GamePlay] = std::bind(&LoginScene::GamePlay, this);
	titleRun_[UpdateMode::Matching] = std::bind(&LoginScene::Matching, this);
	titleRun_[UpdateMode::SetNet] = std::bind(&LoginScene::SetNet, this);
	titleRun_[UpdateMode::SetUpdate] = std::bind(&LoginScene::SetUpdate, this);
	titleRun_[UpdateMode::SetSaveIp] = std::bind(&LoginScene::SetSaveIp, this);
	titleRun_[UpdateMode::IpNotBeFound] = std::bind(&LoginScene::IpNotBeFound, this);
}

LoginScene::~LoginScene()
{
}

void LoginScene::Init(void)
{
	plimage_ = LoadGraph("image/謎のにこちゃん.png");
	pos_ = { 0,0 };
	fpos_ = { 0,32 };
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

	screenID = MakeScreen(lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y);
	updateMode_ = UpdateMode::SetNetWorkMode;
	netWorkRunflag_ = false;
	col_.Red = rand() % 128;
	col_.Blue = rand() % 128;
	col_.Green = rand() % 128;
	connect_ = false;
	ipfirst_ = { false,-1};
	page_ = 0;
	backframe_ = 0;
}

std::unique_ptr<BaseScene> LoginScene::Update(std::unique_ptr<BaseScene> own)
{
	Draw();
	if ((updateMode_ != UpdateMode::SetNetWorkMode && updateMode_ != UpdateMode::SetNet && updateMode_ != UpdateMode::SetUpdate) &&
		lpNetWork.GetNetWorkMode() == NetWorkMode::NON)
	{
		updateMode_ = UpdateMode::SetNetWorkMode;
	}

	if (!titleRun_[updateMode_]())
	{
		return std::make_unique<OpenCloseScene>(std::move(own), std::make_unique<GameScene>());
		//return std::make_unique<CrossOverScene>(std::move(own), std::make_unique<GameScene>());
	}
	KeyLoad();

	if (Trg(VK_ESCAPE))
	{
		lpTiledLoader.Destroy();
		lpNetWork.Destroy();
		fpos_ = { 0,32 };
	}
	return own;
}

void LoginScene::Draw(void)
{
	DrawBox(0, 0, lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y, GetColor(col_.Red, col_.Green, col_.Blue), true);
}

void LoginScene::NoBackDraw(void)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();
	DrawBox(0, 0, lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y, GetColor(col_.Red, col_.Green, col_.Blue), true);
}

void LoginScene::Draw(double ex, double rad)
{
	SetDrawScreen(screenID);
	ClsDrawScreen();

	DrawBox(0,0,lpSceneMng.GetScreenSize().x, lpSceneMng.GetScreenSize().y,GetColor(col_.Red,col_.Green,col_.Blue),true);

	SetDrawScreen(DX_SCREEN_BACK);
	DrawRotaGraph(lpSceneMng.GetScreenSize().x / 2, lpSceneMng.GetScreenSize().y / 2, ex, rad, screenID, true);
}

void LoginScene::KeyLoad(void)
{
	int i = 0;
	for (auto& buf : nowbuf_)
	{
		oldbuf_[i++] = buf & 0xf0;
	}
	if (!GetKeyboardState(nowbuf_))
	{
		TRACE("キーボードの状態取得失敗");
	}
	NumInput();
}

bool LoginScene::SetNetWorkMode(void)
{
	Vector2 tmpos = fpos_;
	int fsize = GetFontSize();
	//int nextf = false;
	auto tmpip = lpNetWork.GetIP();

	ViewIP(tmpos, tmpip, fsize);

	int nextf = InsertView(tmpos);
	std::ifstream ifs("ini/Ip.txt");
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
			savehostip = true;
		}
	}


	auto fauto = [&](const char* name) {
		tmpos.y += fsize;
		DrawString(tmpos.x, tmpos.y, name, 0xffffff);
	};
	fauto("自分の選択するモードの値を入力してください");
	fauto("HOST                           :0");
	fauto("GUEST                          :1");
	if (savehostip)
	{
	fauto("GUEST【保存IPから探す】      　 :2");
	}
	fauto("OFFLINE                        :3");
	if (nextf)
	{
		updateMode_ = UpdateMode::SetNet;
	}
	return true;
}

bool LoginScene::StartInit(void)
{
	if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST && lpNetWork.GetActive() == ActiveState::Init){
	lpNetWork.SendTmxSize();
	SendData();
	lpNetWork.SendStandby();
	}
	if (lpNetWork.GetNetWorkMode() == NetWorkMode::OFFLINE)
	{
		lpNetWork.SetActive(ActiveState::Play);
		updateMode_ = UpdateMode::GamePlay;
	}
	if (lpNetWork.GetActive() == ActiveState::Play)
	{
		updateMode_ = UpdateMode::GamePlay;
		TRACE("ゲームを開始します！\n");
	}
	return true;
}

bool LoginScene::inHostIp(void)
{
	IPDATA hostip = {};
	bool state = false;
	bool nextf = false;
	Vector2 tmpos = fpos_;
	int fsize = GetFontSize();
	tmpos.y += fsize * 3;
	auto tmpip = lpNetWork.GetIP();
	Vector2 pos = pos_;

	ViewIP(tmpos,tmpip,fsize);			// IP表示
	tmpos.y += fsize;
	DrawString(pos.x, tmpos.y, "IPを入力してください", 0xffffff, true);
	tmpos.y += fsize;
	if (haveip_ == GuestMode::IP)
	{
		SetDrawScreen(DX_SCREEN_BACK);
		ClsDrawScreen();
		Draw();
		DrawString(tmpos.x,tmpos.y+=fsize,"検索中.....",0xffffff);
		ScreenFlip();

		lpNetWork.ConnectHost(hostip_);
		TRACE("IP : %d.%d.%d.%d", hostip_.d1, hostip_.d2, hostip_.d3, hostip_.d4);
		state = lpNetWork.GetActive() == ActiveState::Init;
		TRACE("状態は %d です\n", state);

		//while (!state)
		//{
		//	lpNetWork.ConnectHost(hostip_);
		//	TRACE("IP : %d.%d.%d.%d", hostip_.d1, hostip_.d2, hostip_.d3, hostip_.d4);
		//	state = lpNetWork.GetActive() == ActiveState::Init;
		//	TRACE("状態は %d です\n", state);
		//}
		if (state != 1) {
			inputKey.clear();
			updateMode_ = UpdateMode::IpNotBeFound;
			return true;
		}
	}
	else if (haveip_ == GuestMode::NOIP)
	{
		std::string _ip;
		std::string save;

		for (auto& key : inputKey)
		{
			DrawString(pos.x, tmpos.y, key.c_str(), 0xffffff, true);
			pos.x += GetFontSize() / 2;
			if (key == "Enter") {
				nextf = true;
				inputKey.pop_back();
			}
		}
		if (nextf)
		{
			std::string tmp;
			for (auto& key : inputKey) {
				tmp += key;
			}
			_ip = tmp;
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
				inputKey.clear();
				TRACE("IPを入力してください\n");
			}
			hostip_ = hostip;
			if (state != 1) {
				inputKey.clear();
				updateMode_ = UpdateMode::IpNotBeFound;
				return true;
			}
		}
		if (state)
		{
			std::ifstream ifs("ini/Ip.txt");
			bool flag = false;
			std::string str;

			while (std::getline(ifs, str))
			{
				std::istringstream stream(str);
				auto oneip = [&]() {std::getline(stream, save, '.');
				return atoi(save.c_str());
				};
				if (hostip.d1 != oneip())
				{
					flag = true;
					break;
				}
				if (hostip.d2 != oneip())
				{
					flag = true;
					break;
				}
				if (hostip.d3 != oneip())
				{
					flag = true;
					break;
				}
				if (hostip.d4 != oneip())
				{
					flag = true;
					break;
				}
			}
			if (flag)
			{
				std::ofstream ofs("ini/Ip.txt", std::ios::app);

				ofs << std::to_string(hostip.d1) << "." <<
					std::to_string(hostip.d2) << "." <<
					std::to_string(hostip.d3) << "." <<
					std::to_string(hostip.d4) << std::endl;
			}
		}
	}
	if (state == 1) {
		updateMode_ = UpdateMode::Matching;
	}

	//else
	//{
	//	inputKey.clear();

	//	updateMode_ = UpdateMode::IpNotBeFound;
	//}
	return true;
}

bool LoginScene::Matching(void)
{
	end = lpSceneMng.GetNowTime();
	if (lpNetWork.GetNetWorkMode() == NetWorkMode::GUEST)
	{
		if (lpNetWork.GetActive() == ActiveState::Matching || lpNetWork.GetActive() == ActiveState::Lost)
		{
			connect_ = true;
			starttime_ = lpNetWork.TimeStart();
			__int64 countdown = std::chrono::duration_cast<std::chrono::milliseconds>(end - starttime_.now).count();
			std::string str[2];
			str[0] = COUNT_LIMIT - countdown > 0 ? "開始まであと  " : "開始合図無し  ～経過時間～  ";
			str[1] = "%d ms";
			fpos_.y = GetFontSize()*3;
			DrawFormatString(pos_.x, fpos_.y+=GetFontSize(), 0xffffff, (str[0]+str[1]).c_str(), abs(COUNT_LIMIT - countdown));
			DrawString(pos_.x, fpos_.y += GetFontSize(),"モード選択へ戻る  :  ESCキー",0xffffff);
			if (countdown >= COUNT_LIMIT)
			{
				if (lpNetWork.GetRevStandby() && lpNetWork.GetNetWorkMode() == NetWorkMode::GUEST)
				{
					TRACE("送られてきた初期情報で初期化したよ\n\n\n");
					tmxdata_ = lpTiledLoader.ReadTmx("Tiled/mapdata/tmp");
					lpNetWork.SendStart();
					updateMode_ = UpdateMode::GamePlay;
				}
			}
			if (COUNT_LIMIT - countdown <= -OVER_LIMIT)
			{
				fpos_ = { 0,32 };
				lpTiledLoader.Destroy();
				lpNetWork.Destroy();
				return true;
			}
		}
		else if (lpNetWork.GetActive() == ActiveState::Init)
		{
			lpNetWork.SetActive(ActiveState::Matching);
		}
	}
	else if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST)
	{
		Vector2 tmpos = fpos_;
		int fsize = GetFontSize();
		tmpos.y += fsize * 3;
		auto tmpip = lpNetWork.GetIP();

		ViewIP(tmpos, tmpip, fsize);
		DrawString(tmpos.x, tmpos.y+=fsize, "ホスト : 接続待機中", 0xffffff, true);
		auto connect = lpNetWork.GetConnect();
		if (connect.first)
		{
			DrawString(tmpos.x, tmpos.y+=fsize, "一人目の接続を確認済", 0xffffff, true);
			if (connect.second > 0)
			{
				DrawFormatString(tmpos.x, tmpos.y += fsize, 0xffffff, "開始まで　%d ms", connect.second);
			}
			else {
				lpNetWork.SetActive(ActiveState::Init);
			}
		}
		if (lpNetWork.GetActive() == ActiveState::Init)
		{
			lpNetWork.SetListID();
			updateMode_ = UpdateMode::StartInit;
		}
	}
	return true;
}

bool LoginScene::SetNet(void)
{
	netno_  = -1;
	std::string tmp;
	for (auto& key : inputKey) {
		tmp += key;
	}
	netno_ = atoi(tmp.c_str());
	if (netno_ == 0)
	{
		lpNetWork.SetNetWorkMode(NetWorkMode::HOST);
	}
	else if (netno_ == 1)
	{
		lpNetWork.SetNetWorkMode(NetWorkMode::GUEST);
		haveip_ = GuestMode::NOIP;
	}
	else if (netno_ == 2 && savehostip)
	{
		lpNetWork.SetNetWorkMode(NetWorkMode::GUEST);
		haveip_ = GuestMode::IP;
		updateMode_ = UpdateMode::SetSaveIp;
	}
	else if (netno_ == 3)
	{
		lpNetWork.SetNetWorkMode(NetWorkMode::OFFLINE);
	}
	else if (netno_ != -1)
	{
		TRACE("選択できないモードです\n");
		netno_ = -1;
		inputKey.clear();
		updateMode_ = UpdateMode::SetNetWorkMode;
		return true;
	}
	updateMode_ = UpdateMode::SetUpdate;
	return true;
}

bool LoginScene::SetUpdate(void)
{
	lpNetWork.Update();
	switch (lpNetWork.GetNetWorkMode())
	{
	case NetWorkMode::HOST:
		TRACE("ホストになりました\n");
		updateMode_ = UpdateMode::Matching;
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
			updateMode_ = UpdateMode::SetSaveIp;
			break;
		}
		updateMode_ = UpdateMode::inHostIp;
		break;
	case NetWorkMode::OFFLINE:
		TRACE("オフラインモードです\n");
		updateMode_ = UpdateMode::StartInit;
		break;
	default:
		TRACE("\n\n存在しないモードです\n");
		updateMode_ = UpdateMode::SetNetWorkMode;
		break;
	}
	inputKey.clear();
	return true;
}

bool LoginScene::SetSaveIp(void)
{
	Vector2 tmpos = fpos_;
	int fsize = GetFontSize();
	DrawString(tmpos.x,tmpos.y+=fsize,"左矢印キーで一列左にずれる,右矢印キーで一列右にずれる",0xffffff);
	DrawString(tmpos.x, tmpos.y += fsize, "モード選択へ戻る  :  ESCキー", 0xffffff);
	//tmpos.y += fsize * 3;
	Vector2 savepos_ = { tmpos.x,tmpos.y+fsize*3};
	tmpos = savepos_;
	std::pair<bool, int> maxposy;
	maxposy.second = tmpos.y;
	std::ifstream ifs("ini/Ip.txt");
	std::vector<std::string> line;	// Ipの一覧
	std::string str;				// Ip単体の保存先
	std::string id;					// idを付け加える用
	int id_ = page_;						// id
	int i = 0;
	while (std::getline(ifs, str)) {
		line.emplace_back(str);
		id.clear();

		if (i++ < id_)
		{
			continue;
		}
		id += std::to_string(id_++);
		id += " :  ";
		id += str;
		DrawString(tmpos.x, tmpos.y+=fsize,id.c_str(),0xffffff);
		if (id_ != 0 && id_ % pagecount_ == 0)
		{
			tmpos.x += fsize*14;
			maxposy.first = true;
			maxposy.second = tmpos.y;
			tmpos.y = savepos_.y;
		}
	}
	if (maxposy.first) {
		tmpos.y = maxposy.second;
	}
	if (ipfirst_.first){
		tmpos.y += fsize * 2;
		DrawFormatString(pos_.x,tmpos.y+=fsize,0xffffff,"%d は 存在しない番号です",ipfirst_.second);
	}
	bool next = InsertView(tmpos);
	
	if (next)
	{
		int no_ = 0;
		std::string tmp;
		for (auto& key : inputKey) {
			tmp += key;
		}
		no_ = atoi(tmp.c_str());
		std::string save;
		if (line.size() > no_)
		{
			int pos = 0;
			auto one = [&]() {
				int temporary = std::atoi(line[no_].substr(pos, line[no_].find(".")).c_str());
				line[no_].erase(0, line[no_].find(".") + 1);
				return temporary;
			};
			hostip_.d1 = one();
			hostip_.d2 = one();
			hostip_.d3 = one();
			hostip_.d4 = one();

			updateMode_ = UpdateMode::inHostIp;
			ipfirst_.first = false;
		}
		else
		{
			ipfirst_.first = true;
			ipfirst_.second = no_;
			inputKey.clear();
		}
	}

	if (Trg(VK_LEFT) && 0 < page_) { page_ -= pagecount_; }
	if (Trg(VK_RIGHT)&& page_ < line.size()-pagecount_) { page_ += pagecount_; }
	return true;
}

bool LoginScene::IpNotBeFound(void)
{
	Vector2 tmpos = fpos_;
	int fsize = GetFontSize();
	tmpos.y += fsize * 3;

	DrawFormatString(tmpos.x,tmpos.y+=fsize,0xffffff,"IP : %d.%d.%d.%d が 見つかりませんでした", hostip_.d1, hostip_.d2, hostip_.d3, hostip_.d4);
	tmpos.y += fsize * 2;
	DrawString(tmpos.x, tmpos.y += fsize, "もう一度同じIPで探す :  ENTERキー", 0xffffff);
	DrawString(tmpos.x, tmpos.y+=fsize, "モード選択へ戻る  :  ESCキー", 0xffffff);

	if (Trg(VK_RETURN))
	{
		haveip_ = GuestMode::IP;
		updateMode_ = UpdateMode::inHostIp;
	}
	return true;
}

bool LoginScene::GamePlay(void)
{
	if (lpNetWork.GetNetWorkMode() == NetWorkMode::OFFLINE) {
		lpNetWork.SetTimeStart(lpSceneMng.GetNowTime());
		return false;
	}
	chronoi time{ std::chrono::system_clock::now() };
	MesPacket data;

	data.resize(2);

	time.now = lpSceneMng.GetNowTime();
	data[0].iData = time.inow[0];
	data[1].iData = time.inow[1];

	lpNetWork.SendMesAll(MesType::COUNT_DOWN_GAME, data);
	return false;
}

void LoginScene::SendData()
{
	unionData j[2] = {0,0};
	unsigned char* c = reinterpret_cast<unsigned char*>(&j);
	unsigned short id = 0;
	int i = 0;
	int co = 0;
	unsigned char tmp = 0;
	std::vector<unsigned char> _data;
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

	int count = 0;
	MesPacket senddata;

	for (auto& chip : _data)
	{
		tmp |= chip << co % 2 * 4;
		if (++co % 2 == 0 && co != 0) {
			c[i++] = tmp;
			tmp = 0;
		}
		if (co == 8) {
			senddata.emplace_back(j[0]);
			i = 0;
			co = 0;
		}
	}
	if (co != 0)
	{
		while (i < 4) {
			tmp |= 0 << (co++ % 2) * 4;
			c[i++] = tmp;
		}
		senddata.emplace_back(j[0]);
	}
	if (senddata.size() > 0)
	{
		lpNetWork.SendMesAll(MesType::TMX_DATA, senddata);
	}else{
		lpNetWork.SendMesData(MesType::TMX_DATA);
	}
}

void LoginScene::NumInput(void)
{
	auto trg = [&](int inp, std::string str) {if(Trg(inp))inputKey.emplace_back(str); };
	int num[2] = { VK_NUMPAD0,0x30};
	for (int i = 0; i < 10; i++)
	{
		trg(num[0]++,std::to_string(i));
		trg(num[1]++, std::to_string(i));
	}

	trg(VK_OEM_PERIOD,".");
	trg(VK_DECIMAL, ".");
	trg(VK_RETURN,"Enter");

	if (Trg(VK_BACK)) { if(inputKey.size() > 0)inputKey.pop_back(); }
}

bool LoginScene::Trg(int id)
{
	return !oldbuf_[id] && (nowbuf_[id] & 0xf0);
}


void LoginScene::ViewIP(Vector2& tmpos, std::array<IPDATA, 5>& tmpip,int fsize)
{
	for (auto& ip : tmpip)
	{
		if (ip.d1 == 0)
		{
			break;
		}
		if (ip.d1 != 192)
		{
			DrawFormatString(tmpos.x, tmpos.y, 0xffffff, "ローカル  :IPアドレス : %d.%d.%d.%d\n", ip.d1, ip.d2, ip.d3, ip.d4);
		}
		else
		{
			DrawFormatString(tmpos.x, tmpos.y, 0xffffff, "グローバル :IPアドレス : %d.%d.%d.%d\n", ip.d1, ip.d2, ip.d3, ip.d4);
		}
		tmpos.y += fsize;
	}
}

bool LoginScene::InsertView(Vector2& tmpos)
{
	bool next = false;
	Vector2 pos = pos_;
	for (auto& key : inputKey)
	{
		if (key == "Enter") {
			inputKey.pop_back();
			next = true;
		}
		DrawString(pos.x, tmpos.y + 100, key.c_str(), 0xffffff, true);
		pos.x += GetFontSize() / 2;
	}
	return next;
}
