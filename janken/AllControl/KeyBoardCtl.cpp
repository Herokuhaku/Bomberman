#include "KeyBoardCtl.h"
#include <DxLib.h>

ControlType KeyBoard::GetType(void)
{
	return ControlType::KEY;
}

bool KeyBoard::SetUp(int no)
{
	_keyList.try_emplace(INPUT_ID::UP,KEY_INPUT_W);
	_keyList.try_emplace(INPUT_ID::DOWN, KEY_INPUT_S);
	_keyList.try_emplace(INPUT_ID::LEFT, KEY_INPUT_A);
	_keyList.try_emplace(INPUT_ID::RIGHT, KEY_INPUT_D);
	_keyList.try_emplace(INPUT_ID::BOMB, KEY_INPUT_Q);

	_keyData.fill(0);
	return true;
}

void KeyBoard::Update(void)
{
	GetHitKeyStateAll(_keyData.data());
	for (auto id : INPUT_ID())
	{
		_data[id][static_cast<int>(Trg::Old)] = _data[id][static_cast<int>(Trg::Now)];
		_data[id][static_cast<int>(Trg::Now)] = _keyData[_keyList[id]];
	}
}
