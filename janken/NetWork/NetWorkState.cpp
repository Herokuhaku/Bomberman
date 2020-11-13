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
	std::vector<unionData> tmp = revdata_;
	int id = tmp[0].iData;
	if (id == no)
	{
		revdata_.clear();
		revdata_.shrink_to_fit();
		if (tmp.size() != 0)
		{
			tmp.erase(tmp.begin());
			posdata_.try_emplace(id, tmp);
			return posdata_[id];
		}
	}
	std::vector<unionData> null;
	return null;
}
