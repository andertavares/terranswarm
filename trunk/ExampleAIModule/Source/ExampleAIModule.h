//#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include "Task.h"
#include "CommanderAgent.h"

using namespace std;

// Remember not to use "Broodwar" in any global class constructor!

class ExampleAIModule : public BWAPI::AIModule
{

	//tasks that will have a single instance
	Task* gatherMinerals;
	Task* trainMarine;
	Task* buildSupplyDepot;
	Task* explore;
	CommanderAgent* _commanderAgent;

	//map: taskType to list<task> for tasks that may have multiple instances with variable incentives
	unordered_map<TaskType, list<Task>*> otherTasks;

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
	void updateBuildSupplyDepot();
	void updateExplore();
};
