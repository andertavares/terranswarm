//#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include "Task.h"

using namespace std;

class SCVAgent {
	
public:
	SCVAgent(BWAPI::Unit scv);
	~SCVAgent(void);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	bool evaluateIncentive();
	BWAPI::Unit SCVAgent::getUnit();
	BWAPI::Position getPositionToScout();
	BWAPI::Unit gameUnit;
	int unitId;
};

