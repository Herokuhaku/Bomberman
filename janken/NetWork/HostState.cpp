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
	if(active_ != ActiveState::Wait && active_ != ActiveState::Non)
	{
		MesHeader tmp;
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
					if (tmp.type == MesType::POS)
					{
						SavePacket data = std::pair<MesType, MesPacket>(tmp.type, tmpdata);
						{
							std::lock_guard<std::mutex> mut(mtx_);
							revlist[tmpdata[0].iData / 5].first.emplace_back(data);
						}
						break;
					}
					else if (tmp.type == MesType::SET_BOMB)
					{
						SavePacket data = std::pair<MesType, MesPacket>(tmp.type, tmpdata);
						{
							std::lock_guard<std::mutex> mut(mtx_);
							revlist[tmpdata[0].iData / 5].first.emplace_back(data);
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
