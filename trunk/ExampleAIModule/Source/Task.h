#pragma once

enum TaskType{
	TrainMarine,
	TrainWorker,
	BuildSupplyDepot,
	BuildBarracks,
	BuildCommandCenter,
	Fix,
	GatherMinerals,
	Explore,
	Attack,
	GuardBase
};


class Task {
	float incentive; //s_j in SwarmGAP description, the incentive a task produces
	TaskType type;

public:
	Task(TaskType taskType, float incentive);
	float getIncentive();
	void setIncentive(float incentive);
	TaskType getTaskType();
};
