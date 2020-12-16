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
						if (GetNetWorkDataLength(hl.first) > static_cast<int>(tmp.length))
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
	else if (active_ == ActiveState::Wait)
	{
		if (handle != -1)
		{
			//lpNetWork.AddList({ handle,0 });
			//TRACE("�ڑ����ꂽ��\n");
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
			TRACE("�ڑ����ꂽ��\n");
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
		TRACE("�ڑ����؂ꂽ��\n");
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
