#include <fstream>
#include <iostream>
#include <string>
#include "GuestState.h"
#include "NetWork.h"
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
	if (active_ == ActiveState::Init)
	{
		MesHeader tmp;
		auto data = lpNetWork.GetNetWorkHandle();
		if(GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) >= sizeof(MesHeader))
		{
			NetWorkRecv(lpNetWork.GetNetWorkHandle(), &tmp, sizeof(MesHeader));
			if (tmp.type == MesType::STANBY)
			{
				OutCsv();		// 送られてきたデータに","と"\n"を付加してファイルを作成する
				OutData();		// csvと元々あるデータを参考にtmxデータを作成する
				end = std::chrono::system_clock::now();
				std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
				TRACE("ゲストへ通達   :   ホストの準備ができたよ\n");
				lpNetWork.SetRevStandby(true);
			}
			MesData tmpdata;
			tmpdata.resize(tmp.length);
			if (GetNetWorkDataLength(lpNetWork.GetNetWorkHandle()) > tmp.length)
			{
				NetWorkRecv(lpNetWork.GetNetWorkHandle(), tmpdata.data(), tmp.length*4);
				if (tmp.length > 0)
				{
					if (tmp.type == MesType::TMX_SIZE)
					{
						revtmx.resize(tmpdata[0]);
						TRACE("tmp.lengthが%d\n　revtmxをリサイズ : %d",tmp.length, tmpdata);
						//savenum = tmp.data[0];
						//revtmx.resize(savenum);
						begin = std::chrono::system_clock::now();
						//TRACE("送られてきたTMXのサイズ(%d)でrevtmxをリサイズしたよ\n", tmp.data[0]);*/
					}
					if (tmp.type == MesType::TMX_DATA)
					{
						//revtmx.resize(tmp.length);
						int count = 0;
						{
							std::lock_guard<std::mutex> mut(mtx_);
							for (auto& d : tmpdata)
							{
								//revtmx[tmp.id].iData[ = tmp.data[0];
								revtmx[count++].iData = d;
							}
						}
					}
				}
			}
		}
	}
	if (GetLostNetWork() != -1)
	{
		CloseNetWork(networkHandle_);
		TRACE("接続が切れたよ！\n\n\n");
		active_ = ActiveState::Non;
		lpNetWork.SetNetWorkMode(NetWorkMode::NON);
		lpNetWork.SetRevStandby(false);
//		reAccess_ = true;
		return false;
	}
	//if (reAccess_)
	//{
	//	networkHandle_ = ConnectNetWork(hostip_, portNum_);
	//	if (networkHandle_ != -1)
	//	{
	//		TRACE("接続してるよ\n");
	//		reAccess_ = false;
	//	}
	//}
	return true;
}

void GuestState::OutCsv(void)
{
	std::ofstream fp("Tiled/mapdata/csv.tmx"/*,std::ios::binary*/);
	if (fp)
	{
		fp.clear();
	}
	int id = 0;
	for (auto& i : revtmx)
	{
		int data = 0;
		unsigned char onedata[4];
		for (int c = 0; c < 4; c++)
		{
			onedata[c] = i.cData[c];
		}
		for (int c = 0; c < 4; c++)
		{
			unsigned char tmp[2];
			for (int f = 0; f < 2; f++)
			{
				if (id < 357 * 4)
				{
					if (f == 0)
					{
						tmp[f] = onedata[c] & 0x0f;
					}
					else
					{
						tmp[f] = onedata[c] >> 4;
					}
					if (id % 21 != 0)
					{
						fp << ",";
					}
					if (id % 21 == 0 && id != 0)
					{
						if (id % 357 != 0)fp << ",";
						fp << "\n";
					}
					fp << static_cast<int>(tmp[f]);
				}
				id++;
			}
		}
	}
	//for (auto& i : revtmx)
	//{
	//	int data[2] = { 0,0 };
	//	unsigned char onedata[8];
	//	for (int c = 0; c < 8; c++)
	//	{
	//		onedata[c] = i.cData[c];
	//	}
	//	for (int c = 0; c < 8; c++)
	//	{
	//		unsigned char tmp[2];
	//		for (int f = 0; f < 2; f++)
	//		{
	//			if (id <= 357 * 4 - 1)
	//			{
	//				if (f == 0)
	//				{
	//					tmp[f] = onedata[c] & 0x0f;
	//				}
	//				else
	//				{
	//					tmp[f] = onedata[c] >> 4;
	//				}
	//				if (id % 21 != 0)
	//				{
	//					fp << ",";
	//				}
	//				if (id % 21 == 0 && id != 0)
	//				{
	//					if(id % 357 != 0)fp << ",";
	//					fp << "\n";
	//				}
	//				fp << static_cast<int>(tmp[f]);
	//			}
	//			id++;
	//		}
	//	}
	//}
}

void GuestState::OutData(void)
{
	std::ifstream ifp("Tiled/mapdata/tmx.dat");
	std::ofstream ofp("Tiled/mapdata/tmp.tmx");
	std::ifstream csvf("Tiled/mapdata/csv.tmx");
	std::string save;
	int rpos = 0;
	do
	{
		while (getline(ifp, save))
		{
			int pos;
			ofp << save;
			ofp << "\n";
			rpos += save.size() + 2;
			if (pos = save.find("csv"), pos != std::string::basic_string::npos)
			{
				ofp.seekp(rpos);
				std::string hozon;
				int count = 0;
				while (count++ < 17 && getline(csvf, hozon))
				{
					ofp << hozon;
					ofp << "\n";
					rpos += hozon.size() + 2;
				}
			}
		}
	} while (!ifp.eof());
}
