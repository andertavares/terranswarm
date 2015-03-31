#include <cmath>
#include "TaskAssociation.h"


TaskAssociation::TaskAssociation(Task* theTask, float agentCapability) : _task(theTask), _agentCapability(agentCapability) 
{
	float responseLimiar = 1 - _agentCapability;
	float incent = _task->getIncentive();
	_tValue = pow(incent,2) / (pow(incent,2) + pow(responseLimiar,2));
}


TaskAssociation::~TaskAssociation(void)
{
}

float TaskAssociation::tValue(){
	return _tValue;
}

bool TaskAssociation::operator<(TaskAssociation& b){
	return _tValue < b.tValue();
}

Task* TaskAssociation::task(){
	return _task;
}