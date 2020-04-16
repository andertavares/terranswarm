#pragma once
#include <BWAPI.h>
#include <unordered_map>
#include <vector>
#include "Task.h"
using namespace std;

class CommanderAgent{
	//commander agent will act every X frames:
	int latencyFrames;
public:
	CommanderAgent(void);
	~CommanderAgent(void);
	void onFrame(unordered_map<TaskType, vector<Task>*> tasklist, unordered_map<Unit, float> trainSCVIncentives);
	void createSupply(BWAPI::Unit u);
	int calculateBarracksFromCommandCenter(BWAPI::Unit u);
	void createBarrackNearCommandCenter(BWAPI::Unit u);
	int calculateFactoryFromCommandCenter(BWAPI::Unit u);
	void createFactoryNearCommandCenter(BWAPI::Unit u);
	void researchRequest(BWAPI::TechType techType);
	void researchRequest(BWAPI::UpgradeType upgdType);

private:
	BWAPI::Unitset _commandCenters;
	BWAPI::Unitset _barracks;
	BWAPI::Unitset _factories;
};

