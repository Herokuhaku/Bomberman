#pragma once
//#include <array>
#include "Control.h"

class KeyBoard :
	public Control
{
public:
	ControlType GetType(void)override;
	bool SetUp(int no)override;
	void Update(void)override;
private:
	std::array<char, 256> _keyData = {};
};