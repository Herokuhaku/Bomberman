#pragma once
#include "TransitionScene.h"
#include "../common/Vector2.h"
class OpenCloseScene_ver2 :
    public TransitionScene
{
public:
    OpenCloseScene_ver2(std::unique_ptr<BaseScene> old, std::unique_ptr<BaseScene> next);
    ~OpenCloseScene_ver2();
	void Init(void)override;
	std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own)override;
	void DrawOwnScreen(void)override;
	bool Transition(void);
private:
	std::unique_ptr<BaseScene> old_;
	std::unique_ptr<BaseScene> next_;

	Vector2d pos_[2][2];
	int tmpscreen;
};

