//#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include "Task.h"

using namespace std;

class MarineAgent {
	
	bool engaged;

public:
	MarineAgent(BWAPI::Unit u);
	~MarineAgent(void);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, vector<Task>*> tasks);
	bool evaluateIncentive();
	BWAPI::Position getPositionToScout();
	BWAPI::Unit gameUnit;

private:
	
};

