#pragma once
#include <BWAPI.h>
#include <map>
#include <string>

using namespace BWAPI;


//std::map<int, double> initializeMap(std::string path);

class GeneticValues {
	static std::map<int, double> map_instance;

public:
	static void initializeMap(std::string path);
	static std::map<int, double> getMap();
	//static double getValue(int key);
	
};
