//#pragma once
#include <BWAPI.h>
#include <vector>
#include <unordered_map>
#include "Task.h"
#include "CommanderAgent.h"

using namespace std;
using namespace BWAPI;
// Remember not to use "Broodwar" in any global class constructor!

class ExampleAIModule : public BWAPI::AIModule
{

	//tasks that will have a single instance
	Task* gatherMinerals;
	Task* trainMarine;
	Task* buildSupplyDepot;
	Task* explore;
	Task* buildCommandCenter;
	CommanderAgent* _commanderAgent;

	//number of minerals found out of base range
	//int mineralsOutOfBaseRange;

	//set of command centers
	Unitset commandCenters;

	//set of minerals that were discovered
	Unitset discoveredMinerals;

	//map: taskType to list<task> for tasks that may have multiple instances with variable incentives
	unordered_map<TaskType, vector<Task>*> allTasks;

	//keeps track of the barracks that were scheduled to be built around a command center
	unordered_map<Unit, int> scheduledBarracks;

	//keeps track of the barracks built around the command centers
	unordered_map<Unit, int> builtBarracks;

	//incentive to build barracks around the command centers
	unordered_map<Unit, float> buildBarracksIncentives;


	//is a supply depot scheduled to be built?
	bool scheduledSupplyDepots;
	

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

	void updateTasks();
	void updateBuildCommandCenter();
	void updateBuildBarracks();
	void updateBuildSupplyDepot();
	void updateExplore();
	int calculateBarracksFromCommandCenter(BWAPI::Unit cmdCenter);
	void createBarrackNearCommandCenter(BWAPI::Unit cmdCenter);
};
