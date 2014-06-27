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

//map<int, double> initializeMap(string mainPath)
void GeneticValues::initializeMap(string mainPath)
{
	//map<int, double> m;

	HANDLE hFind;
	WIN32_FIND_DATA data;
	mainPath = "c:\\";
	//string fullFilePath = mainPath + "\\" + "*.chr";
	string fullFilePath = "c:\\bestValues.txt";
	Broodwar << "looking for file:" << fullFilePath << endl;
	std::wstring stemp = utf8_decode(fullFilePath);
	LPCWSTR filePath = stemp.c_str();
	
	//hFind = FindFirstFile(filePath, &data);
	//if (hFind != INVALID_HANDLE_VALUE) {
		//do {
			//_tprintf (TEXT("File found is %s\n"), data.cFileName);

			string fileChar = utf8_encode(data.cFileName);
			//cout << fileChar << "  fullFile:" << mainPath+fileChar << endl;

			char fullPathChar[1024];
			char fullPathLockChar[1024];

			string fullPath = "c:\\bestValues.txt";
			//string fullPathLock = fullPath + ".lock";

			//Broodwar << "fileChar: " << fileChar << endl;
			//paramsFile = fileChar;

			//strncpy_s(fullPathChar, fullPath.c_str(), sizeof(fullPathChar));
			//strncpy_s(fullPathLockChar, fullPathLock.c_str(), sizeof(fullPathLockChar));
			//fullPathChar[sizeof(fullPathChar) - 1] = 0;
			//fullPathLockChar[sizeof(fullPathLockChar) - 1] = 0;

			Broodwar << "Will read params from " << fullPath << endl;
			// Read the file and get the values into a map
			string line;
			ifstream myfile (fullPath);
			if (myfile.is_open()) {
				while (getline (myfile,line) ) {
					std::istringstream iss(line);
					
					int key;
					double value;
					if (!(iss >> key >> value)) break;  // error

					//cout << key << " -> " << value << '\n';
					map_instance[key] = value;
				}
				myfile.close();
			}

			//cout << m["aa"] << endl;

			// Rename the file to append the .lock extension
			//int resultRename = rename( fullPathChar , fullPathLockChar );
			//if (resultRename == 0){
				//cout << "File successfully renamed" << endl;
			//	break;
			//}
			//else{
				//cout << "Error renaming file, continue to next one" << endl;
			//}
			//break;
		//} while (FindNextFile(hFind, &data));
		//FindClose(hFind);
	//}
	//return m;
}

//map<int, double> m = initializeMap("c:\\test_files\\");

/*
double GeneticValues::getValue(int key){
	return m[key];
}
*/
