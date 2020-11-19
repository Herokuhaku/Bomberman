#pragma once
#include <type_traits>

enum class INPUT_ID
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	BOMB,
	MAX
};

static INPUT_ID begin(INPUT_ID) { return INPUT_ID::UP; };
static INPUT_ID end(INPUT_ID) { return INPUT_ID::MAX; };
static INPUT_ID operator++(INPUT_ID& num) { return num = INPUT_ID(std::underlying_type<INPUT_ID>::type(num)+1); };
static INPUT_ID operator*(INPUT_ID key) { return key; };