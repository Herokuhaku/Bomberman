#pragma once
#include "BaseScene.h"

enum class TransMode
{
	CrossOver,
	RotationScene,
	FadeIn,
	FadeOut
};

class TransitionScene :
	public BaseScene
{
public:
	TransitionScene();
	~TransitionScene();
	virtual void Init(void) = 0;
	virtual std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own) = 0;
	virtual void Draw();
	virtual void DrawOwnScreen();
protected:
	int count_;
};

