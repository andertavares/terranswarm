#pragma once
#include <vector>
#include <BWAPI.h>
#include "TaskAssociation.h"

using namespace BWAPI;

Task* weightedSelection(std::vector<TaskAssociation>& theTasks);

Position getPositionToScout();
void goScout();
