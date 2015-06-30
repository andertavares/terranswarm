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
	GuardBase,
	BuildVespeneGas,
	BuildAcademy,
	TrainFirebat,
	TrainMedic,
	ResearchAcademyLongRange,
	ResearchAcademyStimPack,
	BuildBunker,
	BuildComSat //this was making marines go explore weird places instead of attacking, why???
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
