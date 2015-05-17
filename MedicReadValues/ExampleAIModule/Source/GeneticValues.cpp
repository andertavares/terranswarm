#include "GeneticValues.h"
#include <BWAPI.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <windows.h>

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


void GeneticValues::initializeMap(string mainPath)
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
	}
}