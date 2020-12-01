#pragma once
#include "TransitionScene.h"
class RotationScene :
	public TransitionScene
{
public:
	RotationScene(std::unique_ptr<BaseScene> old, std::unique_ptr<BaseScene> next);
	~RotationScene();
	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own)override;
	void DrawOwnScreen(void)override;
	bool Transition(void);
private:
	std::unique_ptr<BaseScene> old_;
	std::unique_ptr<BaseScene> next_;
	bool flag;
	double ex;
	double rad;
	int rad_;
};

