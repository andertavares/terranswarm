#pragma once
#include <BWAPI.h>

using namespace BWAPI;

enum TaskType{
	TrainMarine,
	TrainWorker,
	BuildSupplyDepot,
	BuildBarracks,
	BuildCommandCenter,
	Repair,
	GatherMinerals,
	Explore,
	Attack,
	GuardBase
};


class Task {
	float incentive; //s_j in SwarmGAP description, the incentive a task produces
	TaskType type;
	Position position;

public:
	Task(TaskType taskType, float incentive);
	Task(TaskType taskType, float incentive, Position pos);
	float getIncentive();
	void setIncentive(float incentive);
	TaskType getTaskType();
	bool operator<(Task& b);

	Position getPosition();
};
