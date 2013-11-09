#include "Task.h"

Task::Task(TaskType taskType, float incentive):type(taskType), incentive(incentive){}

void Task::setIncentive(float incentive){
	this->incentive = incentive;
}

float Task::getIncentive(){
	return incentive;
}

TaskType Task::getTaskType(){
	return type;
}