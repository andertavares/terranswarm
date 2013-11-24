#pragma once

#include <BWAPI.h>
#include <deque>
#include <unordered_map>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h> 
#include <random>
#include <iostream>
#include "SCVAgent.h"
#include "util.h"
#include "ExampleAIModule.h"
#include "TaskAssociation.h"

using namespace BWAPI;
using namespace Filter;
using namespace std;

//SCVAgent::stateNames[NO_TASK] = NO_TASK;

SCVAgent::SCVAgent(BWAPI::Unit scv){
	srand ( time(NULL) );
	gameUnit = scv;
	unitId = gameUnit->getID();
	lastPosition = Position(0,0);
	lastFrameCount = Broodwar->getFrameCount();
	state = NO_TASK;
	lastChecked = 0;
}


SCVAgent::~SCVAgent(void){

}

/**
  * Returns the Unit that this agent refers to in the game
  */
Unit SCVAgent::getUnit(){
	//Broodwar << "Returning id " << unitId << std::endl;
	return gameUnit;
}

void SCVAgent::onFrame(unordered_map<TaskType, vector<Task>*> *taskMap, Unitset theMinerals, Unitset commandCenters, unordered_map<int, SCVAgent*> scvMap){
	
	if(!gameUnit->isCompleted()){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nIncomplete");
		return;
	}

	if(state == EXPLORING){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nExploring");
		if(goScout()) return;
	}

	if(isBuildingExpansion()){
		buildCommandCenter(theMinerals, commandCenters);
		return;
	}

	if(gameUnit->isGatheringMinerals()){
		if(gameUnit->getOrder() == Orders::MiningMinerals){
			Broodwar->drawTextMap(gameUnit->getPosition(),"\nGathering");
			return;
		}
	}

	// Validate actions and status
	if(gameUnit->isConstructing() || gameUnit->isMoving()){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nConstructing or moving");
		return;
	}

	// Simple approach to local incentives
	if ( lastChecked + 75 < Broodwar->getFrameCount() || lastChecked == 0) {
		for(unordered_map<TaskType, vector<Task>*>::iterator iter = taskMap->begin(); iter != taskMap->end(); ++iter){
			TaskType taskType =  iter->first;
			vector<Task>* taskList = iter->second;

			if(taskType == GatherMinerals){
				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
					auto task = it;
					float rNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					TaskAssociation taskA = TaskAssociation(&(*task), 0.7f);

					if(taskA.tValue() > rNumber){
						state = GATHERING_MINERALS;
						Broodwar->drawTextMap(gameUnit->getPosition(),"\nGathering");
						lastChecked = Broodwar->getFrameCount();
						
						if ( gameUnit->isCarryingGas() || gameUnit->isCarryingMinerals() ) {
							gameUnit->returnCargo();
						}
						else if ( !gameUnit->getPowerUp() ) { 
							if ( !gameUnit->gather( gameUnit->getClosestUnit( IsMineralField || IsRefinery )) ) {
								Broodwar << Broodwar->getLastError() << std::endl;
							}
						}
						return;
					}
				}
			}
			else if(taskType == BuildSupplyDepot){
				//TODO: one supply at time
				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				//for (auto task = taskList->begin(); task != taskList->end(); task++){		
					auto task = it;
					float rNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					TaskAssociation taskA = TaskAssociation(&(*task), 0.5f);
					
					if(taskA.tValue() > rNumber){
						Broodwar << "Agent [" << unitId << "] Task suppy dep " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << std::endl;
						// Check if other SCV near by are constructing the same thing
						Unitset scvAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 20 * TILE_SIZE, Filter::IsWorker);
						int scvNearConstructingSCV = 0;
						for(Unitset::iterator scvIt = scvAround.begin(); scvIt != scvAround.end(); ++scvIt){
							if(scvIt->isConstructing() ){
								//&& scvIt->getBuildUnit() !=NULL
								//&& scvIt->getBuildUnit()->getType() == UnitTypes::Terran_Supply_Depot){
								scvNearConstructingSCV++;
							}
						}
					
						if(scvNearConstructingSCV <= 1){
							Broodwar->drawTextMap(gameUnit->getPosition(),"\nSupply Depot");
							lastChecked = Broodwar->getFrameCount();
							createSupply();
							it = taskList->erase(it);
							return;
						}
					}
				}
			}
			else if(taskType == BuildBarracks){
				// evaluate incentive
				//TODO: build barracks around new bases
				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
					auto task = it;
					float rNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					TaskAssociation taskA = TaskAssociation(&(*task), 0.5f);
					
					if(taskA.tValue() > rNumber){
						Broodwar << "Agent [" << unitId << "] Task barrack " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << std::endl;
						// Check if other SCV near by are constructing the same thing
						Unitset scvAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 50 * TILE_SIZE, Filter::IsWorker);
						int scvNearConstructingSCV = 0;
						for(Unitset::iterator scvIt = scvAround.begin(); scvIt != scvAround.end(); ++scvIt){
							if(scvIt->isConstructing() ){
								scvNearConstructingSCV++;
							}
						}
					
						if(scvNearConstructingSCV <= 1){
							Broodwar->drawTextMap(gameUnit->getPosition(),"\nBarrack");
							lastChecked = Broodwar->getFrameCount();
							createBarrackNearCommandCenter(it->getPosition());
							it = taskList->erase(it);
							return;
						}
					}
				}
			}
			else if(taskType == BuildCommandCenter){
				// Calculate how far is the new command center
				// Calculate how many minerals are near the command center
				// evaluate incentive
				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
					auto task = it;
					float rNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					TaskAssociation taskA = TaskAssociation(&(*task), 1.0f);

					if(taskA.tValue() > rNumber){

						bool isOtherExpanding = false;
						for(auto agent = scvMap.begin(); agent != scvMap.end(); agent++){
							if(agent->second->unitId != unitId && agent->second->isBuildingExpansion()){
								isOtherExpanding = true;
								break;
							}
						}

						if(!isOtherExpanding){
							Broodwar << "Agent [" << unitId << "] Task command " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << std::endl;
							lastChecked = Broodwar->getFrameCount();
							buildCommandCenter(theMinerals, commandCenters);
							it = taskList->erase(it);
							return;
						}
					}
				}
			}
			else if(taskType == Explore){
				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
					auto task = it;
					float rNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					TaskAssociation taskA = TaskAssociation(&(*task), 0.0f);

					if(taskA.tValue() > rNumber){
						
						bool isOthersScouting = false;
						for(auto agent = scvMap.begin(); agent != scvMap.end(); agent++){
							if(agent->second->unitId != unitId && agent->second->state == EXPLORING){
								isOthersScouting = true;
								break;
							}
						}

						if(!isOthersScouting){
							Broodwar << "Agent [" << unitId << "] Task explore " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << std::endl;
							state = EXPLORING;
							Broodwar->drawTextMap(gameUnit->getPosition(),"\nExplore");
							lastChecked = Broodwar->getFrameCount();
							goScout();
							it = taskList->erase(it);
							return;
						}
					}
				}
			}
		}

		// Default action
		if ( gameUnit->isIdle() ) {
			lastChecked = Broodwar->getFrameCount();
			//Broodwar->drawTextMap(gameUnit->getPosition(),"\nIdle");
			// Order workers carrying a resource to return them to the center,
			// otherwise find a mineral patch to harvest.
			if ( gameUnit->isCarryingGas() || gameUnit->isCarryingMinerals() ) {
				gameUnit->returnCargo();
			}
			else if ( !gameUnit->getPowerUp() ) { 
					// The worker cannot harvest anything if it
					// is carrying a powerup such as a flag
					// Harvest from the nearest mineral patch or gas refinery
				if ( !gameUnit->gather( gameUnit->getClosestUnit( IsMineralField || IsRefinery )) ) {
					// If the call fails, then print the last error message
					Broodwar << Broodwar->getLastError() << std::endl;
				}
			} // closure: has no powerup
		} // closure: if idle
	}
	
}

/**
  * Implements the decision of building a new command center (cmd).
  * The SCV chooses a location (near mineral field) to build the cmd, moves towards it and
  * places the new cmd close to the location
  */
void SCVAgent::buildCommandCenter(Unitset theMinerals, Unitset commandCenters){
	//Unitset uncoveredMinerals;
	
	if (state == MOVING_TO_NEW_BASE) {
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nMoving to base");
		Broodwar->drawLineMap(gameUnit->getPosition(),nearBaseArea,Color(Colors::Green));
		gameUnit->move(nearBaseArea);

		Region targetReg = Broodwar->getRegionAt(nearBaseArea);
		Region myReg = gameUnit->getRegion();

		//Broodwar->drawTextMap(gameUnit->getPosition(),"\nmyReg: %d, tgtReg: %d, d:%d", myReg->getID(), targetReg->getID(), gameUnit->getDistance(nearBaseArea));

		//sometimes regions are buggy, so we test if we are very close to the target point
		if(myReg->getID() == targetReg->getID() || (gameUnit->getDistance(nearBaseArea)/TILE_SIZE) <= 2){
		//}

		//if destination is in sight
		//if (gameUnit->getType().sightRange() > gameUnit->getDistance(nearBaseArea) ){
			//Broodwar->sendText("Arrived!");
			state = IN_BASE_AREA;
		}

	}
	
	else if (state == IN_BASE_AREA){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nIn base area");
		//must find suitable location to build base
		Broodwar->drawCircleMap(gameUnit->getPosition(), gameUnit->getType().sightRange(),Color(Colors::Cyan));
		UnitType centerType = gameUnit->getType().getRace().getCenter();
		TilePosition targetBuildLocation = Broodwar->getBuildLocation(centerType, gameUnit->getTilePosition() );

		//TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
		//TODO: insist in case of "something is in the way" error
		if (targetBuildLocation){

			// Register an event that draws the target build location
			Broodwar->registerEvent([targetBuildLocation,centerType](Game*)
			{
				Broodwar->drawBoxMap( Position(targetBuildLocation),
					Position(targetBuildLocation + centerType.tileSize()),
					Colors::Blue);
			},
			nullptr,  // condition
			centerType.buildTime() + 100 );  // frames to run

			//if ( targetBuildLocation ){

				// Order the builder to construct the supply structure
				gameUnit->build(centerType, targetBuildLocation);
				state = BUILDING_BASE;
			//}
		}
		
	}

	else if(state == BUILDING_BASE){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nBuilding Base");
		//when finished, clears all
		if(! gameUnit->isConstructing()){
			state = NO_TASK;// NOT_BUILDING_BASE;
		}
	}

	else {
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nelse");
		nearBaseArea = pointNearNewBase(theMinerals, commandCenters);
		//checks consistency of new assigned point
		if(nearBaseArea.x != -1 && nearBaseArea.y != -1) {
			//Broodwar->sendText("Tgt point inconsistent");
			Broodwar->drawTextMap(gameUnit->getPosition(),"\n\ninconsistent tgt");
			state = MOVING_TO_NEW_BASE;
		}
	}
}

bool SCVAgent::isBuildingExpansion(){
	return state == MOVING_TO_NEW_BASE || state == IN_BASE_AREA || state == BUILDING_BASE;
}

Position SCVAgent::pointNearNewBase(Unitset theMinerals, Unitset commandCenters){

	Unit closestMineral = NULL; //mineral to which the base will be built
	//closestMineralPosition = new Position(0,0);

	int minDistance = INT_MAX;

	//finds out which mineral is closest to the scv
	Unitset::iterator mineral;
	//bool allReachable = true;
	for (mineral = theMinerals.begin(); mineral != theMinerals.end(); ++mineral){
		bool reachable = false;
		for(Unitset::iterator cmd = commandCenters.begin(); cmd != commandCenters.end(); ++cmd){
			if (cmd->getDistance(mineral->getPosition()) < BASE_RADIUS){
				reachable = true;
				break;
			}
		}
		if (!reachable){
			//allReachable = false;
			if (gameUnit->getDistance(*mineral) < minDistance){
				minDistance = gameUnit->getDistance(*mineral);
				closestMineral = *mineral;
			}
		}
	}
	/*if(allReachable) {
		return Position(0,0);//returns dummy position if all minerals are in range...
	}*/
	if (closestMineral == NULL){
		//Broodwar->sendText("Dummy position returned");
		return Position(-1,-1);//returns dummy position if all minerals are in range...
	}
	return closestMineral->getPosition();

}



bool SCVAgent::goScout(){

	Position myPos = gameUnit->getPosition();
	Broodwar->drawLineMap(myPos, lastPosition, Colors::Blue);

	double square_dist = pow((lastPosition.x- myPos.x), 2.0) + pow((lastPosition.y - myPos.y), 2.0);
	if (lastPosition.x != 0 && lastPosition.y != 0 && square_dist >= pow(3*TILE_SIZE, 2.0)){
		gameUnit->move(lastPosition);
		return false;
	}

	int currentFrameCount = Broodwar->getFrameCount();
	if ( currentFrameCount >= lastFrameCount + 20){
		lastFrameCount = currentFrameCount;
		Position pos = getPositionToScout();
		//Broodwar << "Agent [" << unitId << "] Scouting to :" << pos << std::endl;

		gameUnit->move(pos);
		return true;
	}

	return false;
}

Position SCVAgent::getPositionToScout(){
	Position returnPosition;
	Unit unit = ((Unit) gameUnit);

	int radiousInPixels = 120*TILE_SIZE;
	std::deque<Position> positionListInRadious;
	std::deque<Position> positionListInMap;
	Position myPos = unit->getPosition();
	Region myRegion = Broodwar->getRegion( unit->getTilePosition() );

	TilePosition seedTilePos = TilePosition(myPos);
	
	int x      = seedTilePos.x;
	int y      = seedTilePos.y;
	int length = 1;
	int j      = 0;
	bool first = true;
	int dx     = 0;
	int dy     = 1;	

	while (length < Broodwar->mapWidth()) {
		returnPosition = Position(x*TILE_SIZE, y*TILE_SIZE);
		
		if (x >= 0 && x < Broodwar->mapWidth() 
			&& y >= 0 && y < Broodwar->mapHeight() 
			&& Broodwar->hasPath(myPos,returnPosition) 
			&& lastPosition.x != returnPosition.x 
			&& lastPosition.y != returnPosition.y
			&& !Broodwar->isExplored(x,y)
			&& Broodwar->isWalkable(x,y)) {
				double square_dist = pow((x- myPos.x), 2.0) + pow((y - myPos.y), 2.0);
				if (square_dist < pow(radiousInPixels, 2.0)){

					positionListInRadious.push_back(returnPosition);
					//lastPosition = returnPosition;
					//return returnPosition;
				}
				else{
					positionListInMap.push_back(returnPosition);
				}
		}

		//otherwise, move to another position
		x = x + dx;
		y = y + dy;
		//count how many steps we take in this direction
		j++;
		if (j == length) { //if we've reached the end, its time to turn
			j = 0;	//reset step counter

			//Spiral out. Keep going.
			if (!first)
				length++; //increment step counter if needed

			first =! first; //first=true for every other turn so we spiral out at the right rate

			//turn counter clockwise 90 degrees:
			if (dx == 0) {
				dx = dy;
				dy = 0;
			} else {
				dy = -dx;
				dx = 0;
			}
		}
		//Spiral out. Keep going.
	}

	if(positionListInRadious.size() > 0){
		int randomIndex = rand() % positionListInRadious.size();
		returnPosition = positionListInRadious[randomIndex];

		//Broodwar << "positionList [" << positionListInRadious.size() << "]" << std::endl;
		//Broodwar << "Agent [" << unitId << "] returning random option in Radious:" << returnPosition << std::endl;
		lastPosition = returnPosition;
	}
	else if(positionListInMap.size() > 0){
		int randomIndex = rand() % positionListInMap.size();
		returnPosition = positionListInMap[randomIndex];

		//Broodwar << "positionListInMap [" << positionListInMap.size() << "]" << std::endl;
		//Broodwar << "Agent [" << unitId << "] returning random option in Map :" << returnPosition << std::endl;
		lastPosition = returnPosition;
	}
	
	return returnPosition;
}

void SCVAgent::createSupply(){
	state = BUILDING_SUPPLY_DEPOT;
	UnitType supplyProviderType = gameUnit->getType().getRace().getSupplyProvider();
	if ( supplyProviderType.isBuilding() ){
		TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, gameUnit->getTilePosition());
		if ( targetBuildLocation ){
			// Register an event that draws the target build location
			Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*){
					Broodwar->drawBoxMap( Position(targetBuildLocation), Position(targetBuildLocation + supplyProviderType.tileSize()),	Colors::Yellow);
				},
				nullptr,  // condition
				supplyProviderType.buildTime() + 100 
			);  // frames to run

			// Order the builder to construct the supply structure
			gameUnit->build( supplyProviderType, targetBuildLocation );
		}
	}
}

void SCVAgent::createBarrackNearCommandCenter(Position commandCenterPos) {
	UnitType barrackType = UnitTypes::Terran_Barracks;

	TilePosition targetBuildLocation = Broodwar->getBuildLocation(barrackType, gameUnit->getTilePosition());
	if ( targetBuildLocation ){
		// Register an event that draws the target build location
		Broodwar->registerEvent([targetBuildLocation,barrackType](Game*)
			{
				Broodwar->drawBoxMap( Position(targetBuildLocation),
					Position(targetBuildLocation + barrackType.tileSize()),
					Colors::Blue);
			},
				nullptr,  // condition
			barrackType.buildTime() + 100 );  // frames to run

		// Order the builder to construct the supply structure
		gameUnit->build( barrackType, targetBuildLocation );
	}
}

	//only chooses another task if current is gathering minerals or doing nothing
	/*if(! (state == GATHERING_MINERALS || state == NO_TASK) ){
		Broodwar->drawTextMap(gameUnit->getPosition(),"%\nBusy %d", state);
		return;
	}*/

	vector<TaskAssociation> taskAssociations;
	//feasibleTasks.clear(); //initializes the task vector
	//vector<Task>::iterator fit = feasibleTasks.begin();
	
	//inserts all tasks that SCV can perform
	/*feasibleTasks.insert(feasibleTasks.begin(),taskMap[BuildBarracks]->begin(),taskMap[BuildBarracks]->end());
	feasibleTasks.insert(feasibleTasks.begin(),taskMap[BuildSupplyDepot]->begin(),taskMap[BuildSupplyDepot]->end());
	feasibleTasks.insert(feasibleTasks.begin(),taskMap[BuildCommandCenter]->begin(),taskMap[BuildCommandCenter]->end());
	feasibleTasks.insert(feasibleTasks.begin(),taskMap[Explore]->begin(),taskMap[Explore]->end());
	*/
	
	/*
	switch (state){
	case BUILDING_BARRACKS:
	case BUILDING_SUPPLY_DEPOT:
	case MOVING_TO_NEW_BASE:
		return;
		return
MOVING_TO_NEW_BASE, IN_BASE_AREA, BUILDING_BASE
	}*/

	/*
	//TODO: calculate the T values for feasible tasks...
	for(auto taskIter = taskMap.begin(); taskIter != taskMap.end(); ++taskIter){
		float capability = 0;

		switch(taskIter->first){
			case BuildBarracks:
			case BuildSupplyDepot:
			case BuildCommandCenter:
			case GatherMinerals:
				capability = 1;
				break;

			case Explore:
				capability = 0.3;
				break;
		} //closure: switch

		if (capability == 0){
			continue; //agent is not able to perform tasks of this type, go to next
		}
		for(auto task = taskIter->second->begin(); task != taskIter->second->end(); task++){
			taskAssociations.push_back(TaskAssociation(&(*task), capability));

			if(gameUnit->getID() == 1){
				Broodwar->sendText("TI: %.2f, K: %.2f",task->getIncentive(),capability);
			}
		}
	}
	
	// Debug
	if(gameUnit->getID() == 1){
		int offset = 0;
		//Broodwar->drawTextScreen(200,115,"%d items", feasibleTasks.size());
		for (auto ta = taskAssociations.begin(); ta != taskAssociations.end(); ta++){
			Broodwar->drawTextScreen(200,115+offset,"%d - %d", ta->task()->getTaskType(), ta->task()->getIncentive());
			offset += 15;
		}	
	}

	Task* toPerform = NULL;
	for(auto ta = taskAssociations.begin(); ta != taskAssociations.end(); ++ta){
		float random = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/1));
		if(ta->tValue() > random){
			toPerform = ta->task();
			break;
		}
	}

	if (toPerform == NULL || toPerform->getTaskType() == GatherMinerals){
		Broodwar->drawTextMap(gameUnit->getPosition(),"%\nGAT");
		if ( !gameUnit->gather( gameUnit->getClosestUnit( IsMineralField || IsRefinery )) ) {
			// If the call fails, then print the last error message
			Broodwar << Broodwar->getLastError() << std::endl;
		}
		state = GATHERING_MINERALS;
	}

	if(toPerform->getTaskType() == BuildSupplyDepot){
		Broodwar->drawTextMap(gameUnit->getPosition(),"%\nBS");
		state = BUILDING_SUPPLY_DEPOT;
		createSupply();
	}

	if(toPerform->getTaskType() == BuildBarracks){
		Broodwar->drawTextMap(gameUnit->getPosition(),"%\nBB");
		state = BUILDING_BARRACKS;
	}

	if(toPerform->getTaskType() == BuildCommandCenter){
		Broodwar->drawTextMap(gameUnit->getPosition(),"%\nBC");
		buildCommandCenter(theMinerals, commandCenters);
	}

	toPerform->setIncentive(0.0f);
	*/