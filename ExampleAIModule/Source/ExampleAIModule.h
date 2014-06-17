#pragma once

#include <BWAPI.h>
#include <vector>
#include <unordered_map>
#include <list>
#include "Parameters.h"
#include "Task.h"
#include "CommanderAgent.h"
#include "MarineAgent.h"
#include "SCVAgent.h"
#include "MedicAgent.h"


#define BASE_RADIUS 20 * TILE_SIZE

using namespace std;
using namespace BWAPI;

class SCVAgent; //forward decl. to prevent compile error

typedef unordered_map<int, SCVAgent*> SCVMap;
typedef unordered_map<int, MarineAgent*> MarineMap;
typedef unordered_map<int, MedicAgent*> MedicMap;

// Remember not to use "Broodwar" in any global class constructor!

class ExampleAIModule : public BWAPI::AIModule {

	//relative path where the chromosome files are located
	string workingDir;

	//parameters that are used here and will be evolved by the GA
	map<int, double> parameters;

	//tasks that will have a single instance
	Task* gatherMinerals;
	Task* trainMarine;
	Task* buildSupplyDepot;
	Task* explore;
	Task* buildCommandCenter;
	Task* buildVespeneGas;
	Task* buildAcademy;
	Task* trainMedic;
	Task* researchAcademyStimpack;
	Task* researchAcademyLongRange;
	Task* researchAcademyOpticalFlare;
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

	// Map of SVCs agents
	SCVMap scvMap;
	MarineMap marines; //stores the marines owned
	MedicMap medics;

	//is a supply depot scheduled to be built?
	bool scheduledSupplyDepots;

	int academyCount; //keeps track of the number of academies

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
	void updateTrainSCV();
	void updateTrainMarine();
	void updateBuildCommandCenter();
	void updateBuildBarracks();
	void updateBuildSupplyDepot();
	void updateExplore();
	int calculateBarracksFromCommandCenter(BWAPI::Unit cmdCenter);
	void createBarrackNearCommandCenter(BWAPI::Unit cmdCenter);
	unordered_map<int, SCVAgent*>& getSCVMap();
	unordered_map<TaskType, vector<Task>*>& getTasks();

	// Vespene gas
	void updateBuildRefinery();
	void updateBuildAcademy();
	void updateTrainMedic();

	void updateResearchLongRange();
	void updateResearchStimPack();
	void updateBuildBunker();
	int calculateBunkersFromCommandCenter(BWAPI::Unit cmdCenter);
	int generateRandomnteger(int nMin, int nMax);
	
};
