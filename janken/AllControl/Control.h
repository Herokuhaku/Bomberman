#pragma once
#include <DxLib.h>
#include <map>
#include <memory>
#include <array>
#include <mutex>
#include "INPUT_ID.h"

enum class ControlType
{
	KEY,
	MOUSE,
	XBOX_CONTROLLER,
	MAX
};
enum class Trg
{
	Now,
	Old,
	Max
};

using TrgBool = std::array<bool, static_cast<size_t>(Trg::Max)>;
using CntData = std::map<INPUT_ID, TrgBool>;

struct Control
{
	void operator()() {
		Update();
	}
	const CntData& GetCntData(void)
	{
		return _data;
	}
	virtual ControlType GetType(void) = 0;
	virtual bool SetUp(int no) = 0;
private:
	virtual void Update() = 0;
protected:
	CntData _data = {};
	std::map<INPUT_ID, int> _keyList = {};
};