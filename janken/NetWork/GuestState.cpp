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
						if (tmp.type < MesType::NON || MesType::MAX < tmp.type || tmp.length > MAXSENDBYTE / 4 || (tmp.next != 0 && tmp.next != 1))
						{
							MesPacket Error;
							Error.resize(GetNetWorkDataLength(hl.first));
							NetWorkRecvToPeek(hl.first, Error.data(), GetNetWorkDataLength(hl.first));
							continue;
						}
						if (GetNetWorkDataLength(hl.first) > static_cast<int>(tmp.length))
						{
							tmpdata.resize(tmp.length);
							NetWorkRecv(hl.first, tmpdata.data(), tmp.length * 4);
						}
						bool flag = false;
						if (GetNetWorkDataLength(hl.first) > static_cast<int>(sizeof(tmp)))
						{
							MesPacket t;
							t.resize(2);
							while ((tmp.length >= 1 && !flag) || tmpdata.size() < 2)
							{
								for (int i = 0;i+1 < tmpdata.size(); i++)
								{
									if (tmpdata[i].iData == 108 && tmpdata[i+1].iData == 4)
									{
										if (tmpdata.size() == 2)
										{
											flag = true;
										}
										int mem = 0;
										for (int k = i;k + 2 < tmpdata.size();k++)
										{
											tmpdata[k].iData = tmpdata[k + 2].iData;
											mem = k+1;
										}
										if (mem == 0)
										{
											mem = i;
										}
										NetWorkRecv(hl.first, t.data(),8);
										tmpdata[mem++] = t[0];
										tmpdata[mem++] = t[1];
										flag = false;
									}
									else
									{
										flag = true;
									}
								}
							}
						}
						//if (GetNetWorkDataLength(hl.first) > static_cast<int>(tmp.length))
						//{
						//	NetWorkRecv(hl.first, tmpdata.data(), tmp.length * 4);
						//}
						if (MesTypeList_[tmp.type](tmp, tmpdata, revcount_,hl.first))
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