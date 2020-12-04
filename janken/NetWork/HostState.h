#pragma once
#include "NetWorkState.h"
class HostState :
	public NetWorkState
{
public:
	HostState();
	virtual	~HostState();
	NetWorkMode GetMode(void) { return NetWorkMode::HOST; };

	bool CheckNetWork(void);
private:

	bool handleFlag_;

};

