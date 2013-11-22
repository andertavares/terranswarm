#include "Task.h"
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;

/**
  * Selects a Task from the list with chance in proportion to its incentive value
  */
Task* weightedSelection(vector<Task>* theTasks){
	sort(theTasks->begin(), theTasks->end());

	float sum = 0.f;

	for(auto task = theTasks->begin(); task != theTasks->end(); ++task){
		sum += task->getIncentive();
	}

	//generates a pseudo-random number between 0 and sum
	float random = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/sum));
	
	//traverses the list until it finds the task that matches the random number
	float acc = 0;
	for(auto task = theTasks->begin(); task != theTasks->end(); ++task){
		if(acc + task->getIncentive() > random){
			return &(*task); //hope this returns a valid Task*
		}
	}
}

/**
  * Returns whether task a has a lower incentive than task b
  *
bool lowerIncentive(Task& a, Task& b){
}*/