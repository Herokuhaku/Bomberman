#include <fstream>
#include <iostream>
#include <string>
#include "NetWorkState.h"
#include "../TiledLoader.h"
#include "../Scene/SceneMng.h"

NetWorkState::NetWorkState() :timestart_{std::chrono::system_clock::now()}
{
	result_.fill(-1);
	active_ = ActiveState::Non;
	MesTypeList_.try_emplace(MesType::POS, [&](MesHeader tmp,MesPacket tmpdata,int&,int hl) {
		bool flag_ = false;	// breakだとforから抜けるだけなのでwhileから抜けるためにフラグを用意
		if (GetNetWorkDataLength(hl) > static_cast<int>(sizeof(tmp)))
		{
			MesPacket t;
			t.resize(2);
			while ((tmp.length >= 1 && !flag_) || tmpdata.size() < 2)
			{
				for (int i = 0;i + 1 < tmpdata.size(); i++)
				{
					if (tmpdata[i].iData == 108 && tmpdata[i + 1].iData == 4)
					{
						if (tmpdata.size() == 2)
						{
							flag_ = true;
						}
						int mem = 0;		// tmpdataに108 4のデータが紛れてた場合そこを消して次の数値を持ってくる
						// 
						for (int k = i;k + 2 < tmpdata.size();k++)
						{
							tmpdata[k].iData = tmpdata[k + 2].iData;
							mem = k + 1;
						}
						if (mem == 0)
						{
							mem = i;
						}
						NetWorkRecv(hl, t.data(), 8);
						tmpdata[mem++] = t[0];
						tmpdata[mem++] = t[1];
						flag_ = false;
					}
					else
					{
						flag_ = true;
					}
				}
			}
		}
		bool flag = false;
		if (static_cast<unsigned int>(revlist.size()) < static_cast<unsigned int>(tmpdata[0].iData / 5) + 1 ||
			lpNetWork.GetActive() != ActiveState::Play || (tmpdata.size() >= 4 &&
			(tmpdata[1].iData <= 0 || tmpdata[2].iData <= 0 || tmpdata[1].iData > lpSceneMng.GetScreenSize().x || tmpdata[2].iData > lpSceneMng.GetScreenSize().y)))
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
		if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST) {
			lpNetWork.SendMesNotAll(MesType::POS, tmpdata,hl);
		}
		BombCheck(hl);
			return true;
		});

	MesTypeList_.try_emplace(MesType::SET_BOMB, [&](MesHeader tmp, MesPacket tmpdata,int&,int hl) {
		if (static_cast<unsigned int>(revlist.size()) < static_cast<unsigned int>(tmpdata[0].iData / 5) + 1 ||
			lpNetWork.GetActive() != ActiveState::Play || (tmpdata.size() >= 4 &&
				(tmpdata[2].iData <= 0 || tmpdata[3].iData <= 0 || tmpdata[2].iData > lpSceneMng.GetScreenSize().x || tmpdata[3].iData > lpSceneMng.GetScreenSize().y)))
		{
			return true;
		}

		__int64 seconds = std::chrono::duration_cast<std::chrono::milliseconds>(lpSceneMng.GetNowTime() - lpNetWork.TimeStart().now).count();
		if (tmpdata[0].iData / 5 == tmpdata[1].iData / 5 && tmpdata[0].iData != tmpdata[1].iData &&
			mesFlag_[MesType::COUNT_DOWN_GAME] && seconds >= START_LIMIT)
		{
			if (static_cast<unsigned int>(revlist.size()) < static_cast<unsigned int>(tmpdata[0].iData / 5) + 1 ||
				lpNetWork.GetActive() != ActiveState::Play)
			{
				return true;
			}
			SavePacket data = std::pair<MesType, MesPacket>(tmp.type, tmpdata);
			{
				std::lock_guard<std::mutex> mut(revlist[tmpdata[0].iData / 5].second);
				revlist[tmpdata[0].iData / 5].first.emplace_back(data);
			}
			if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST) {
				lpNetWork.SendMesNotAll(MesType::SET_BOMB, tmpdata,hl);
			}
		}
		BombCheck(hl);
		return true;
		});

	MesTypeList_.try_emplace(MesType::TMX_DATA, [&](MesHeader tmp, MesPacket tmpdata, int& revcount_,int) {
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

	MesTypeList_.try_emplace(MesType::TMX_SIZE, [&](MesHeader tmp, MesPacket tmpdata, int&,int) {
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

	MesTypeList_.try_emplace(MesType::STANBY_HOST, [&](MesHeader tmp, MesPacket tmpdata, int&,int) {
		if (lpNetWork.GetActive() != ActiveState::Matching)return true;
		OutCsv();		// 送られてきたデータに","と"\n"を付加してファイルを作成する
		OutData();		// csvと元々あるデータを参考にtmxデータを作成する
		end = lpSceneMng.GetNowTime();
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
		TRACE("ゲストへ通達   :   ホストの準備ができたよ\n");
		lpNetWork.SetRevStandby(true);
		return true;
		});
	MesTypeList_.try_emplace(MesType::COUNT_DOWN_ROOM, [&](MesHeader tmp, MesPacket tmpdata, int&,int) {
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
	MesTypeList_.try_emplace(MesType::ID, [&](MesHeader tmp, MesPacket tmpdata, int&,int) {
		if (tmpdata.size() >= 2 && tmpdata[0].iData % 5 == 0&& tmpdata[0].iData <= tmpdata[1].iData*5-5
			&& !mesFlag_[MesType::ID])
		{
			player.first = tmpdata[0].iData;
			player.second = tmpdata[1].iData;
			mesFlag_[MesType::ID] = true;
		}
		return true;
		});
	MesTypeList_.try_emplace(MesType::STNBY_GUEST, [&](MesHeader tmp, MesPacket tmpdata, int&,int) {
		if (lpNetWork.GetNetWorkMode() != NetWorkMode::HOST)return true;
		int num = lpNetWork.StanbyCountUp(1);
		if (num == lpNetWork.ListSize())
		{
			active_ = ActiveState::Play;
			timestart_.now = lpSceneMng.GetNowTime();
			mesFlag_[MesType::COUNT_DOWN_GAME] = true;
			TRACE("ホスト側へ通達   :   ゲストの準備ができたよ\n");
		}
		return true;
		});
	MesTypeList_.try_emplace(MesType::COUNT_DOWN_GAME, [&](MesHeader tmp, MesPacket tmpdata, int&,int) {
		if (lpNetWork.GetActive() != ActiveState::Play)return true;
		if (tmpdata.size() >= 2 && !mesFlag_[MesType::COUNT_DOWN_GAME])
		{
			timestart_.uninow[0] = tmpdata[0];
			timestart_.uninow[1] = tmpdata[1];
			mesFlag_[MesType::COUNT_DOWN_GAME] = true;
		}
		return true;
		});
	MesTypeList_.try_emplace(MesType::DEATH, [&](MesHeader tmp, MesPacket tmpdata, int&,int hl) {
		for (auto& note : deathnote_)
		{
			if (note == tmpdata[0].iData)return true;
		}
		if (tmpdata.size() == 1)
		{
			if (tmpdata[0].iData < player.second*5)
			{
				deathnote_.emplace_back(tmpdata[0].iData);
			}
		}
		if (lpNetWork.GetNetWorkMode() == NetWorkMode::HOST)
		{
			lpNetWork.SendMesNotAll(MesType::DEATH, tmpdata,hl);
		}
		return true;
		});
	MesTypeList_.try_emplace(MesType::LOST, [&](MesHeader tmp, MesPacket tmpdata, int&,int) {
		for (auto& note : deathnote_)
		{
			if (note == tmpdata[0].iData)return true;
		}
		if (tmpdata.size() == 1)
		{
			if (tmpdata[0].iData < player.second * 5)
			{
				deathnote_.emplace_back(tmpdata[0].iData);
			}
		}
		return true;
	});
	MesTypeList_.try_emplace(MesType::RESULT, [&](MesHeader tmp,MesPacket tmpdata,int&,int) {
		int i = 0;
		//result_.fill(-1);
		std::map<int,bool> check;
		std::array<int, 5> tmpres_;
		tmpres_.fill(-1);
		if (tmpdata.size() > 5)
		{
			return true;
		}
		for (auto& ary : tmpres_)
		{
			if ((tmpdata.size() > i && tmpdata.size() <= result_.size() && check.find(tmpdata[i].iData) == check.end() && 
				!mesFlag_[MesType::RESULT]) || (tmpdata.size() > i && tmpdata[i].iData == -1))
				{
					ary = tmpdata[i++].iData;
					check.emplace(ary,true);
				}
				else {
					return true;
				}
		}
		result_ = tmpres_;
		mesFlag_[MesType::RESULT] = true;
		lpNetWork.SetActive(ActiveState::Result);
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

void NetWorkState::SetResult(std::array<int, 5> result)
{
	result_ = result;
}

std::array<int, 5> NetWorkState::GetResult(void)
{
	return result_;
}

bool NetWorkState::CheckNetWork(void)
{
	return true;
}
std::pair<bool, int> NetWorkState::GetConnect()
{
	return std::pair<bool, int>(false,0);
}
void NetWorkState::AddDeathNote(int id)
{
	for (auto& note : deathnote_)
	{
		if (note == id)return;
	}
	deathnote_.emplace_back(id);
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
			rpos += static_cast<int>(save.size()) + 2;
			if (pos = static_cast<int>(save.find("csv")), pos != std::string::basic_string::npos)
			{
				ofp.seekp(rpos);
				std::string hozon;
				int count = 0;
				while (count++ < num["height"] && getline(csvf, hozon))
				{
					ofp << hozon;
					ofp << "\n";
					rpos += static_cast<int>(hozon.size()) + 2;
				}
			}
		}
	} while (!ifp.eof());
}

std::list<int> NetWorkState::GetDeathNote(void)
{
	return deathnote_;
}

void NetWorkState::BombCheck(int hl)
{
	int revcount_ = 0;
	MesPacket Rev;
	Header head;
	MesPacket tmpbomb;
	MesPacket tmpdeath;
	if (GetNetWorkDataLength(hl) >= 1)
	{
		Rev.resize(GetNetWorkDataLength(hl));
		NetWorkRecv(hl, Rev.data(), GetNetWorkDataLength(hl));
		for (int i = 0;i + 1 < Rev.size();i++)
		{
			if (Rev[i].iData == static_cast<int>(MesType::SET_BOMB) && Rev[i + 1].iData == 7 && i + 8 < Rev.size())
			{
				head.iheader[0] = Rev[i].iData;
				head.iheader[1] = Rev[i + 1].iData;
				for (int k = 1;k < 8;k++)
				{
					tmpbomb.emplace_back(Rev[i + 1 + k]);
				}
				if (tmpbomb.size() == 7) {
					MesTypeList_[head.header.type](head.header, tmpbomb, revcount_, hl);
				}
			}
		}
		for (int i = 0;i + 1 < Rev.size();i++)
		{
			if (Rev[i].iData == static_cast<int>(MesType::DEATH) && Rev[i + 1].iData == 1 && i + 1 < Rev.size())
			{
				head.iheader[0] = Rev[i].iData;
				head.iheader[1] = Rev[i + 1].iData;
				for (int k = 1;k < 2;k++)
				{
					tmpbomb.emplace_back(Rev[i + 1 + k]);
				}
				if (tmpbomb.size() == 1) {
					MesTypeList_[head.header.type](head.header, tmpbomb, revcount_, hl);
				}
			}
		}
	}
}

//void NetWorkState::DeathCheck(int hl)
//{
//	int revcount_ = 0;
//	MesPacket Rev;
//	Header head;
//	MesPacket tmpbomb;
//	if (GetNetWorkDataLength(hl) >= 1)
//	{
//		Rev.resize(GetNetWorkDataLength(hl));
//		NetWorkRecv(hl, Rev.data(), GetNetWorkDataLength(hl));
//		for (int i = 0;i + 1 < Rev.size();i++)
//		{
//			if (Rev[i].iData == 110 && Rev[i + 1].iData == 1 && i + 1< Rev.size())
//			{
//				head.iheader[0] = Rev[i].iData;
//				head.iheader[1] = Rev[i + 1].iData;
//				for (int k = 1;k < 2;k++)
//				{
//					tmpbomb.emplace_back(Rev[i + 1 + k]);
//				}
//				if (tmpbomb.size() == 1) {
//					MesTypeList_[head.header.type](head.header, tmpbomb, revcount_, hl);
//				}
//			}
//		}
//	}
//}
