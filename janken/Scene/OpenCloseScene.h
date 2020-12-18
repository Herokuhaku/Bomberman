#pragma once
#include "TransitionScene.h"
#include "../common/Vector2.h"
class OpenCloseScene :
    public TransitionScene
{
public:
    OpenCloseScene(std::unique_ptr<BaseScene> old, std::unique_ptr<BaseScene> next);
    ~OpenCloseScene();
	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own)override;
	void DrawOwnScreen(void)override;
	bool Transition(void);
private:
	std::unique_ptr<BaseScene> old_;
	std::unique_ptr<BaseScene> next_;

	Vector2 pos_[2][2];
};

