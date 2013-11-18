//#pragma once
#include <BWAPI.h>
#include <deque>
#include <unordered_map>
#include "Task.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h> 

using namespace std;

class SCVAgent {
	
public:
	SCVAgent(BWAPI::Unit scv);
	~SCVAgent(void);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	bool evaluateIncentive();
	BWAPI::Unit SCVAgent::getUnit();
	BWAPI::Position getPositionToScout();
	bool goScout();
	BWAPI::Unit gameUnit;
	int unitId;
	int lastFrameCount;

private:
	BWAPI::Position lastPosition;
	
};

