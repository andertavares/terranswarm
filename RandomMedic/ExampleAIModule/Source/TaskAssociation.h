#pragma once
#include "Task.h"

class TaskAssociation {
	Task* _task;
	float _tValue;
	float _agentCapability;

public:
	TaskAssociation(Task* theTask, float agentCapability);
	~TaskAssociation(void);

	float tValue();

	bool operator<(TaskAssociation& b);
	Task* task();
};

