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
	if(active_ != ActiveState::Wait && active_ != ActiveState::Non)
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
					}else{
						continue;
					}
				}
			}
		}
	}
	int handle = GetNewAcceptNetWork();
	if (handle != -1)
	{
		handle_.push_back({handle,0});
		handleFlag_ = true;
		//networkHandle_ = handle;
		TRACE("ê⁄ë±Ç≥ÇÍÇΩÇÊ\n");
		active_ = ActiveState::Matching;
		StopListenNetWork();
		//active_ = ActiveState::Init;
	}
	if (handleFlag_)
	{
		begin = std::chrono::system_clock::now();
	}
	end = lpSceneMng.GetNowTime();
	if (GetLostNetWork() != -1)
	{
		TRACE("ê⁄ë±Ç™êÿÇÍÇΩÇÊ\n");
		//PreparationListenNetWork(portNum_);
		active_ = ActiveState::Non;
		lpNetWork.SetNetWorkMode(NetWorkMode::NON);
		
		return false;
	}
	return true;
}
