#pragma once
#include "BaseScene.h"
#include "../common/Vector2.h"

class ResultScene :
    public BaseScene
{
public:
    ResultScene(int screen,int id);
    ~ResultScene();

    void Init(void);
    std::unique_ptr<BaseScene> Update(std::unique_ptr<BaseScene> own);
    void Draw(void);
private:
    int gameScreen_;
    Vector2 pos_;
    int id_;
};

