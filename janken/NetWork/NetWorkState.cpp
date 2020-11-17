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

std::vector<unionData> NetWorkState::GetRevdata(void)
{
	std::vector<unionData> tmp = revdata_;
	revdata_.clear();
	revdata_.shrink_to_fit();
	return tmp;
}

std::vector<unionData> NetWorkState::GetPosdata(int no)
{
	std::vector<unionData> tmp = posdata_[no];
	int id = -1;
	if (tmp.size() >= 1)id = tmp[0].iData;
	if (id == no)
	{
		if (tmp.size() != 0)
		{
			std::vector<unionData> tmpd = posdata_[id];
			posdata_[id].clear();
			tmp.erase(tmp.begin());
			//posdata_.try_emplace(id, tmp);
			return tmpd;
		}
	}
	std::vector<unionData> null;
	return null;
}

int NetWorkState::RevPosSize(void)
{
	return static_cast<int>(posdata_.size());
}
