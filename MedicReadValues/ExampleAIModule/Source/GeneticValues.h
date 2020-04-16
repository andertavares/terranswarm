#pragma once
#include <BWAPI.h>
#include <map>
#include <string>

using namespace BWAPI;


//std::map<int, double> initializeMap(std::string path);

class GeneticValues {
	static std::map<int, double> map_instance;
	static std::string paramsFile;

public:
	static bool loadFromFile(std::string path);
	static std::map<int, double> getMap();
	static std::string getParamsFile();

	static void loadDefaultVsTerran();
	static void loadDefaultVsProtoss();
	static void loadDefaultVsZerg();
	static void loadDefaultVsUnknown();
	//static double getValue(int key);
	
};
