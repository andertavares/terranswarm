#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include "Task.h"
#include "AgentStates.h"

using namespace std;

#define MEETING_RADIUS TILE_SIZE * 10

class MedicAgent {

	//bool engaged;
	int latencyFrames;
	int state;
	int unitId;
	int lastFrameCount;
	Position lastPosition;
	Position target;
	Position originPosition;
	int lastHealedId;
	//static int MAX_MAP_DISTANCE;

public:
	MedicAgent(BWAPI::Unit u);
	~MedicAgent(void);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, vector<Task>*> taskMap);
	void cureInPos();
	void updatePositionToCure();
	bool evaluateIncentive();
	bool allreadyFired(BWAPI::Unit enemy);

	bool isOnAttack();
	BWAPI::Unit gameUnit;

private:
	
};

