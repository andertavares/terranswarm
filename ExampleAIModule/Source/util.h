#pragma once
#include <vector>
#include <BWAPI.h>
#include "TaskAssociation.h"

using namespace BWAPI;

Task* weightedSelection(std::vector<TaskAssociation>& theTasks);
const string currentDateTime();


Position getPositionToScout();
void goScout();
