//#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include "Task.h"

using namespace std;

class MarineAgent {
public:
	MarineAgent(BWAPI::Unit* u);
	~MarineAgent(void);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	bool evaluateIncentive();
	BWAPI::Position getPositionToScout(BWAPI::Region* myRegion);

private:
	BWAPI::Unit* _marine;
};

