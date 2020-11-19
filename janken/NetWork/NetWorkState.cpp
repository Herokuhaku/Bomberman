#include "NetWorkState.h"


NetWorkState::NetWorkState()
{
	active_ = ActiveState::Non;
	sizedata_ = {0,0,0};
}

NetWorkState::~NetWorkState()
{
}

ActiveState NetWorkState::GetActive(void)
{
	return active_;
}

bool NetWorkState::SetActive(ActiveState act)
{
	active_ = act;
	return true;
}

ActiveState NetWorkState::ConnectHost(IPDATA hostip)
{
	return active_;
}

int NetWorkState::GetNetWorkHandle(void)
{
	return networkHandle_;
}

void NetWorkState::SetNetWorkHandle(int nethandle)
{
	networkHandle_ = nethandle;
}

bool NetWorkState::CheckNetWork(void)
{
	return true;
}
void NetWorkState::SetPlayerList(int id, MesList& list, std::mutex& mtx)
{

	//revlist_.try_emplace(id,std::pair<MesList&,std::mutex&>(list, mtx) );
}