#include "GuestState.h"
#include "NetWork.h"
#include "../Scene/SceneMng.h"
#include "../TiledLoader.h"

GuestState::GuestState()
{
	reAccess_ = false;
	hostip_ = {};
	indent = 0;
	savenum = -1;
	revflag_ = false;
}

GuestState::~GuestState()
{
}

ActiveState GuestState::ConnectHost(IPDATA hostip)
{
	networkHandle_ = ConnectNetWork(hostip, portNum_);
	hostip_ = hostip;
	if (networkHandle_ >= 0)
	{
		lpNetWork.AddList(std::pair<int, unsigned int>(networkHandle_, 0));
		active_ = ActiveState::Init;
	}
	else
	{
		active_ = ActiveState::Non;
	}

	return active_;
}

bool GuestState::CheckNetWork(void)
{
	if (active_ != ActiveState::Wait && active_ != ActiveState::Non)
	{
		MesHeader tmp;
		//auto data = lpNetWork.GetNetWorkHandle();
		int revcount_ = 0;
		for (auto& hl : lpNetWork.GetListID())
		{
			while (ProcessMessage() == 0)
			{
				if (GetNetWorkDataLength(hl.first) >= sizeof(MesHeader))
				{
					NetWorkRecv(hl.first, &tmp, sizeof(MesHeader));
					int typenum = static_cast<int>(tmp.type);
					if (static_cast<int>(MesType::NON) < typenum && typenum < static_cast<int>(MesType::MAX))
					{
						MesPacket tmpdata;
						tmpdata.resize(tmp.length);
						if (GetNetWorkDataLength(hl.first) >= static_cast<int>(tmp.length) * 4)
						{
							NetWorkRecv(hl.first, tmpdata.data(), tmp.length * 4);
						}
						if (MesTypeList_[tmp.type](tmp, tmpdata, revcount_))
						{
							break;
						}
						else
						{
							continue;
						}
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	if (GetLostNetWork() != -1)
	{
		CloseNetWork(lpNetWork.GetListIdFront().first);
		TRACE("ê⁄ë±Ç™êÿÇÍÇΩÇÊÅI\n\n\n");
		active_ = ActiveState::Lost;
		//lpNetWork.SetNetWorkMode(NetWorkMode::NON);
		lpNetWork.SetRevStandby(false);
		return false;
	}
	return true;
}