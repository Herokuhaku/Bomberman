#include <fstream>
#include <iostream>
#include <string>
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
		auto data = lpNetWork.GetNetWorkHandle();
		int revcount_ = 0;	
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
						MesList bomblist;
						for (auto& rev : revlist[tmpdata[0].iData / 5].first)
						{
							if (rev.first == MesType::SET_BOMB);bomblist.emplace_back(rev);
						}
						revlist[tmpdata[0].iData / 5].first.clear();
						revlist[tmpdata[0].iData / 5].first = bomblist;
						
						SavePacket data = std::pair<MesType,MesPacket>(tmp.type,tmpdata);
						{
							std::lock_guard<std::mutex> mut(mtx_);
							revlist[tmpdata[0].iData / 5].first.insert(revlist[tmpdata[0].iData / 5].first.begin(),data);
						}
						break;
					}
					if (tmp.type == MesType::SET_BOMB)
					{
						MesPacket u;
						for (auto& d : tmpdata)
						{
							unionData uni;
							uni = d;
							u.emplace_back(uni);
						}
						SavePacket data = std::pair<MesType, MesPacket>(tmp.type, u);
						{
							std::lock_guard<std::mutex> mut(mtx_);
							revlist[tmpdata[0].iData / 5].first.emplace_back(data);
						}
						break;
					}
					if (tmp.type == MesType::TMX_DATA)
					{
						{
							std::lock_guard<std::mutex> mut(mtx_);
							for (auto& d : tmpdata)
							{
								if (revtmx.size() <= revcount_)
								{
									break;
								}
								revtmx[revcount_] = d;
								revcount_++;
							}
						}
						if (tmp.next)
						{
							continue;
						}
						break;
					}
					if (tmp.type == MesType::TMX_SIZE)
					{
						//revtmx.reserve(tmpdata[0]);
						unionData uni;
						uni = tmpdata[0];
						lpTiledLoader.SetTmxSize(uni);
						num["width"] = uni.cData[0];
						num["height"] = uni.cData[1];
						num["layer"] = uni.cData[2];
						uni.iData = uni.cData[0] * uni.cData[1] * uni.cData[2];
						uni.iData /= 8;
						if (uni.iData % 8 != 0) { uni.iData++; }
						//revtmx.resize(uni.iData);
						revtmx.resize(uni.iData);
						//TRACE("tmp.lengthが%d\n　revtmxをリサイズ : %d\n", tmp.length, tmpdata[0]);
						begin = lpSceneMng.GetNowTime();
						break;
					}

				}
				if (tmp.type == MesType::STANBY)
				{
					OutCsv();		// 送られてきたデータに","と"\n"を付加してファイルを作成する
					OutData();		// csvと元々あるデータを参考にtmxデータを作成する
					end = lpSceneMng.GetNowTime();
					std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
					TRACE("ゲストへ通達   :   ホストの準備ができたよ\n");
					lpNetWork.SetRevStandby(true);
					break;
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
		return false;
	}
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
				if (id < 357 * num["layer"])
				{
					if (f == 0)
					{
						tmp[f] = onedata[c] & 0x0f;
					}
					else
					{
						tmp[f] = onedata[c] >> 4;
					}
					if (id % num["width"] != 0)
					{
						fp << ",";
					}
					if (id % num["width"] == 0 && id != 0)
					{
						if (id % (num["width"]*num["height"]) != 0)fp << ",";
						fp << "\n";
					}
					fp << static_cast<int>(tmp[f]);
				}
				id++;
			}
		}
	}
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
				while (count++ < num["height"] && getline(csvf, hozon))
				{
					ofp << hozon;
					ofp << "\n";
					rpos += hozon.size() + 2;
				}
			}
		}
	} while (!ifp.eof());
}
