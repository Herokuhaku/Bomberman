#pragma once
#include "BaseScene.h"

enum class TransMode
{
	CrossOver,
	FadeIn,
	FadeOut
};

class Transition :
	public BaseScene
{
public:
	Transition();
	~Transition();
	virtual void DrawOwnScreen();
private:
};

