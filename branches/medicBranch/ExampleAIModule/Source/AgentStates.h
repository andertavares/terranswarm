#pragma once
#include <map>

enum State { //7,4,2
	NO_TASK,
	MOVING_TO_NEW_BASE,
	IN_BASE_AREA,
	BUILDING_BASE,
	BUILDING_SUPPLY_DEPOT,
	BUILDING_BARRACKS,
	GATHERING_MINERALS,
	REPAIRING,
	EXPLORING,
	PACKING,
	ATTACKING,
	CURE_MARINE
};

//std::map<State, std::string> stateNames; //do not use, initialization is not working =/


	//NOT_BUILDING_BASE,
	