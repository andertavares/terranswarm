#include <map>
#include "AgentStates.h"

using namespace std;

extern map<State, std::string> stateNames;

//stateNames[NO_TASK] = "NO_TASK"; <-- this does not work =/

	/*NO_TASK,
	MOVING_TO_NEW_BASE,
	IN_BASE_AREA,
	BUILDING_BASE,
	BUILDING_SUPPLY_DEPOT,
	BUILDING_BARRACKS,
	GATHERING_MINERALS,
	REPAIRING,
	EXPLORING,
	PACKING,
	ATTACKING*/