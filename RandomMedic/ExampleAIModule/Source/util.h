#pragma once
#include <vector>
#include <BWAPI.h>
#include "TaskAssociation.h"

using namespace BWAPI;

Task* weightedSelection(std::vector<TaskAssociation>& theTasks);
int randomInRange(int min, int max);

Position getPositionToScout();
void goScout();
