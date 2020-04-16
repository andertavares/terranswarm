#include "GeneticValues.h"
#include <BWAPI.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <windows.h>

using namespace BWAPI;
using namespace std;

std::map<int, double> GeneticValues::map_instance; //definition, prevents link error
std::string GeneticValues::paramsFileName = "";
std::string GeneticValues::paramsFilePath = "";

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

/**
 * Returns the name of the parameter file that was read
 */
string GeneticValues::getParamsFile(){
	return paramsFileName;
}

/**
 * Returns the full path to the parameter file that was read
 */
string GeneticValues::getParamsFilePath(){
	return paramsFilePath;
}


//map<int, double> initializeMap(string mainPath)
void GeneticValues::initializeMap(string mainPath)
{
	//map<int, double> m;

	HANDLE hFind;
	WIN32_FIND_DATA data;
	//string mainPath = "c:\\test_files\\";
	string fullFilePath = mainPath + "\\" + "*.chr";
	Broodwar << "Parameter seek pattern:" << fullFilePath << endl;
	std::wstring stemp = utf8_decode(fullFilePath);
	LPCWSTR filePath = stemp.c_str();
	
	hFind = FindFirstFile(filePath, &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			
			string fileChar = utf8_encode(data.cFileName);
			
			char fullPathChar[1024];
			char fullPathLockChar[1024];

			string fullPath = mainPath + "\\" + fileChar;
			string fullPathLock = fullPath + ".lock";

			//Broodwar << "fileChar: " << fileChar << endl;

			//updates class attributes (can be queried later)
			paramsFileName = fileChar;
			paramsFilePath = fullPath;

			strncpy_s(fullPathChar, fullPath.c_str(), sizeof(fullPathChar));
			strncpy_s(fullPathLockChar, fullPathLock.c_str(), sizeof(fullPathLockChar));
			fullPathChar[sizeof(fullPathChar) - 1] = 0;
			fullPathLockChar[sizeof(fullPathLockChar) - 1] = 0;

			Broodwar << "Will read params from " << fullPathChar << endl;
			// Read the file and get the values into a map
			string line;
			ifstream myfile (fullPathChar);
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
				break;
			}

			//cout << m["aa"] << endl;
			
			// Rename the file to append the .lock extension
			/*int resultRename = rename( fullPathChar , fullPathLockChar );
			
			if (resultRename == 0){
				Broodwar << "File successfully renamed" << endl;
				Broodwar << "From: " << fullPathChar << endl;
				Broodwar << "to: " << fullPathLockChar << endl;
				break;
			}
			else{
				Broodwar << "Error renaming file, continue to next one" << endl;
				Broodwar << "From: " << fullPathChar << endl;
				Broodwar << "to: " << fullPathLockChar << endl;
				Broodwar << "reason: " << strerror(errno) << endl;
			}
			//break;
			*/
			
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
	//return m;
}

/**
 * Appends the .lock at the parameter file it has read before
 */
int GeneticValues::lockParamsFile(){
	// Rename the file to append the .lock extension

	if (getParamsFilePath().compare("")) {
		Broodwar << "ERROR: no parameter file was found to lock";
	}

	//string lockFile = getParamsFile() + ".lock";
	//const char* pfile = getParamsFilePath().c_str();
	//const char* lockFile = (getParamsFilePath() + ".lock").c_str();

	/*
	char fullPathChar[1024];
	char fullPathLockChar[1024];

	strncpy_s(fullPathChar, getParamsFilePath().c_str(), sizeof(fullPathChar));
	strncpy_s(fullPathLockChar, (getParamsFilePath() + ".lock").c_str(), sizeof(fullPathLockChar));
	fullPathChar[sizeof(fullPathChar) - 1] = 0;
	fullPathLockChar[sizeof(fullPathLockChar) - 1] = 0;
	*/

	//return rename( pfile , lockFile );
	//return rename( "nonexiste" , "estetampoco");
	/*
	ofstream errfile("c:\\debug.err", ios_base::app);
	errfile << "fullPathChar: " << fullPathChar << endl;
	errfile << "fullPathLockChar: " << fullPathLockChar << endl;
	//errfile << "pfile: " << pfile << endl;
	//errfile << "lockfile: " << lockFile << endl;
	errfile << "pfile.c_str: " << getParamsFilePath().c_str() << endl;
	errfile << "lockfile.c_str: " << (getParamsFilePath() + ".lock").c_str() << endl;
	errfile.close();
	*/

	return rename( getParamsFilePath().c_str() , (getParamsFilePath() + ".lock").c_str() );
}

//map<int, double> m = initializeMap("c:\\test_files\\");

/*
double GeneticValues::getValue(int key){
	return m[key];
}
*/
