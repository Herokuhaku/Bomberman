#include "GuestState.h"
#include "NetWork.h"
#include "../Scene/SceneMng.h"
#include "../TiledLoader.h"

GuestState::GuestState()
{
	reAccess_ = false;
	hostip_ = {};
	indent = 0;
	savenum = -1;
	revflag_ = false;
}

GuestState::~GuestState()
{
}

ActiveState GuestState::ConnectHost(IPDATA hostip)
{
	networkHandle_ = ConnectNetWork(hostip, portNum_);
	hostip_ = hostip;
	if (networkHandle_ >= 0)
	{
		lpNetWork.AddList(std::pair<int, unsigned int>(networkHandle_, 0));
		active_ = ActiveState::Init;
	}
	else
	{
		active_ = ActiveState::Non;
	}

	return active_;
}

bool GuestState::CheckNetWork(void)
{
	if (active_ != ActiveState::Wait && active_ != ActiveState::Non)
	{
		MesHeader tmp;
		//auto data = lpNetWork.GetNetWorkHandle();
		int revcount_ = 0;
		for (auto& hl : lpNetWork.GetListID())
		{
			while (ProcessMessage() == 0 && GetLostNetWork() == -1)
			{
				if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) >= sizeof(MesHeader))
				{
					NetWorkRecv(hl.first, &tmp, sizeof(MesHeader));
					MesPacket tmpdata;
					tmpdata.resize(tmp.length);
					if (GetNetWorkDataLength(hl.first) >= tmp.length * 4)
					{
						NetWorkRecv(hl.first, tmpdata.data(), tmp.length * 4);
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
	}
	if (GetLostNetWork() != -1)
	{
		CloseNetWork(networkHandle_);
		TRACE("ê⁄ë±Ç™êÿÇÍÇΩÇÊÅI\n\n\n");
		active_ = ActiveState::Non;
		lpNetWork.SetNetWorkMode(NetWorkMode::NON);
		lpNetWork.SetRevStandby(false);
		return false;
	}
	return true;
}

//void GuestState::OutCsv(void)
//{
//	std::ofstream fp("Tiled/mapdata/csv.tmx"/*,std::ios::binary*/);
//	if (fp)
//	{
//		fp.clear();
//	}
//	int id = 0;
//	for (auto& i : revtmx)
//	{
//		int data = 0;
//		unsigned char onedata[4];
//		for (int c = 0; c < 4; c++)
//		{
//			onedata[c] = i.cData[c];
//		}
//		for (int c = 0; c < 4; c++)
//		{
//			unsigned char tmp[2];
//			for (int f = 0; f < 2; f++)
//			{
//				if (id < 357 * num["layer"])
//				{
//					if (f == 0)
//					{
//						tmp[f] = onedata[c] & 0x0f;
//					}
//					else
//					{
//						tmp[f] = onedata[c] >> 4;
//					}
//					if (id % num["width"] != 0)
//					{
//						fp << ",";
//					}
//					if (id % num["width"] == 0 && id != 0)
//					{
//						if (id % (num["width"]*num["height"]) != 0)fp << ",";
//						fp << "\n";
//					}
//					fp << static_cast<int>(tmp[f]);
//				}
//				id++;
//			}
//		}
//	}
//}
//
//void GuestState::OutData(void)
//{
//	std::ifstream ifp("Tiled/mapdata/tmx.dat");
//	std::ofstream ofp("Tiled/mapdata/tmp.tmx");
//	std::ifstream csvf("Tiled/mapdata/csv.tmx");
//	std::string save;
//	int rpos = 0;
//	do
//	{
//		while (getline(ifp, save))
//		{
//			int pos;
//			ofp << save;
//			ofp << "\n";
//			rpos += save.size() + 2;
//			if (pos = save.find("csv"), pos != std::string::basic_string::npos)
//			{
//				ofp.seekp(rpos);
//				std::string hozon;
//				int count = 0;
//				while (count++ < num["height"] && getline(csvf, hozon))
//				{
//					ofp << hozon;
//					ofp << "\n";
//					rpos += hozon.size() + 2;
//				}
//			}
//		}
//	} while (!ifp.eof());
//}
