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
	static void initializeMap(std::string path);
	static std::map<int, double> getMap();
	static std::string getParamsFile();
	//static double getValue(int key);
	
};
