#include "GeneticValues.h"
#include <BWAPI.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <windows.h>

#include "Parameters.h"

using namespace BWAPI;
using namespace std;

std::map<int, double> GeneticValues::map_instance; //definition, prevents link error
std::string GeneticValues::paramsFile = "";

std::string utf8_encode(const std::wstring &wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

map<int, double> GeneticValues::getMap(){
	return map_instance;
}

string GeneticValues::getParamsFile(){
	return paramsFile;
}

/**
 * Attempts to read the parameters from a file. 
 * Returns a bool indicating success
 */
bool GeneticValues::loadFromFile(string mainPath)
{
	// starcraft path is the root path, no need to specify it
	string partialFilePath = "bwapi-data\\bestValues.txt";

	Broodwar << "looking for file:" << partialFilePath << endl;
	Broodwar << "Will read params from " << partialFilePath << endl;
	
	// Read the file and get the values into a map
	string line;
	ifstream myfile (partialFilePath);
	if (myfile.is_open()) {
		while (getline (myfile,line) ) {
			std::istringstream iss(line);
					
			int key;
			double value;
			if (!(iss >> key >> value)) break;  // error

			map_instance[key] = value;
		}
		myfile.close();
		return true;
	}
	else {
		Broodwar << "Could not read parameters from " << partialFilePath << endl;
		Broodwar << "Will use default params vs. unknown opponent" << endl;

		loadDefaultVsUnknown();
		return false;
	}
}

/**
 * Loads the default vs-terran best values
 * and returns the map
 */
void GeneticValues::loadDefaultVsTerran(){
	map_instance[Params::S_GATHER_MINERALS] =			0.45;
	map_instance[Params::S_BUILD_BARRACKS_DENOMINATOR] =2;
	map_instance[Params::S_BUILD_CMD_CENTER] = 			0.75;
	map_instance[Params::S_ATTACK] = 					1.0;
	map_instance[Params::S_TRAIN_SCV_DENOMINATOR] = 	2.5;
	map_instance[Params::S_TRAIN_MEDIC_RATIO] = 		4.5;
	map_instance[Params::S_TRAIN_MARINE] = 				0.7;
	map_instance[Params::K_SCV_GATHER_MINERALS] = 		0.4;
	map_instance[Params::K_SCV_REPAIR_NEAR] = 			0.75;
	map_instance[Params::K_SCV_REPAIR_MID] = 			0.2;
	map_instance[Params::K_SCV_REPAIR_FAR] = 			0.45;
	map_instance[Params::K_SCV_EXPLORE] = 				0.55;
	map_instance[Params::K_SCV_ATTACK_NEAR] = 			0.95;
	map_instance[Params::K_SCV_ATTACK_MID] = 			0.2;
	map_instance[Params::K_SCV_ATTACK_FAR] = 			0.25;
	map_instance[Params::K_MARINE_EXPLORE] = 			0.7;
	map_instance[Params::K_MARINE_ATTACK_NEAR] = 		0.95;
	map_instance[Params::K_MARINE_ATTACK_MID] = 		0.6;
	map_instance[Params::K_MARINE_ATTACK_FAR] = 		0.65;
	map_instance[Params::K_GENERAL_TRAIN_SCV] = 		0.3;
	map_instance[Params::K_GENERAL_TRAIN_MARINE] = 		1.0;
	map_instance[Params::K_GENERAL_TRAIN_MEDIC] = 		0.45;
	map_instance[Params::M_PACK_SIZE] = 				10;
}

/**
 * Loads the default vs-protoss best values
 * and returns the map
 */
void GeneticValues::loadDefaultVsProtoss(){
	map_instance[Params::S_GATHER_MINERALS] =			0.55;
	map_instance[Params::S_BUILD_BARRACKS_DENOMINATOR] =3;
	map_instance[Params::S_BUILD_CMD_CENTER] = 			1.0;
	map_instance[Params::S_ATTACK] = 					0.95;
	map_instance[Params::S_TRAIN_SCV_DENOMINATOR] = 	2.5;
	map_instance[Params::S_TRAIN_MEDIC_RATIO] = 		5.0;
	map_instance[Params::S_TRAIN_MARINE] = 				0.9;
	map_instance[Params::K_SCV_GATHER_MINERALS] = 		0.6;
	map_instance[Params::K_SCV_REPAIR_NEAR] = 			0.7;
	map_instance[Params::K_SCV_REPAIR_MID] = 			0.4;
	map_instance[Params::K_SCV_REPAIR_FAR] = 			0.25;
	map_instance[Params::K_SCV_EXPLORE] = 				0.2;
	map_instance[Params::K_SCV_ATTACK_NEAR] = 			0.75;
	map_instance[Params::K_SCV_ATTACK_MID] = 			0.55;
	map_instance[Params::K_SCV_ATTACK_FAR] = 			0.45;
	map_instance[Params::K_MARINE_EXPLORE] = 			0.95;
	map_instance[Params::K_MARINE_ATTACK_NEAR] = 		0.9;
	map_instance[Params::K_MARINE_ATTACK_MID] = 		0.2;
	map_instance[Params::K_MARINE_ATTACK_FAR] = 		0.65;
	map_instance[Params::K_GENERAL_TRAIN_SCV] = 		0.7;
	map_instance[Params::K_GENERAL_TRAIN_MARINE] = 		0.95;
	map_instance[Params::K_GENERAL_TRAIN_MEDIC] = 		0.7;
	map_instance[Params::M_PACK_SIZE] = 				12;
}

/**
 * Loads the default vs-zerg best values
 * and returns the map
 */
void GeneticValues::loadDefaultVsZerg(){
	map_instance[Params::S_GATHER_MINERALS] =			0.0;
	map_instance[Params::S_BUILD_BARRACKS_DENOMINATOR] =2;
	map_instance[Params::S_BUILD_CMD_CENTER] = 			0.45;
	map_instance[Params::S_ATTACK] = 					0.05;
	map_instance[Params::S_TRAIN_SCV_DENOMINATOR] = 	2.5;
	map_instance[Params::S_TRAIN_MEDIC_RATIO] = 		4.0;
	map_instance[Params::S_TRAIN_MARINE] = 				0.3;
	map_instance[Params::K_SCV_GATHER_MINERALS] = 		0.3;
	map_instance[Params::K_SCV_REPAIR_NEAR] = 			0.3;
	map_instance[Params::K_SCV_REPAIR_MID] = 			0.6;
	map_instance[Params::K_SCV_REPAIR_FAR] = 			0.6;
	map_instance[Params::K_SCV_EXPLORE] = 				0.25;
	map_instance[Params::K_SCV_ATTACK_NEAR] = 			0.0;
	map_instance[Params::K_SCV_ATTACK_MID] = 			0.6;
	map_instance[Params::K_SCV_ATTACK_FAR] = 			0.35;
	map_instance[Params::K_MARINE_EXPLORE] = 			0.05;
	map_instance[Params::K_MARINE_ATTACK_NEAR] = 		0.05;
	map_instance[Params::K_MARINE_ATTACK_MID] = 		0.65;
	map_instance[Params::K_MARINE_ATTACK_FAR] = 		0.35;
	map_instance[Params::K_GENERAL_TRAIN_SCV] = 		0.9;
	map_instance[Params::K_GENERAL_TRAIN_MARINE] = 		0.95;
	map_instance[Params::K_GENERAL_TRAIN_MEDIC] = 		0.05;
	map_instance[Params::M_PACK_SIZE] = 				10;
}


/**
 * Loads the default vs-unknown values
 * and returns the map
 */
void GeneticValues::loadDefaultVsUnknown(){
	
	map_instance[Params::S_GATHER_MINERALS] =			0.6;
	map_instance[Params::S_BUILD_BARRACKS_DENOMINATOR] =3;
	map_instance[Params::S_BUILD_CMD_CENTER] = 			0.8;
	map_instance[Params::S_ATTACK] = 					0.8;
	map_instance[Params::S_TRAIN_SCV_DENOMINATOR] = 	2.5;
	map_instance[Params::S_TRAIN_MEDIC_RATIO] = 		3.0;
	map_instance[Params::S_TRAIN_MARINE] = 				0.8;
	map_instance[Params::K_SCV_GATHER_MINERALS] = 		0.7;
	map_instance[Params::K_SCV_REPAIR_NEAR] = 			0.9;
	map_instance[Params::K_SCV_REPAIR_MID] = 			0.6;
	map_instance[Params::K_SCV_REPAIR_FAR] = 			0.3;
	map_instance[Params::K_SCV_EXPLORE] = 				0.0;
	map_instance[Params::K_SCV_ATTACK_NEAR] = 			0.9;
	map_instance[Params::K_SCV_ATTACK_MID] = 			0.0;
	map_instance[Params::K_SCV_ATTACK_FAR] = 			0.0;
	map_instance[Params::K_MARINE_EXPLORE] = 			0.3;
	map_instance[Params::K_MARINE_ATTACK_NEAR] = 		0.9;
	map_instance[Params::K_MARINE_ATTACK_MID] = 		0.6;
	map_instance[Params::K_MARINE_ATTACK_FAR] = 		0.3;
	map_instance[Params::K_GENERAL_TRAIN_SCV] = 		0.7;
	map_instance[Params::K_GENERAL_TRAIN_MARINE] = 		0.7;
	map_instance[Params::K_GENERAL_TRAIN_MEDIC] = 		0.4;
	map_instance[Params::M_PACK_SIZE] = 				8;
}
