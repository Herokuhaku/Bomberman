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
		int revcount_ = 0;
		int i = 0;
		int id = 0;
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
						{
							std::lock_guard<std::mutex> mut(mtx_);
							for (auto& d : tmpdata)
							{
								if (i++ == 0)
								{
									id = d;
									TRACE("id :  %d　のPOSを受信したよ\n", id);
									if (posdata_[id].size() < tmp.length)
									{
										posdata_[id].resize(tmp.length);
									}
								}
								posdata_[id][revcount_++].iData = d;
							}
						}
						revcount_ = 0;
						break;
					}
					//if (tmp.type == MesType::POS)
					//{
					//	revdata_.resize(tmp.length);
					//	{
					//		std::lock_guard<std::mutex> mut(mtx_);
					//		for (auto& d : tmpdata)
					//		{
					//			revdata_[revcount_++].iData = d;
					//		}
					//	}
					//	revcount_ = 0;
					//}
				}
				if (tmp.type == MesType::GAME_START)
				{
					TRACE("ホスト側へ通達   :   ゲストの準備ができたよ\n");
					active_ = ActiveState::Play;
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
