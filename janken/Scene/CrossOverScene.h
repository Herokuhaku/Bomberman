#pragma once
#include "TransitionScene.h"
class CrossOverScene :
	public TransitionScene
{
public:
	CrossOverScene(std::unique_ptr<BaseScene> old,std::unique_ptr<BaseScene> next);
	~CrossOverScene();	
	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own)override;
	void DrawOwnScreen(void)override;
	bool Transition(void);
private:
	std::unique_ptr<BaseScene> old_;
	std::unique_ptr<BaseScene> next_;
};

