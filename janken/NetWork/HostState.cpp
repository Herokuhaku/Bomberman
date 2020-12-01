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
}

HostState::~HostState()
{
}

bool HostState::CheckNetWork(void)
{
	end = std::chrono::system_clock::now();
	int handle = GetNewAcceptNetWork();
	if(active_ != ActiveState::Wait && active_ != ActiveState::Non && active_ != ActiveState::Matching)
	{
		MesHeader tmp;
		int revcount_ = 0;
		while (ProcessMessage() == 0 && GetLostNetWork() == -1)
		{
			if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) >= sizeof(MesHeader))
			{
				NetWorkRecv(lpNetWork.GetNetWorkHandle(), &tmp, sizeof(MesHeader));
				MesPacket tmpdata;
				tmpdata.resize(tmp.length);
				if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) > tmp.length)
				{
					NetWorkRecv(lpNetWork.GetNetWorkHandle(), tmpdata.data(), tmp.length * 4);
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
		}
	}
	else if (active_ == ActiveState::Wait)
	{
		if (handle != -1)
		{
			lpNetWork.AddList({ handle,0 });
			TRACE("ê⁄ë±Ç≥ÇÍÇΩÇÊ\n");
			active_ = ActiveState::Matching;
			begin = std::chrono::system_clock::now();
			//StopListenNetWork();
			//active_ = ActiveState::Init;
		}
	}
	else if (active_ == ActiveState::Matching)
	{
		if (handle != -1)
		{
			lpNetWork.AddList({ handle,0 });
			chronoi time{ std::chrono::system_clock::now() };
			time.now = begin;
			lpNetWork.SendMesData(MesType::COUNT_DOWN_ROOM,{time.uninow[0], time.uninow[1]},handle);
			TRACE("ê⁄ë±Ç≥ÇÍÇΩÇÊ\n");
		}
		int seconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		TRACE("%d \n",COUNT_LIMIT - seconds);
		if (seconds >= COUNT_LIMIT)
		{
			active_ = ActiveState::Init;
			StopListenNetWork();
		}
	}
	int lost = GetLostNetWork();
	if (lost != -1)
	{
		TRACE("ê⁄ë±Ç™êÿÇÍÇΩÇÊ\n");
		lpNetWork.RemoveList(lost);
		//PreparationListenNetWork(portNum_);
		if(lpNetWork.ListSize() >= 0)
		{
			active_ = ActiveState::Non;
			lpNetWork.SetNetWorkMode(NetWorkMode::NON);
		}
		return false;
	}
	return true;
}
