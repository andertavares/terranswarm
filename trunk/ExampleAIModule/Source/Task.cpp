#include "Task.h"
using namespace BWAPI;

/**
  * this constructor initializes position to 0,0...
  */
Task::Task(TaskType taskType, float incentive) : type(taskType), incentive(incentive){
	position = *(new Position(0,0));
}

Task::Task(TaskType taskType, float incentive, Position pos) : type(taskType), incentive(incentive), position(pos){

}


void Task::setIncentive(float incentive){
	this->incentive = incentive;
}

float Task::getIncentive(){
	return incentive;
}

TaskType Task::getTaskType(){
	return type;
}

Position Task::getPosition(){
	return position;
}
