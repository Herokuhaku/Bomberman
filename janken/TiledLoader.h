#pragma once
#include <string>
#include <vector>
#include <map>
#define lpTiledLoader TiledLoader::GetInstance()

struct TmxData
{
	//	int Layerid,Width,Height,Firstgid;
		//std::map<char*, int> numint;
	std::map<std::string, std::string> num;
	//	std::string Name, Orientation, Renderorder, Tilewidth, TileHeight;
	std::map < std::string, std::vector<unsigned char> > MapData;
};

struct TsxData
{
	std::map<std::string,std::string>num;
	std::string pass;
};

struct TmxLayerData
{
	int Layerid, width, height;
	std::string name;
};

class TiledLoader
{
public:
	static TiledLoader& GetInstance()
	{
		static TiledLoader sInstance;
		return sInstance;
	}
	TmxData ReadTmx(std::string filename);
	TsxData ReadTsx(std::string filename);

	TmxData GetTmx();
	std::vector<unsigned char> TmxCsv(void);
private:
	TmxData tmxdata_;
	TsxData tsxdata_;
	std::map<std::string, std::string> MapData;
	TmxLayerData laydata_;
	std::map<int,TmxLayerData> layer_;
	TiledLoader();
	~TiledLoader();
};

