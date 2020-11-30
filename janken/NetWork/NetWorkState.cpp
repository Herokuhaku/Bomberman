#include "NetWorkState.h"


NetWorkState::NetWorkState() :timec{std::chrono::system_clock::now()}
{
	active_ = ActiveState::Non;
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
	revlist.emplace_back(std::pair<MesList&,std::mutex&>(list,mtx));
}