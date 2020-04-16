#pragma once
#include <BWAPI.h>
#include <list>
#include <unordered_map>
#include <map>
#include "Task.h"
#include "AgentStates.h"

using namespace std;

#define MEETING_RADIUS TILE_SIZE * 10

class VultureAgent {

	//parameters read from a file
	//static std::map<int, double>& parameters;

	//bool engaged;
	int latencyFrames;
	int state;
	int unitId;
	int lastFrameCount;
	Position lastPosition;
	Position target;
	Unit bunkerToMove;
	//static int MAX_MAP_DISTANCE;

public:
	VultureAgent(BWAPI::Unit u);
	~VultureAgent(void);
	//static void setParameters(map<int, double>& params);
	void onTask(unordered_map<TaskType, list<Task>*> taskMap);
	void onFrame(unordered_map<TaskType, vector<Task>*> tasks, unordered_map<int, VultureAgent*> colleagues);
	bool evaluateIncentive();
	BWAPI::Position getPositionToScout();
	bool goScout();
	void attack(Position target, unordered_map<int, VultureAgent*> colleagues);
	void attack(unordered_map<int, VultureAgent*> colleagues);
	void doAttack(Position target);
	bool isOnAttack();
	Unit VultureAgent::oldestColleagueAround();
	BWAPI::Unit gameUnit;

};

