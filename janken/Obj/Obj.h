#pragma once
#include <vector>
#include <memory>
#include "../Wall.h"
#include "../common/Vector2.h"
class Obj
{
public:
	Obj();
	Obj(Vector2 pos,Vector2 size);
	~Obj();
	virtual void Draw(void) = 0;
	virtual void Update(void) = 0;
	virtual int GetNo() = 0;
	virtual void Init(void) = 0;
	virtual bool GetDeleteFlag(void);
protected:
	Vector2 pos_;
	Vector2 size_;
	int id_;
	int screen;
	bool alive_;
	std::vector<int> animation_;
	std::shared_ptr<Wall> wall_;
	bool deleteflag_;

	int width;
	std::map<std::string, int> numint;
	//int stagewidth_;
private:
};

