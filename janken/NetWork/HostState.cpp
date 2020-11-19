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
		while (ProcessMessage() == 0 && GetLostNetWork() == -1)
		{
			if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) >= sizeof(MesHeader))
			{
				NetWorkRecv(lpNetWork.GetNetWorkHandle(), &tmp, sizeof(MesHeader));
				MesData tmpdata;
				tmpdata.resize(tmp.length);
				revtmx_.first = tmp.type;
				if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) > tmp.length)
				{
					NetWorkRecv(lpNetWork.GetNetWorkHandle(), tmpdata.data(), tmp.length * 4);
					if (tmp.type == MesType::POS)
					{
						if (posdata_[tmpdata[0]].size() < tmp.length)
						{
							posdata_[tmpdata[0]].resize(tmp.length);
						}
						{
							std::lock_guard<std::mutex> mut(mtx_);
							posdata_[tmpdata[0]] = tmpdata;
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
