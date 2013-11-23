#include <vector>
#include <algorithm>
#include <cstdlib>
#include <BWAPI.h>
#include "TaskAssociation.h"

using namespace std;

/**
  * Selects a Task from the list with chance in proportion to its incentive value
  */
Task* weightedSelection(vector<TaskAssociation>& theTasks){
	sort(theTasks.begin(), theTasks.end());

	float sum = 0.f;

	for(auto ta = theTasks.begin(); ta != theTasks.end(); ++ta){
		sum += ta->tValue();
	}

	//generates a pseudo-random number between 0 and sum
	float random = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/sum));
	
	//traverses the list until it finds the task that matches the random number
	float acc = 0;
	for(auto ta = theTasks.begin(); ta != theTasks.end(); ++ta){
		if(acc + ta->tValue() < random){
			return ta->task(); 
		}
	}
	return NULL;
	//Broodwar->sendText("Weighted selection error! %d items; %f sum", theTasks.size(), sum);
}

/**
  * Returns whether task a has a lower incentive than task b
  *
bool lowerIncentive(Task& a, Task& b){
}*/