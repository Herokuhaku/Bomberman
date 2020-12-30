#include <DxLib.h>
#include "HostState.h"
#include "NetWork.h"
#include "../Scene/SceneMng.h"
HostState::HostState()
{
	if (PreparationListenNetWork(portNum_) == 0)
	{
		active_ = ActiveState::Wait;
	}
	handleFlag_ = false;
	connect_ = false;
}

HostState::~HostState()
{
	StopListenNetWork();
}

bool HostState::CheckNetWork(void)
{
	end = std::chrono::system_clock::now();
	int handle = GetNewAcceptNetWork();
	if(active_ != ActiveState::Wait && active_ != ActiveState::Non && active_ != ActiveState::Lost)
	{
		MesHeader tmp;
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
							if (tmpdata[i].iData == 109 && tmpdata[i + 1].iData == 7)
							{
								bombcount = true;
								bombNo_ = i;
							}
						}
						if (lpNetWork.GetActive() == ActiveState::Play && GetNetWorkDataLength(hl.first) > 0)
						{
							MesPacket Rev;
							Header head;
							MesPacket tmpbomb;
							Rev.resize(GetNetWorkDataLength(hl.first));
							NetWorkRecvToPeek(hl.first, Rev.data(), GetNetWorkDataLength(hl.first));

							if (bombcount)
							{
								head.iheader[0] = tmpdata[bombNo_].iData;
								head.iheader[1] = tmpdata[bombNo_ + 1].iData;
								tmpdata.erase(tmpdata.begin(), tmpdata.begin() + bombNo_ + 2);
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
									if (Rev[i].iData == 109 && Rev[i + 1].iData == 7 && i + 8 < Rev.size())
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

						if (MesTypeList_[tmp.type](tmp, tmpdata, revcount_, hl.first))
						{
							break;
						}
						else
						{
							continue;
						}
					}
					else if (GetNetWorkDataLength(hl.first))
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
	else if (active_ == ActiveState::Wait)
	{
		if (handle != -1)
		{
			//lpNetWork.AddList({ handle,0 });
			//TRACE("接続されたよ\n");
			begin = std::chrono::system_clock::now();

			//chronoi time{ std::chrono::system_clock::now() };
			//time.now = begin;
			//lpNetWork.SendMesData(MesType::COUNT_DOWN_ROOM, { time.uninow[0], time.uninow[1] }, handle);
			active_ = ActiveState::Matching;
			//StopListenNetWork();
			//active_ = ActiveState::Init;
		}
	}
	if (active_ == ActiveState::Matching)
	{
		if (handle != -1)
		{
			lpNetWork.AddList({ handle,0 });
			chronoi time{ std::chrono::system_clock::now() };

			time.now = begin;
			lpNetWork.SendMesData(MesType::COUNT_DOWN_ROOM,{time.uninow[0], time.uninow[1]},handle);
			TRACE("接続されたよ\n");
			connect_ = true;
		}
		__int64 seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		TRACE("%d \n",static_cast<int>(COUNT_LIMIT - seconds));
		if (seconds >= COUNT_LIMIT)
		{
			active_ = ActiveState::Init;
			StopListenNetWork();
		}
	}
	int lost = GetLostNetWork();
	if (lost != -1)
	{
		TRACE("接続が切れたよ\n");
		MesPacket data;
		data.resize(2);
		std::pair<int,unsigned int> pl = lpNetWork.GetLostPlayer(lost);
		data[0].iData = pl.first;
		data[1].iData = pl.second;
		lpNetWork.RemoveList(lost);
		lpNetWork.SendMesAll(MesType::LOST, data);
		//PreparationListenNetWork(portNum_);
		if(lpNetWork.ListSize() <= 0)
		{
			active_ = ActiveState::Lost;
			//lpNetWork.SetNetWorkMode(NetWorkMode::NON);
			StopListenNetWork();
		}
		return false;
	}
	return true;
}

std::pair<bool, int> HostState::GetConnect()
{
	return std::pair<bool, int>(connect_, COUNT_LIMIT - std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
}
