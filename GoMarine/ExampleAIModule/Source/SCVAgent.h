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
#include "ExampleAIModule.h"

using namespace std;

#define EULER 2.71828182845904523536

class ExampleAIModule; //forward decl. to prevent compile error

class SCVAgent {
	
	Position nearBaseArea; //will build a base near this position
	ExampleAIModule* _aiModule;
	int latencyFrames; //I should not act every frame
	
	
	
public:
	SCVAgent(BWAPI::Unit scv, ExampleAIModule* aiModule);
	~SCVAgent(void);
	
	int unitId;
	int lastFrameCount;
	Unit gameUnit;
	Unit repairTarget;
	Position atkTarget;
	Position originPosition;
	Unit toAttack;
	TilePosition newBuildingLocation;

	//void onTask(unordered_map<TaskType, vector<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, vector<Task>*> *taskMap, vector<Position> mineralPositions, Unitset commandCenters, unordered_map<int, SCVAgent*> scvMap);
	bool evaluateIncentive();
	bool isDangerous(Position pos);
	BWAPI::Unit SCVAgent::getUnit();
	BWAPI::Position getPositionToScout();

	void attack(Position theTarget);
	void attack();

	bool goScout();
	void goRepair(Position dmgUnitPos);
	void goRepair();
	void buildCommandCenter(vector<Position> theMinerals, Unitset commandCenters);
	Position pointNearNewBase(vector<Position> theMinerals, Unitset commandCenters);
	void createSupply();
	void createBarrackNearCommandCenter(Position commandCenterPos);
	bool isBuildingExpansion();
	bool isRepairing();
	bool shouldBeRepaired(Unit target);
	State state;
	

private:
	BWAPI::Position lastPosition;
	int lastChecked;
	
};

