#pragma once
#include "NetWorkState.h"
class GuestState :
	public NetWorkState
{
public:
	GuestState();
	virtual	~GuestState();
	virtual NetWorkMode GetMode(void) { return NetWorkMode::GUEST; };
	ActiveState ConnectHost(IPDATA hostip);
	bool CheckNetWork(void);
private:
	//void OutCsv(void);
	//void OutData(void);

	bool reAccess_;
	IPDATA hostip_;
	short indent;
	int savenum;

	bool revflag_;


};