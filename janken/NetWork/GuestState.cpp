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
							continue;
						}
						if (GetNetWorkDataLength(hl.first) > static_cast<int>(tmp.length))
						{
							tmpdata.resize(tmp.length);
							NetWorkRecv(hl.first, tmpdata.data(), tmp.length * 4);
						}	
						bool count = false;			// 全て0のパターン回避用
						bool bombcount = false;		// tmpdataにbombがいた時用
						int bombNo_ = 0;			// その場所の番号
						for (int i = 0;i < tmpdata.size();i++)
						{
							count |= tmpdata[i].iData;
							if (tmpdata[i].iData == static_cast<int>(MesType::SET_BOMB) && tmpdata[i+1].iData == 7)
							{
								bombcount = true;
								bombNo_ = i;
							}
						}
						if (lpNetWork.GetActive() == ActiveState::Play && GetNetWorkDataLength(hl.first) > 0 )
						{
							MesPacket Rev;
							Header head;	
							MesPacket tmpbomb;
							Rev.resize(GetNetWorkDataLength(hl.first));
							NetWorkRecvToPeek(hl.first, Rev.data(), GetNetWorkDataLength(hl.first));

							if (bombcount)
							{
								head.iheader[0] = tmpdata[bombNo_].iData;
								head.iheader[1] = tmpdata[bombNo_+1].iData;
								tmpdata.erase(tmpdata.begin(),tmpdata.begin()+bombNo_+2);
								int mem = 0;
								for (int i = 0;i < tmpdata.size();i++)
								{
									tmpbomb.emplace_back(tmpdata[i]);
									mem++;
								}
								for (auto& c : Rev)
								{
									if (mem < 7)
									{
										tmpbomb.emplace_back(c);
										mem++;
									}
									else {
										break;
									}
								}
								MesTypeList_[head.header.type](head.header, tmpbomb, revcount_, hl.first);
								Rev.resize(GetNetWorkDataLength(hl.first));
								NetWorkRecv(hl.first, Rev.data(), GetNetWorkDataLength(hl.first));
								break;
							}
							else {
								for (int i = 0;i + 1 < Rev.size();i++)
								{
									if (Rev[i].iData == static_cast<int>(MesType::SET_BOMB) && Rev[i + 1].iData == 7 && i + 8 < Rev.size())
									{
										head.iheader[0] = Rev[i].iData;
										head.iheader[1] = Rev[i + 1].iData;
										for (int k = 1;k < 8;k++)
										{
											tmpbomb.emplace_back(Rev[i + 1 + k]);
										}
										if (tmpbomb.size() == 7 && !count) {
											MesTypeList_[head.header.type](head.header, tmpbomb, revcount_, hl.first);
											break;
										}
									}
								}
							}
						}

						if (MesTypeList_[tmp.type](tmp, tmpdata, revcount_,hl.first))
						{
							break;
						}
						else
						{
							continue;
						}
					}
					else if(GetNetWorkDataLength(hl.first))
					{
						BombCheck(hl.first);
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
		TRACE("接続が切れたよ！\n\n\n");
		active_ = ActiveState::Lost;
		//lpNetWork.SetNetWorkMode(NetWorkMode::NON);
		lpNetWork.SetRevStandby(false);
		return false;
	}
	return true;
}