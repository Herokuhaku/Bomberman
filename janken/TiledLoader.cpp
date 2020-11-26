#include "TiledLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include "_debug/_DebugConOut.h"

namespace xml = rapidxml;
using rapid_node = xml::xml_node<>*;
using rapid_atr = xml::xml_attribute<>*;

TmxData TiledLoader::ReadTmx(std::string filename)
{
	filename += ".tmx";
	xml::xml_document<> doc; //コールする
	MapData.clear();

	try
	{
		xml::file<>input(filename.c_str());
		doc.parse<0>(input.data());
		// 親データ
		rapid_node node = doc.first_node("map");	// これでマップを探す

		// 親データの弟データ
		for (rapid_atr atb = node->first_attribute(); atb != nullptr; atb = atb->next_attribute())
		{
			//std::cout << atb->name() << " : [" << atb->value() << "]\n";
			tmxdata_.num.try_emplace(atb->name(), atb->value());
		}
		int id = 0;
		// 子データ
		for (rapid_node child = node->first_node(); child != nullptr; child = child->next_sibling())
		{
			//	std::cout << child->name() << " : [" << child->value() << "]\n";
			tmxdata_.num.try_emplace(child->name(), child->value());
			TmxLayerData lay_ = { -1,-1,-1,"Non" };
			// 子データの弟データ
			for (rapid_atr atb = child->first_attribute(); atb != nullptr; atb = atb->next_attribute())
			{
				//	std::cout << atb->name() << " : [" << atb->value() << "]\n";
				tmxdata_.num.try_emplace(atb->name(), atb->value());
				std::string _name = atb->name();
				if (_name == "id")
				{
					id = atoi(atb->value());
					lay_.Layerid = id;
					layer_.try_emplace(id, lay_);
				}
				else
				{
					if (_name == "name") {
						lay_.name = atb->value();
					}
					else if (_name == "width") {
						lay_.width = atoi(atb->value());
					}
					else if (_name == "height") {
						lay_.height = atoi(atb->value());
					}
					layer_[id] = lay_;
				}
			}
			// 孫データ
			for (rapid_node son = child->first_node(); son != nullptr; son = son->next_sibling())
			{
				//	std::cout << son->name() << son->value() << "\n";
					// マップデータ
				std::string s = son->value();
				s.erase(0, 2);
				MapData.try_emplace(layer_[id].name,s);
			}
		}
		for (auto& d : MapData)
		{
			std::vector<unsigned char>num;
			tmxdata_.MapData.try_emplace(d.first, num);
			std::string save;
			std::stringstream line(d.second);
			while (getline(line, save, ','))
			{

				if (save.find("\r") != std::string::basic_string::npos)
				{
					//num.emplace_back('\r');
					//num.emplace_back('\n');
					num.emplace_back(atoi(save.c_str()));
				}else{
					num.emplace_back(atoi(save.c_str()));
				}
			}
			tmxdata_.MapData[d.first] = num;
		}
	}
	catch (...)
	{
		TmxData errordata = {};
		TRACE("そのファイルはないよ！\n");
		return errordata;
	}

	
	return tmxdata_;
}

TsxData TiledLoader::ReadTsx(std::string filename)
{
	filename += ".tsx";
	xml::xml_document<> doc; //コールする

	try
	{
		xml::file<>input(filename.c_str());
		doc.parse<0>(input.data());


		rapid_node node = doc.first_node("tileset");	// これでマップを探す

		// 親データの弟データ
		for (rapid_atr atb = node->first_attribute(); atb != nullptr; atb = atb->next_attribute())
		{
		//	std::cout << atb->name() << " : [" << atb->value() << "]\n";
			tsxdata_.num.try_emplace(atb->name(), atb->value());
		}
		// 子データ
		for (rapid_node child = node->first_node(); child != nullptr; child = child->next_sibling())
		{
			// 子データの弟データ
			for (rapid_atr atb = child->first_attribute(); atb != nullptr; atb = atb->next_attribute())
			{
				std::string _name = atb->name();
				if (_name == "source")
				{
					tsxdata_.pass = atb->value();
				}
			//	std::cout << atb->name() << " : [" << atb->value() << "]\n";
				tsxdata_.num.try_emplace(atb->name(), atb->value());
			}
		}
	}
	catch (...)
	{
		TsxData errordata = {};
		TRACE("そのファイルはないよ！\n");
		return errordata;
	}
	// 親データ
//	rapid_node node = doc.first_node("");	// これでマップを探す

	// 親データの弟データ
	//for (rapid_atr atb = node->first_attribute(); atb != nullptr; atb = atb->next_attribute())
	//{
	//	std::cout << atb->name() << " : [" << atb->value() << "]\n";
	//	data_.num.try_emplace(atb->name(), atb->value());
	//}
	tsxdata_.pass = tsxdata_.pass.substr(tsxdata_.pass.find_last_of("/"));
	tsxdata_.pass = "Tiled/image/" + tsxdata_.pass;
	return tsxdata_;
}

TmxData TiledLoader::GetTmx(void)
{
	return tmxdata_;
}

std::vector<unsigned char> TiledLoader::TmxCsv(void)
{
	std::vector<unsigned char> csvdata_;
	std::ifstream ifp("Tiled/mapdata/map.tmx");
	std::string save;
	do
	{
		while (std::getline(ifp, save))
		{
			if (save.find("data encoding") != std::string::basic_string::npos)
			{
				break;
			}
		}
		while (std::getline(ifp, save,','))
		{
			int pos;
			if (pos = save.find("/data") != std::string::basic_string::npos)
			{
				break;
			}
			csvdata_.emplace_back(atoi(save.c_str()));
		}
	} while (!ifp.eof());

	return csvdata_;
}

void TiledLoader::SetTmxSize(unionData data)
{
	tmxdata_.num["width"] = std::to_string(data.cData[0]);
	tmxdata_.num["height"] = std::to_string(data.cData[1]);
	tmxdata_.num["layer"] = std::to_string(data.cData[2]);
}

TiledLoader::TiledLoader()
{
}

TiledLoader::~TiledLoader()
{
}
