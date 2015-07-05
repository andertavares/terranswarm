#pragma once

#include <BWAPI.h>
#include <vector>
#include <unordered_map>
#include <list>
#include "Parameters.h"
#include "Task.h"
#include "CommanderAgent.h"
#include "MarineAgent.h"
#include "FirebatAgent.h"
#include "GoliathAgent.h"
#include "VultureAgent.h"
#include "SCVAgent.h"
#include "MedicAgent.h"


#define BASE_RADIUS 20 * TILE_SIZE

using namespace std;
using namespace BWAPI;

class SCVAgent; //forward decl. to prevent compile error

typedef unordered_map<int, SCVAgent*> SCVMap;
typedef unordered_map<int, MarineAgent*> MarineMap;
typedef unordered_map<int, FirebatAgent*> FirebatMap;
typedef unordered_map<int, GoliathAgent*> GoliathMap;
typedef unordered_map<int, VultureAgent*> VultureMap;
typedef unordered_map<int, MedicAgent*> MedicMap;

// Remember not to use "Broodwar" in any global class constructor!

class ExampleAIModule : public BWAPI::AIModule {

	//stores whether the game has lasted too long and should end as a draw
	bool timeOver;


	string startTime, endTime; //start and finishing times, stored as strings

	//relative path where the chromosome files are located
	string workingDir;

	//parameters that are used here and will be evolved by the GA
	map<int, double> parameters;

	//tasks that will have a single instance
	Task* gatherMinerals;
	Task* trainMarine;
	Task* trainFirebat;
	Task* trainGoliath;
	Task* trainVulture;
	Task* buildSupplyDepot;
	Task* explore;
	Task* buildCommandCenter;
	Task* buildVespeneGas;
	Task* buildAcademy;
	Task* buildArmory;
	Task* trainMedic;
	Task* researchAcademyStimpack;
	Task* researchAcademyLongRange;
	Task* researchAcademyOpticalFlare;
	Task* researchArmoryWeapon;
	Task* researchArmoryPlating;
	Task* buildBunker;

	Unit ourComSat;

	CommanderAgent* _commanderAgent;

	//number of minerals found out of base range
	//int mineralsOutOfBaseRange;

	int lastScan;

	//set of command centers
	Unitset commandCenters;

	//set of minerals that were discovered
	vector<Position> discoveredMineralPositions;

	//map: taskType to list<task> for tasks that may have multiple instances with variable incentives
	unordered_map<TaskType, vector<Task>*> allTasks;

	//stores the incentives to build SCVs per base
	unordered_map<Unit, float> trainSCVIncentives;

	//keeps track of the barracks that were scheduled to be built around a command center
	unordered_map<Unit, int> scheduledBarracks;

	//keeps track of the barracks built around the command centers
	unordered_map<Unit, int> builtBarracks;

	//incentive to build barracks around the command centers
	unordered_map<Unit, float> buildBarracksIncentives;

	//keeps track of the factories that were scheduled to be built around a command center
	unordered_map<Unit, int> scheduledFactory;

	//keeps track of the factories built around the command centers
	unordered_map<Unit, int> builtFactory;

	//incentive to build factories around the command centers
	unordered_map<Unit, float> buildFactoryIncentives;


	// Map of SVCs agents
	SCVMap scvMap;
	MarineMap marines; //stores the marines owned
	MedicMap medics;
	FirebatMap firebats;
	GoliathMap goliaths;
	VultureMap vultures;

	//is a supply depot scheduled to be built?
	bool scheduledSupplyDepots;

	int academyCount; //keeps track of the number of academies
	int armoryCount;

	void _drawStats(); //writes information texts on screen and draws some useful figures
	void _drawExploredStats();

public:
	// Virtual functions for callbacks, leave these as they are.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit);
	
	
	// Everything below this line is safe to modify.
	ExampleAIModule();
	~ExampleAIModule();

	void revealHiddenUnits();

	void updateTasks();
	void updateRepair();
	void updateAttack();
	void updateBuildCommandCenter();
	void updateBuildBarracks();
	void updateBuildSupplyDepot();
	void updateExplore();
	int calculateBarracksFromCommandCenter(BWAPI::Unit cmdCenter);
	void createBarrackNearCommandCenter(BWAPI::Unit cmdCenter);
	int calculateFactoriesFromCommandCenter(BWAPI::Unit cmdCenter);
	void createFactoryNearCommandCenter(BWAPI::Unit cmdCenter);
	unordered_map<int, SCVAgent*>& getSCVMap();
	unordered_map<TaskType, vector<Task>*>& getTasks();

	// Vespene gas
	void updateBuildRefinery();
	void updateBuildAcademy();
	void updateBuildArmory();
	void updateBuildFactory();

	void updateResearchLongRange();
	void updateResearchStimPack();
	void updateResearchArmoryWeapon();
	void updateResearchArmoryPlating();
	void updateBuildBunker();
	int calculateBunkersFromCommandCenter(BWAPI::Unit cmdCenter);
	int generateRandomnteger(int nMin, int nMax);
	
	// Units
	void updateTrainSCV();
	void updateTrainMarine();
	void updateTrainFirebat();
	void updateTrainGoliath();
	void updateTrainVulture();
	void updateTrainMedic();

};
