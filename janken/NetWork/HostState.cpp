#include <DxLib.h>
#include "HostState.h"
#include "NetWork.h"
HostState::HostState()
{
	if (PreparationListenNetWork(portNum_) == 0)
	{
		active_ = ActiveState::Wait;
	}
}

HostState::~HostState()
{
}

bool HostState::CheckNetWork(void)
{
	if (active_ == ActiveState::Standby)
	{
		MesHeader tmp;
		if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) >= sizeof(MesHeader))
		{
			NetWorkRecv(lpNetWork.GetNetWorkHandle(), &tmp, sizeof(MesHeader));
			if (tmp.type == MesType::GAME_START)
			{
				TRACE("ホスト側へ通達   :   ゲストの準備ができたよ\n");
				active_ = ActiveState::Play;
			}
		}
	}
	int handle = GetNewAcceptNetWork();
	if (handle != -1)
	{
		networkHandle_ = handle;
		TRACE("接続されたよ\n");
		StopListenNetWork();
		active_ = ActiveState::Init;
	}
	if (GetLostNetWork() != -1)
	{
		TRACE("接続が切れたよ\n");
		//PreparationListenNetWork(portNum_);
		active_ = ActiveState::Non;
		lpNetWork.SetNetWorkMode(NetWorkMode::NON);
		
		return false;
	}
	return true;
}
