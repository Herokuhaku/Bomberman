#include <DxLib.h>
#include "Player.h"
#include "../_debug/_DebugConOut.h"
#include "../NetWork/NetWork.h"
#include "../Scene/GameScene.h"
#include "../AllControl/Control.h"
#include "../AllControl/KeyBoardCtl.h"
#include "../AllControl/XboxController.h"
#include "../TiledLoader.h"

int Player::countid_ = 0;
int Player::fallCount = 0;

Player::Player(Vector2 pos, Vector2 size, std::shared_ptr<Wall>& wall,BaseScene& scene) :scene_(scene)
{
	pos_ = pos;
	size_ = size;
	wall_ = wall;
	Init();
}

Player::~Player()
{
	countid_ -= 5;
}

void Player::Draw(void)
{
	if (animalive_)
	{
		SetDrawScreen(screen);
		ClsDrawScreen();
		int coma = frame_++ / oneanimCnt * 5 + animationdir_[pldir_];
		DrawGraph(0, 0, animation_[coma], true);
		if (!alive_ && frame_ > oneanimCnt * 4 - 1 && pldir_ == DIR::DEATH) {
			animalive_ = false;
		}
		else if (frame_ > oneanimCnt * 4 - 1) {
			frame_ = oneanimCnt * 2;
		}
		SetDrawScreen(DX_SCREEN_BACK);
		DrawRotaGraph(pos_.x + size_.x / 2, pos_.y + size_.y / 6, 1.0f, 0.0f, screen, true);
		DrawBox(centerpos_.x - size_.x / 2, centerpos_.y - size_.x / 2, centerpos_.x + size_.x / 2, centerpos_.y + size_.x / 2, 0xff00ff, false);
	}
}

void Player::Update(void)
{
	for (auto& list : lpNetWork.GetDeathNote())
	{
		if (list == id_)
		{
			pldir_ = DIR::DEATH;
			alive_ = false;
			return;
		}
	}
	if (alive_)
	{
		update_();
	}
}

int Player::GetNo()
{
	return countid_;
}

void Player::UpdateDef()
{
	(*controller_)();

	CheckDeath();
	CheckItem();

	centerpos_ = { pos_.x + size_.x / 2,pos_.y + size_.y - size_.x};
	bombpos_ = centerpos_;
	bool flag = false;
	for (auto& data : controller_->GetCntData())
	{
		// 既存のlistチェック
		for (auto& check : keylist_)
		{
			// 同じDIRのものがあれば flag をtrueにしてkeylistに追加できないようにする
			// 現在のキー情報を更新 kiylistの中身データ check.firstにdataをいれる;
			if (data.first == check.first.first)
			{
				check.first = data;
				flag = true;
			}
		}
		// キーを押しとき追加
		if (data.second[0] && !flag)
		{
			// listに追加
			keylist_.emplace_front(std::pair<INPUT_ID, TrgBool>(data.first, data.second),false);
		}
		flag = false;
	}
	// listの一番上だけ回す　他はキーボードの入力をみて0だったらkeylist_.secondのbool をtrueにする
	for (auto& key : keylist_)
	{
		if (keymove_[key.first.first](key,flag))
		{
			flag = true;
		}
	}
	// keylist_.second のboolがtrueだったら消す
	auto itr = std::remove_if(keylist_.begin(), keylist_.end(), [&](DellistData& list) {return list.second;});
	keylist_.erase(itr, keylist_.end());

	Header tmp = { MesType::POS,0,0,1 };
	MesPacket tmpmes;
	tmpmes.resize(4);
	tmpmes[0].iData = id_;
	tmpmes[1].iData = pos_.x;
	tmpmes[2].iData = pos_.y;
	tmpmes[3].iData = static_cast<int>(pldir_);
	lpNetWork.SendMesAll(MesType::POS,tmpmes);
}

void Player::UpdateAuto()
{
	dirupdate_[pldir_](pos_,width);
	Header tmp = { MesType::POS,0,0,1 };
	MesPacket tmpmes;
	tmpmes.resize(4);
	tmpmes[0].iData = id_;
	tmpmes[1].iData = pos_.x;
	tmpmes[2].iData = pos_.y;
	tmpmes[3].iData = static_cast<int>(pldir_);

	lpNetWork.SendMesAll(MesType::POS, tmpmes);
}

void Player::UpdateNet()
{
	while(meslist_.size() != 0)
	{
		auto tmp = meslist_.front();
		if (tmp.first == MesType::POS)
		{
			if (tmp.second.size() != 0)
			{
				pos_.x = tmp.second[1].iData;
				pos_.y = tmp.second[2].iData;
				pldir_ = static_cast<DIR>(tmp.second[3].iData);
			}
			meslist_.erase(meslist_.begin());
		}
		else if (tmp.first == MesType::SET_BOMB)
		{
			if (tmp.second.size() != 0)
			{
				Vector2 tmpos = { tmp.second[2].iData ,tmp.second[3].iData };
				chronoi tmptime{std::chrono::system_clock::time_point()};
				tmptime.inow[0] = tmp.second[5].iData;
				tmptime.inow[1] = tmp.second[6].iData;
				dynamic_cast<GameScene&>(scene_).SetBomb(tmp.second[0].iData,tmp.second[1].iData, { tmp.second[2].iData ,tmp.second[3].iData },tmptime.now,3000,tmp.second[4].iData,false);
				wall_->ChangeMapData("Obj", tmpos, -1);
			}
			meslist_.erase(meslist_.begin());
		}
	}
}

void Player::Init(void)
{
	objtype_ = ObjType::Player;
	id_ = countid_;
	animation_.resize(20);
	LoadDivGraph("Tiled/image/bomberman.png", 20, 5, 4, size_.x, size_.y, animation_.data());
	speed_ = 2;
	pldir_ = DIR::RIGHT;
	animationdir_[DIR::DOWN] = 0;
	animationdir_[DIR::LEFT] = 1;
	animationdir_[DIR::RIGHT] = 2;
	animationdir_[DIR::UP] = 3;
	animationdir_[DIR::DEATH] = 4;
	frame_ = oneanimCnt*2;

	if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST)
	{
		if (id_ == 0)	{
			update_ = std::bind(&Player::UpdateDef, this);
			type = MOVE_TYPE::Def;
			controller_ = std::make_unique<KeyBoard>();
		}
		else if(id_ % 5 == 0 && id_ % 10 != 0)
		{
			update_ = std::bind(&Player::UpdateNet, this);
			type = MOVE_TYPE::Net;
		}
		else
		{
			update_ = std::bind(&Player::UpdateAuto, this);
			type = MOVE_TYPE::Auto;
		}
	}
	else if(lpNetWork.GetNetWorkMode() == NetWorkMode::GUEST)
	{
		if (id_ == lpNetWork.PlayerID().first)
		{
			update_ = std::bind(&Player::UpdateDef, this);
			type = MOVE_TYPE::Def;
			controller_ = std::make_unique<KeyBoard>();
		}
		else
		{
			update_ = std::bind(&Player::UpdateNet, this);
			type = MOVE_TYPE::Net;
		}
	}
	else
	{
		if (id_ == 0)
		{
			update_ = std::bind(&Player::UpdateDef, this);
			type = MOVE_TYPE::Def;
			controller_ = std::make_unique<KeyBoard>();
		}
		else
		{
			update_ = std::bind(&Player::UpdateAuto, this);
		}
	}

	dirupdate_[DIR::RIGHT] = [&](Vector2 pos,int width) {DirRight(pos,width);};
	dirupdate_[DIR::LEFT] = [&](Vector2 pos, int width) {DirLeft(pos, width);};
	dirupdate_[DIR::UP] = [&](Vector2 pos, int width) {DirUp(pos, width);};
	dirupdate_[DIR::DOWN] = [&](Vector2 pos, int width) {DirDown(pos, width);};
	dirupdate_[DIR::DEATH] = [&](Vector2 pos, int width) {DirDeath(pos, width);};
	Mapdata = wall_->GetMapData();

	lpNetWork.AddMesList(id_,meslist_,mtx_);
	bombpos_ = pos_;
	countid_ +=5;
	playerid_= id_;
	screen = MakeScreen(size_.x,size_.y,true);
	KeyInit();
	if (controller_ != nullptr)
	{
		controller_->SetUp(0);
	}
	numstr = lpTiledLoader.GetTmx().num;
	width = 32;
	layerchip_ = std::atoi(numstr["width"].c_str()) * std::atoi(numstr["height"].c_str());
	numint["width"] = std::atoi(numstr["width"].c_str());

	plm.try_emplace(Have::Bomb,4,4,10);
	plm.try_emplace(Have::Length,2,6,9);
	plm.try_emplace(Have::Speed,2,8,12);

	for (int i = 0;i < std::get<1>(plm[Have::Bomb]);i++)
	{
		bomb_[i] = { i+1,false };
	}
}

void Player::SetBombBool(int no, bool flag)
{
	bomb_[no].second = flag;
}

void Player::KeyInit()
{
	centerpos_ = { pos_.x + size_.x/2,pos_.y+size_.y-size_.x/2};
	bombpos_ = centerpos_;
	keymove_.try_emplace(INPUT_ID::RIGHT, [&](DellistData& data, bool flag) {
		if (data.first.second[static_cast<int>(Trg::Now)] && !flag)
		{
			centerpos_.x += (size_.x/2+ std::get<0>(plm[Have::Speed]));
			pldir_ = DIR::RIGHT;
			// 普通の時の移動確認用添え字
			int check = (centerpos_.x / width) + ((centerpos_.y / width) * numint["width"]);
			// 爆弾の上にいる時用添え字
			int bombcheck = (bombpos_.x / width) + ((bombpos_.y / width) * numint["width"]);
			int next = ((centerpos_.x + (size_.x / 2 + std::get<0>(plm[Have::Speed]))) / width) + ((centerpos_.y / width) * numint["width"]);
			if (wall_->GetMapData()["Obj"][check] == 0)
			{
				pos_.x += std::get<0>(plm[Have::Speed]);
				pos_.y = centerpos_.y/32*32;	// 32の倍数になるように切り捨て
				return true;
			}
			else if (wall_->GetMapData()["Obj"][bombcheck] == 255 &&
				wall_->GetMapData()["Obj"][next] == 0)
			{
				pos_.x += std::get<0>(plm[Have::Speed]);
				pos_.y = centerpos_.y / 32 * 32;	// 32の倍数になるように切り捨て
				return true;
			}
			centerpos_.x -= (size_.x / 2 + std::get<0>(plm[Have::Speed]));
		}
		else if (!data.first.second[static_cast<int>(Trg::Now)] || !data.first.second[static_cast<int>(Trg::Old)])
		{
			data.second = true;
		}
		return false;
		});
	keymove_.try_emplace(INPUT_ID::LEFT, [&](DellistData& data, bool flag) {
		if (data.first.second[static_cast<int>(Trg::Now)]&& !flag)
		{
			centerpos_.x -= (size_.x / 2+ std::get<0>(plm[Have::Speed]));
			pldir_ = DIR::LEFT;
			int check = (centerpos_.x / width) + ((centerpos_.y / width) * numint["width"]);
			int bombcheck = (bombpos_.x / width) + ((bombpos_.y / width) * numint["width"]);
			int next = ((centerpos_.x - (size_.x / 2 + std::get<0>(plm[Have::Speed]))) / width) + ((centerpos_.y / width) * numint["width"]);
			if (wall_->GetMapData()["Obj"][check] == 0)
			{
				pos_.x -= std::get<0>(plm[Have::Speed]);
				pos_.y = centerpos_.y / 32 * 32;
				return true;
			}
			else if (wall_->GetMapData()["Obj"][bombcheck] == 255 &&
				wall_->GetMapData()["Obj"][next] == 0)
			{
				pos_.x -= std::get<0>(plm[Have::Speed]);
				pos_.y = centerpos_.y / 32 * 32;
				return true;
			}
			centerpos_.x += (size_.x / 2 + std::get<0>(plm[Have::Speed]));
		}
		else if (!data.first.second[static_cast<int>(Trg::Now)])
		{
			data.second = true;
		}
		return false;
		});	
	keymove_.try_emplace(INPUT_ID::UP, [&](DellistData& data, bool flag) {
		if (data.first.second[static_cast<int>(Trg::Now)] && data.first.second[static_cast<int>(Trg::Old)] && !flag)
		{
			centerpos_.y -= (size_.x/2+ std::get<0>(plm[Have::Speed]));
			pldir_ = DIR::UP;
			int check = (centerpos_.x / width) + ((centerpos_.y / width) * numint["width"]);
			int bombcheck = (bombpos_.x / width) + ((bombpos_.y / width) * numint["width"]);
			int next = (centerpos_.x / width) + (((centerpos_.y - (size_.x / 2 + std::get<0>(plm[Have::Speed]))) / width) * numint["width"]);
			if (wall_->GetMapData()["Obj"][check] == 0)
			{
				pos_.y -= std::get<0>(plm[Have::Speed]);
				pos_.x = centerpos_.x / 32 * 32;
				return true;
			}
			else if (wall_->GetMapData()["Obj"][bombcheck] == 255 &&
				wall_->GetMapData()["Obj"][next] == 0)
			{
				pos_.y -= std::get<0>(plm[Have::Speed]);
				pos_.x = centerpos_.x / 32 * 32;
				return true;
			}
			centerpos_.y += (size_.x / 2 + std::get<0>(plm[Have::Speed]));
		}
		else if (!data.first.second[static_cast<int>(Trg::Now)])
		{
			data.second = true;
		}
		return false;
		});	
	keymove_.try_emplace(INPUT_ID::DOWN, [&](DellistData& data, bool flag) {
		if (data.first.second[static_cast<int>(Trg::Now)]&&!flag)
		{
			centerpos_.y += (size_.x / 2 + std::get<0>(plm[Have::Speed]));
			pldir_ = DIR::DOWN;
			int check = (centerpos_.x / width) + ((centerpos_.y / width) * numint["width"]);
			int bombcheck = (bombpos_.x / width) + ((bombpos_.y / width) * numint["width"]);
			int next = (centerpos_.x / width) + (((centerpos_.y + (size_.x / 2 + std::get<0>(plm[Have::Speed]))) / width) * numint["width"]);
			if (wall_->GetMapData()["Obj"][check] == 0)
			{
				pos_.y += std::get<0>(plm[Have::Speed]);
				pos_.x = centerpos_.x / 32 * 32;
				return true;
			}
			else if (wall_->GetMapData()["Obj"][bombcheck] == 255 &&
				wall_->GetMapData()["Obj"][next] == 0)
			{
				pos_.y += std::get<0>(plm[Have::Speed]);
				pos_.x = centerpos_.x / 32 * 32;
				return true;
			}
			centerpos_.y -= (size_.x / 2 + std::get<0>(plm[Have::Speed]));
		}
		else if (!data.first.second[static_cast<int>(Trg::Now)])
		{
			data.second = true;
		}
		return false;
		});	
	keymove_.try_emplace(INPUT_ID::BOMB, [&](DellistData& data, bool flag) {
		if (data.first.second[static_cast<int>(Trg::Now)] && !data.first.second[static_cast<int>(Trg::Old)])
		{
			Vector2 tmpos = Vector2(pos_.x + size_.x / 2, pos_.y + size_.x / 2) / 32 * 32 + size_.x / 2;
			//Vector2 tmpos = pos_;
			int check = (tmpos.x / width) + ((tmpos.y / width) * numint["width"]);
			if (wall_->GetMapData()["Obj"][check] == 0 && bomblist < std::get<0>(plm[Have::Bomb]))
			{
				for (auto& bom : bomb_)
				{
					if (!bom.second)
					{
						dynamic_cast<GameScene&>(scene_).SetBomb(id_,id_+bom.first, tmpos, lpSceneMng.GetNowTime(), 3000, std::get<0>(plm[Have::Length]), true);
						bom.second = true;
						break;
					}
				}
				bomblist++;
				wall_->ChangeMapData("Obj", tmpos, -1);
				return true;
			}
		}
		return false;
		});


	keylist_.clear();
}

void Player::CheckDeath(void)
{
	if (wall_->GetFireData()[(centerpos_.x / width) + ((centerpos_.y / width) * numint["width"])].first != 255)
	{
		pldir_ = DIR::DEATH;
		unionData uni;
		uni.iData = id_;
		if (alive_) {
			lpNetWork.SendMesAll(MesType::DEATH, { uni });
		}

		alive_ = false;
	}
}

void Player::CheckItem(void)
{
	bomblist = dynamic_cast<GameScene&>(scene_).BombCount(id_);
	int checknum = wall_->GetMapData()["Item"][(centerpos_.x / width) + ((centerpos_.y / width) * numint["width"])];
	// 爆弾数
	if (checknum == std::get<2>(plm[Have::Bomb]))
	{
		if (std::get<0>(plm[Have::Bomb]) < std::get<1>(plm[Have::Bomb]))
		{
			plm[Have::Bomb] = { std::get<0>(plm[Have::Bomb]) + 1, std::get<1>(plm[Have::Bomb]), std::get<2>(plm[Have::Bomb]) };
		}
		wall_->ChangeMapData("Item", centerpos_, -1);
	}
	// 爆弾の威力
	if (checknum == std::get<2>(plm[Have::Length]))
	{
		if (std::get<0>(plm[Have::Length]) < std::get<1>(plm[Have::Length]))
		{
			std::get<0>(plm[Have::Length]) = std::get<0>(plm[Have::Length]) + 1;
		}
		wall_->ChangeMapData("Item", centerpos_, -1);
	}
	// 足の速さ
	if (checknum == std::get<2>(plm[Have::Speed]))
	{
		if (std::get<0>(plm[Have::Speed]) < std::get<1>(plm[Have::Speed]))
		{
			std::get<0>(plm[Have::Speed]) = std::get<0>(plm[Have::Speed])*2;
		}
		wall_->ChangeMapData("Item", centerpos_, -1);
	}
}

void Player::DirRight(Vector2 pos, int width)
{
	pos.x += size_.x;
	int check = (pos.x / width) + ((pos.y / width) * numint["width"]);
	if ((check >= 0 && check < layerchip_)&& wall_->GetMapData()["Obj"][check] == 0)
	{
		pos_.x += speed_;
	}
	else
	{
		pldir_ = DIR::DOWN;
		frame_ = oneanimCnt * 2;
	}
}
void Player::DirLeft(Vector2 pos, int width)
{
	pos.x -= speed_;
	int check = (pos.x / width) + ((pos.y / width) * numint["width"]);
	if ((check >= 0 && check < layerchip_) && wall_->GetMapData()["Obj"][check] == 0)
	{
		pos_.x -= speed_;
	}
	else
	{
		pldir_ = DIR::UP;
		frame_ = oneanimCnt * 2;
	}
}

void Player::DirUp(Vector2 pos, int width)
{
	pos.y -= speed_;
	int check = (pos.x / width) + ((pos.y / width) * numint["width"]);
	if ((check >= 0 && check < layerchip_) && wall_->GetMapData()["Obj"][check] == 0)
	{
		pos_.y -= speed_;
	}
	else
	{
		pldir_ = DIR::RIGHT;
		frame_ = oneanimCnt * 2;
	}
}

void Player::DirDown(Vector2 pos, int width)
{
	pos.y += size_.x;
	int check = (pos.x / width) + ((pos.y / width) * numint["width"]);
	if ((check >= 0 && check < layerchip_) && wall_->GetMapData()["Obj"][check] == 0)
	{
		pos_.y += speed_;
	}
	else
	{
		pldir_ = DIR::LEFT;
		frame_ = oneanimCnt * 2;
	}
}
