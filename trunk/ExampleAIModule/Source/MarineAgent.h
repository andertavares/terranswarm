#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include "Task.h"
#include "AgentStates.h"

using namespace std;

class MarineAgent {

	//bool engaged;
	int state;
	int unitId;
	int lastFrameCount;
	Position lastPosition;
	//static int MAX_MAP_DISTANCE;

public:
	MarineAgent(BWAPI::Unit u);
	~MarineAgent(void);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, vector<Task>*> tasks);
	bool evaluateIncentive();
	BWAPI::Position getPositionToScout();
	bool goScout();

	BWAPI::Unit gameUnit;

private:
	
};

