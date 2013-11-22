#pragma once

#include <BWAPI.h>
#include <deque>
#include <unordered_map>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h> 
#include <random>
#include <iostream>

using namespace std;

enum State {
	CREATED,
	NOT_BUILDING_BASE,
	MOVING_TO_NEW_BASE,
	IN_BASE_AREA,
	BUILDING_BASE,
	BUILDING_SUPPLY
};

class SCVAgent {
	State state;
	Position nearBaseArea; //will build a base near this position
	
public:
	SCVAgent(BWAPI::Unit scv);
	~SCVAgent(void);
	
	int unitId;
	int lastFrameCount;
	BWAPI::Unit gameUnit;

	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, list<Task>*> taskMap, Unitset theMinerals, Unitset commandCenters);
	bool evaluateIncentive();
	BWAPI::Unit SCVAgent::getUnit();
	BWAPI::Position getPositionToScout();
	bool goScout();
	void buildCommandCenter(Unitset theMinerals, Unitset commandCenters);
	Position pointNearNewBase(Unitset theMinerals, Unitset commandCenters);
	void createSupply();
	

private:
	BWAPI::Position lastPosition;
	
};

