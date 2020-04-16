#pragma once
#include <BWAPI.h>
#include <map>
#include <string>

using namespace BWAPI;


//std::map<int, double> initializeMap(std::string path);

class GeneticValues {
	static std::map<int, double> map_instance;
	static std::string paramsFileName; //only the file name
	static std::string paramsFilePath; //full path to the file

public:
	static void initializeMap(std::string path);
	static std::map<int, double> getMap();
	static std::string getParamsFile();
	static std::string getParamsFilePath();
	static int lockParamsFile();
};
