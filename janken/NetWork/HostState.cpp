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
	//if (active_ == ActiveState::Standby)
	if(active_ != ActiveState::Wait && active_ != ActiveState::Non)
	{
		MesHeader tmp;
		int revcount_ = 0;
		int i = 0;
		int id = -1;
		while (ProcessMessage() == 0)
		{
			if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) >= sizeof(MesHeader))
			{
				NetWorkRecv(lpNetWork.GetNetWorkHandle(), &tmp, sizeof(MesHeader));
				MesData tmpdata;
				tmpdata.resize(tmp.length);
				if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) > tmp.length)
				{
					NetWorkRecv(lpNetWork.GetNetWorkHandle(), tmpdata.data(), tmp.length * 4);
					if (tmp.type == MesType::POS)
					{
						id = tmpdata[0];
						{
							std::lock_guard<std::mutex> mut(mtx_);
							if (posdata_[id].size() < tmp.length)
							{
								posdata_[id].resize(tmp.length);
							}
							posdata_[id] = tmpdata;
						}
						break;
					}
				}
				if (tmp.type == MesType::GAME_START)
				{
					TRACE("ホスト側へ通達   :   ゲストの準備ができたよ\n");
					active_ = ActiveState::Play;
					break;
				}
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
