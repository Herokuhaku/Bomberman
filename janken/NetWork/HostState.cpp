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
				TRACE("�z�X�g���֒ʒB   :   �Q�X�g�̏������ł�����\n");
				active_ = ActiveState::Play;
			}
		}
	}
	int handle = GetNewAcceptNetWork();
	if (handle != -1)
	{
		networkHandle_ = handle;
		TRACE("�ڑ����ꂽ��\n");
		StopListenNetWork();
		active_ = ActiveState::Init;
	}
	if (GetLostNetWork() != -1)
	{
		TRACE("�ڑ����؂ꂽ��\n");
		//PreparationListenNetWork(portNum_);
		active_ = ActiveState::Non;
		lpNetWork.SetNetWorkMode(NetWorkMode::NON);
		
		return false;
	}
	return true;
}
