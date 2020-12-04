#include <fstream>
#include <iostream>
#include <string>
#include "NetWorkState.h"
#include "../TiledLoader.h"
#include "../Scene/SceneMng.h"

NetWorkState::NetWorkState() :timestart_{std::chrono::system_clock::now()}
{
	active_ = ActiveState::Non;
	MesTypeList_.try_emplace(MesType::POS, [&](MesHeader tmp,MesPacket tmpdata,int& revcount_) {
		bool flag = false;
		if (revlist.size() < tmpdata[0].iData / 5 + 1)
		{
			return true;
		}
		for (auto& rev : revlist[tmpdata[0].iData / 5].first)
		{
			if (rev.first == MesType::POS)
			{
				{
					std::lock_guard<std::mutex> mut(revlist[tmpdata[0].iData / 5].second);
					rev.second = tmpdata;
				}
				flag = true;
			}
		}
		if (!flag) {
			SavePacket data = std::pair<MesType, MesPacket>(tmp.type, tmpdata);
			{
				std::lock_guard<std::mutex> mut(revlist[tmpdata[0].iData / 5].second);
				revlist[tmpdata[0].iData / 5].first.insert(revlist[tmpdata[0].iData / 5].first.begin(), data);
			}
		}
	return true;
		});

	MesTypeList_.try_emplace(MesType::SET_BOMB, [&](MesHeader tmp, MesPacket tmpdata,int& revcount_) {
		if (tmpdata.size() >= 2)
		{
			int seconds = std::chrono::duration_cast<std::chrono::milliseconds>(lpSceneMng.GetNowTime() - lpNetWork.TimeStart().now).count();
			if (tmpdata[0].iData / 5 == tmpdata[1].iData / 5 && tmpdata[0].iData != tmpdata[1].iData && 
				mesFlag_[MesType::COUNT_DOWN_GAME] &&  seconds >= START_LIMIT)
			{
				if (revlist.size() < tmpdata[0].iData / 5 + 1)
				{
					return true;
				}
				SavePacket data = std::pair<MesType, MesPacket>(tmp.type, tmpdata);
				{
					std::lock_guard<std::mutex> mut(revlist[tmpdata[0].iData/5].second);
					revlist[tmpdata[0].iData / 5].first.emplace_back(data);
				}
			}
		}
		return true;
		});

	MesTypeList_.try_emplace(MesType::TMX_DATA, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_) {
		if (lpNetWork.GetActive() != ActiveState::Matching)return true;
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
			return false;
		}
		return true;
		});

	MesTypeList_.try_emplace(MesType::TMX_SIZE, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_) {
		if (lpNetWork.GetActive() != ActiveState::Matching)return true;
		unionData uni;
		uni = tmpdata[0];
		lpTiledLoader.SetTmxSize(uni);
		num["width"] = uni.cData[0];
		num["height"] = uni.cData[1];
		num["layer"] = uni.cData[2];
		uni.iData = uni.cData[0] * uni.cData[1] * uni.cData[2];
		uni.iData /= 8;
		if (uni.iData % 8 != 0) { uni.iData++; }
		revtmx.resize(uni.iData);
		begin = lpSceneMng.GetNowTime();
		return true;
		});

	MesTypeList_.try_emplace(MesType::STANBY_HOST, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_) {
		if (lpNetWork.GetActive() != ActiveState::Matching)return true;
		OutCsv();		// 送られてきたデータに","と"\n"を付加してファイルを作成する
		OutData();		// csvと元々あるデータを参考にtmxデータを作成する
		end = lpSceneMng.GetNowTime();
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
		TRACE("ゲストへ通達   :   ホストの準備ができたよ\n");
		lpNetWork.SetRevStandby(true);
		return true;
		});
	MesTypeList_.try_emplace(MesType::COUNT_DOWN_ROOM, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_) {
		mesFlag_.try_emplace(MesType::COUNT_DOWN_ROOM,false);
		if (tmpdata.size() >= 2 && !mesFlag_[MesType::COUNT_DOWN_ROOM])
		{
			timestart_.uninow[0] = tmpdata[0];
			timestart_.uninow[1] = tmpdata[1];
			active_ = ActiveState::Matching;
			mesFlag_[MesType::COUNT_DOWN_ROOM] = true;
		}
		return true;
		});
	MesTypeList_.try_emplace(MesType::ID, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_) {
		if (tmpdata.size() >= 2 && tmpdata[0].iData % 5 == 0&& tmpdata[0].iData <= tmpdata[1].iData*5-5
			&& !mesFlag_[MesType::ID])
		{
			player.first = tmpdata[0].iData;
			player.second = tmpdata[1].iData;
			mesFlag_[MesType::ID] = true;
		}
		return true;
		});
	MesTypeList_.try_emplace(MesType::STNBY_GUEST, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_) {
		if (lpNetWork.GetNetWorkMode() != NetWorkMode::HOST)return true;
		int num = lpNetWork.StanbyCountUp(1);
		if (num == lpNetWork.ListSize())
		{
			active_ = ActiveState::Play;
			timestart_.now = lpSceneMng.GetNowTime();
			TRACE("ホスト側へ通達   :   ゲストの準備ができたよ\n");
		}
		return true;
		});
	MesTypeList_.try_emplace(MesType::COUNT_DOWN_GAME, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_) {
		if (lpNetWork.GetActive() != ActiveState::Play)return true;
		if (tmpdata.size() >= 2 && !mesFlag_[MesType::COUNT_DOWN_GAME])
		{
			timestart_.uninow[0] = tmpdata[0];
			timestart_.uninow[1] = tmpdata[1];
			mesFlag_[MesType::COUNT_DOWN_GAME] = true;
		}
		return true;
		});
	MesTypeList_.try_emplace(MesType::DEATH, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_) {
		for (auto& note : deathnote_)
		{
			if (note == tmpdata[0].iData)return true;
		}
		if (tmpdata.size() >= 1)
		{
			deathnote_.emplace_back(tmpdata[0].iData);
		}
		return true;
		});
	MesTypeList_.try_emplace(MesType::LOST, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_) {
		for (auto& note : deathnote_)
		{
			if (note == tmpdata[0].iData)return true;
		}
		if (tmpdata.size() >= 1)
		{
			deathnote_.emplace_back(tmpdata[0].iData);
		}
		return true;
		});

}

NetWorkState::~NetWorkState()
{
}

ActiveState NetWorkState::GetActive(void)
{
	return active_;
}

bool NetWorkState::SetActive(ActiveState act)
{
	active_ = act;
	return true;
}

ActiveState NetWorkState::ConnectHost(IPDATA hostip)
{
	return active_;
}

int NetWorkState::GetNetWorkHandle(void)
{
	return networkHandle_;
}

void NetWorkState::SetNetWorkHandle(int nethandle)
{
	networkHandle_ = nethandle;
}

bool NetWorkState::CheckNetWork(void)
{
	return true;
}
void NetWorkState::SetPlayerList(int id, MesList& list, std::mutex& mtx)
{
	revlist.emplace_back(std::pair<MesList&,std::mutex&>(list,mtx));
}

chronoi NetWorkState::TimeStart(void)
{
	return timestart_;
}

void NetWorkState::SetTimeStart(std::chrono::system_clock::time_point time)
{
	timestart_.now = time;
}

std::pair<int, int> NetWorkState::PlayerID(void)
{
	return player;
}

void NetWorkState::SetPlayerID(std::pair<int, unsigned int> pl)
{
	player = pl;
}

void NetWorkState::DeletePlayerList()
{
	revlist.clear();
	revlist.shrink_to_fit();
}

void NetWorkState::OutCsv(void)
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
		unsigned char onedata[4] = {0,0,0,0};
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
						if (id % (num["width"] * num["height"]) != 0)fp << ",";
						fp << "\n";
					}
					fp << static_cast<int>(tmp[f]);
				}
				id++;
			}
		}
	}
}

void NetWorkState::OutData(void)
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

std::list<int> NetWorkState::GetDeathNote(void)
{
	return deathnote_;
}
