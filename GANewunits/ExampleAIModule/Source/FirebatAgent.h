#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include <map>
#include "Task.h"
#include "AgentStates.h"

using namespace std;

#define MEETING_RADIUS TILE_SIZE * 10

class FirebatAgent {

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
	FirebatAgent(BWAPI::Unit u);
	~FirebatAgent(void);
	//static void setParameters(map<int, double>& params);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, vector<Task>*> tasks, unordered_map<int, FirebatAgent*> colleagues);
	bool evaluateIncentive();
	void attack(Position target, unordered_map<int, FirebatAgent*> colleagues);
	void attack(unordered_map<int, FirebatAgent*> colleagues);
	void doAttack(Position target);
	bool isOnAttack();
	Unit FirebatAgent::oldestColleagueAround();
	BWAPI::Unit gameUnit;

};

