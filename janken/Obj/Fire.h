#pragma once
#include <memory>
#include "Obj.h"
#include "../Wall.h"
class Fire :
    public Obj
{
public:
    Fire(Vector2&& size,std::shared_ptr<Wall>& wall);
    ~Fire();
    void Draw(void);
    void Update(void);
    int GetNo();
    void Init(void);
private:
};

