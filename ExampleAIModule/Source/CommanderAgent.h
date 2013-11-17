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
	void onFrame(unordered_map<TaskType, vector<Task>*> tasklist);
	void createSupply(BWAPI::Unit u);
	int calculateBarracksFromCommandCenter(BWAPI::Unit u);
	void createBarrackNearCommandCenter(BWAPI::Unit u);

private:
	BWAPI::Unitset _commandCenters;
	BWAPI::Unitset _barracks;
};

