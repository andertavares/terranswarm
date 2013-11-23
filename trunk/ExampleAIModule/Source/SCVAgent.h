#pragma once

#include <BWAPI.h>
#include <deque>
#include <map>
#include <unordered_map>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h> 
#include <random>
#include <iostream>
#include "Task.h"
#include "AgentStates.h"

using namespace std;

//map<State, std::string> stateNames;

class SCVAgent {
	
	Position nearBaseArea; //will build a base near this position
	
public:
	SCVAgent(BWAPI::Unit scv);
	~SCVAgent(void);
	
	int unitId;
	int lastFrameCount;
	BWAPI::Unit gameUnit;

	//void onTask(unordered_map<TaskType, vector<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, vector<Task>*> *taskMap, Unitset theMinerals, Unitset commandCenters, unordered_map<int, SCVAgent*> scvMap);
	bool evaluateIncentive();
	BWAPI::Unit SCVAgent::getUnit();
	BWAPI::Position getPositionToScout();
	bool goScout();
	void buildCommandCenter(Unitset theMinerals, Unitset commandCenters);
	Position pointNearNewBase(Unitset theMinerals, Unitset commandCenters);
	void createSupply();
	void createBarrackNearCommandCenter(Position commandCenterPos);
	State state;
	

private:
	BWAPI::Position lastPosition;
	int lastChecked;
	
};

