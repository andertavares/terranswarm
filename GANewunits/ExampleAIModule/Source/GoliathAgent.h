#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include <map>
#include "Task.h"
#include "AgentStates.h"

using namespace std;

#define MEETING_RADIUS TILE_SIZE * 10

class GoliathAgent {

	//parameters read from a file
	//static std::map<int, double>& parameters;

	//bool engaged;
	int latencyFrames;
	int state;
	int unitId;
	int lastFrameCount;
	Position lastPosition;
	Position target;
	//static int MAX_MAP_DISTANCE;

public:
	GoliathAgent(BWAPI::Unit u);
	~GoliathAgent(void);
	//static void setParameters(map<int, double>& params);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, vector<Task>*> tasks, unordered_map<int, GoliathAgent*> colleagues);
	bool evaluateIncentive();
	void attack(Position target, unordered_map<int, GoliathAgent*> colleagues);
	void attack(unordered_map<int, GoliathAgent*> colleagues);
	void doAttack(Position target);
	bool isOnAttack();
	Unit GoliathAgent::oldestColleagueAround();
	BWAPI::Unit gameUnit;

};

