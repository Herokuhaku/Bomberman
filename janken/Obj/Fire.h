#pragma once
#include <memory>
#include "Obj.h"
#include "../Wall.h"

#define ƒÎ 3.14159265
#define RAD(X) X*(ƒÎ/180.00)

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
    std::map<DIR,double> dirRad_;
};

