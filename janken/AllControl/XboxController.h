#pragma once
#include "Control.h"
#include "../common/Vector2.h"

#define XINPUT_THUMBL_X 0		// Lスティックの横
#define XINPUT_THUMBL_Y 1		// Lスティックの縦
#define XINPUT_THUMBR_X 2		// Rスティックの横
#define XINPUT_THUMBR_Y 3		// Rスティックの縦

// スティック取得用
#define THUMB_LEFT  0
#define THUMB_RIGHT 1


class XboxController :
	public Control
{
	ControlType GetType(void)override;
	bool SetUp(int no)override;
	void Update(void)override;
private:
	void StickUp(void);

	XINPUT_STATE input = {};

	char pad_ = {};
	int id_ = {};
	static int count_;
};

