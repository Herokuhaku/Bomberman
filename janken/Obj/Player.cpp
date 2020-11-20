#include <DxLib.h>
#include "Player.h"
#include "../_debug/_DebugConOut.h"
#include "../NetWork/NetWork.h"
#include "../Scene/GameScene.h"
#include "../AllControl/Control.h"
#include "../AllControl/KeyBoardCtl.h"
#include "../AllControl/XboxController.h"

int Player::countid_ = 0;
int Player::fallCount = 0;

Player::Player()
{
	pos_ = { 0,0 };
	size_ = { 0,0 };
	Init();
}

Player::Player(Vector2 pos, Vector2 size, std::shared_ptr<Wall> wall,BaseScene& scene) :scene_(std::move(&scene))
{
	pos_ = pos;
	size_ = size;
	wall_ = wall;
	Init();
}

Player::~Player()
{
}

void Player::Draw(void)
{
	SetDrawScreen(screen);
	ClsDrawScreen();
	//DrawRotaGraph(pos_.x + size_.x /2, pos_.y + size_.y / 6,1.0f,0.0f,animation_[frame_++ / oneanimCnt * 5 + animationdir_[pldir_]],true);
	DrawGraph(0,0,animation_[frame_++ / oneanimCnt * 5 + animationdir_[pldir_]], true);
	if (frame_ > oneanimCnt * 4-1)frame_ = oneanimCnt*2;
	SetDrawScreen(DX_SCREEN_BACK);
	DrawRotaGraph(pos_.x + size_.x / 2, pos_.y + size_.y / 6,1.0f,0.0f,screen,true);
}

void Player::Update(void)
{
	update_();
}

int Player::GetNo()
{
	return countid_;
}

//int Player::OkNum()
//{
//	return lpNetWork.TakeOutRevData(id_).size();
//}

void Player::UpdateDef()
{
	(*controller_)();
	bool flag = false;
	for (auto& data : controller_->GetCntData())
	{
		for (auto& check : keylist_)
		{
			if (data.first == check.first)
			{
				check = data;
				flag = true;
			}
		}
		if (data.second[0] && data.second[1] && !flag)
		{
			keylist_.emplace_front(std::pair<INPUT_ID, TrgBool>(data.first, data.second));
		}
		flag = false;
	}
	for (auto& key : keylist_)
	{
		if (keymove_[key.first](key.second,flag))
		{
			flag = true;
		}
	}
	Header tmp = { MesType::POS,0,0,1 };
	lpNetWork.SendMesData(MesType::POS, { id_,pos_.x,pos_.y,static_cast<int>(pldir_) });

	//dynamic_cast<GameScene&>(*scene_).SetBomb();
}

void Player::UpdateAuto()
{
	dirupdate_[pldir_](pos_,width);
	Header tmp = { MesType::POS,0,0,1 };
	lpNetWork.SendMesData(MesType::POS, {id_,pos_.x,pos_.y,static_cast<int>(pldir_)});
}

void Player::UpdateNet()
{
	if (meslist_.size() != 0)
	{
		auto tmp = meslist_.front();
		meslist_.erase(meslist_.begin());
		pos_.x = tmp.second[1].iData;
		pos_.y = tmp.second[2].iData;
		pldir_ = static_cast<DIR>(tmp.second[3].iData);
	}
	else
	{
		TRACE("NotData  Å°Å°%dÅ°Å° \n",id_);
		fallCount++;
	}
}

void Player::Init(void)
{
	id_ = countid_;
	animation_.resize(20);
	LoadDivGraph("Tiled/image/bomberman.png", 20, 5, 4, size_.x, size_.y, animation_.data());
	//if (animation_[0] = LoadGraph("Tiled/image/bomberman.png"),animation_[0] == -1)
	//{
	//	return;
	//}
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
		if (id_ == 5)
		{
			update_ = std::bind(&Player::UpdateDef, this);
			type = MOVE_TYPE::Def;
			controller_ = std::make_unique<XboxController>();
		}
		else if(id_ % 5 == 0 && id_ % 10 == 0)
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
	oldpos_ = pos_;
	countid_ +=5;
	playerid_+=countid_;
	screen = MakeScreen(size_.x,size_.y,true);
	KeyInit();
	if(controller_ != nullptr)controller_->SetUp(0);
}

void Player::KeyInit()
{
	keymove_.try_emplace(INPUT_ID::RIGHT, [&](TrgBool data,bool flag) {
		if (data[static_cast<int>(Trg::Now)] && data[static_cast<int>(Trg::Old)] && flag)
		{
			pos_.x += 2;
			return true;
		}
		return false;
		});
	keymove_.try_emplace(INPUT_ID::LEFT, [&](TrgBool data,bool flag) {
		if (data[static_cast<int>(Trg::Now)] && data[static_cast<int>(Trg::Old)] && flag)
		{
			pos_.x -= 2;
			return true;
		}
		else if (!data[static_cast<int>(Trg::Now)] || !data[static_cast<int>(Trg::Old)]) {};
		return false;
		});
	keymove_.try_emplace(INPUT_ID::UP, [&](TrgBool data,bool flag) {
		if (data[static_cast<int>(Trg::Now)] && data[static_cast<int>(Trg::Old)] && flag)
		{
			pos_.y -= 2;
			return true;
		}
		return false; });
	keymove_.try_emplace(INPUT_ID::DOWN, [&](TrgBool data, bool flag) {
		if (data[static_cast<int>(Trg::Now)] && data[static_cast<int>(Trg::Old)] && flag)
		{
			pos_.y += 2;
			return true;
		}
		return false; });
	keymove_.try_emplace(INPUT_ID::BOMB, [&](TrgBool data, bool flag) {
		if (data[static_cast<int>(Trg::Now)] && !data[static_cast<int>(Trg::Old)] && flag)
		{
			dynamic_cast<GameScene&>(*scene_).SetBomb(countid_,playerid_++,pos_,true);
			return true;
		}
		return false;
		});
}

void Player::DirRight(Vector2 pos, int width)
{
	pos.x += size_.x;
	if (wall_->GetMapData()["Obj"][(pos.x / width) + ((pos.y / width) * 21)] == 0)
	{
		pos_.x += 5;
	}
	else
	{
		pldir_ = DIR::DOWN;
		frame_ = oneanimCnt * 2;
	}
}
void Player::DirLeft(Vector2 pos, int width)
{
	pos.x -= 5;
	if (wall_->GetMapData()["Obj"][(pos.x / width) + ((pos.y / width) * 21)] == 0)
	{
		pos_.x -= 5;
	}
	else
	{
		pldir_ = DIR::UP;
		frame_ = oneanimCnt * 2;
	}
}

void Player::DirUp(Vector2 pos, int width)
{
	pos.y -= 5;
	if (wall_->GetMapData()["Obj"][(pos.x / width) + ((pos.y / width) * 21)] == 0)
	{
		pos_.y -= 5;
	}
	else
	{
		pldir_ = DIR::RIGHT;
		frame_ = oneanimCnt * 2;
	}
}

void Player::DirDown(Vector2 pos, int width)
{
	pos.y += 32;
	if (wall_->GetMapData()["Obj"][(pos.x / width) + ((pos.y / width) * 21)] == 0)
	{
		pos_.y += 5;
	}
	else
	{
		pldir_ = DIR::LEFT;
		frame_ = oneanimCnt * 2;
	}
}
