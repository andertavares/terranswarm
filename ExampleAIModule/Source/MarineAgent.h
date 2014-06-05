#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include "Task.h"
#include "AgentStates.h"

using namespace std;

#define MEETING_RADIUS TILE_SIZE * 10

class MarineAgent {

	//bool engaged;
	int latencyFrames;
	int state;
	int unitId;
	int lastFrameCount;
	Position lastPosition;
	Position target;
	//static int MAX_MAP_DISTANCE;

public:
	MarineAgent(BWAPI::Unit u);
	~MarineAgent(void);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, vector<Task>*> tasks, unordered_map<int, MarineAgent*> colleagues);
	bool evaluateIncentive();
	BWAPI::Position getPositionToScout();
	bool goScout();
	void attack(Position target, unordered_map<int, MarineAgent*> colleagues);
	void attack(unordered_map<int, MarineAgent*> colleagues);
	void doAttack(Position target);
	bool isOnAttack();
	Unit MarineAgent::oldestColleagueAround();
	BWAPI::Unit gameUnit;

private:
	
};

