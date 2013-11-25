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
#include <set>

using namespace BWAPI;
using namespace Filter;
using namespace std;

//SCVAgent::stateNames[NO_TASK] = NO_TASK;

SCVAgent::SCVAgent(Unit scv, ExampleAIModule* aiModule) : _aiModule(aiModule), repairTarget(NULL), atkTarget(NULL), toAttack(NULL){
	srand ( time(NULL) );
	gameUnit = scv;
	unitId = gameUnit->getID();
	originPosition = Position(0,0);
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
	// Get our starting location
	if (originPosition.x == 0 && originPosition.y == 0) {
		originPosition = gameUnit->getPosition();
	}

	Broodwar->drawTextMap(gameUnit->getPosition(),"\n\n%d-%d-%d-%d", state, gameUnit->isGatheringMinerals(), gameUnit->isConstructing(), gameUnit->getOrder());
	//Broodwar->drawCircleMap(gameUnit->getPosition(),gameUnit->getType().sightRange(),Color(Colors::Blue));
	if(!gameUnit->isCompleted()){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nIncomplete");
		return;
	}

	if(state == EXPLORING){
		// Check if are enemies near by and RUN!
		Unitset foes = Broodwar->getUnitsInRadius(gameUnit->getPosition(), TILE_SIZE * 4, Filter::IsEnemy);
		if(foes.size() > 1){
			gameUnit->move(getPositionToScout());
			state = NO_TASK;
		}
		else{
			if(goScout()){
				Broodwar->drawTextMap(gameUnit->getPosition(),"\nExploring");
				return;
			}
			else {
				gameUnit->move(originPosition);
				state == NO_TASK;
			}
		}
	}

	if(state == ATTACKING){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nATK");
		attack();
		return;
	}

	if(isBuildingExpansion()){
		buildCommandCenter(theMinerals, commandCenters);
		return;
	}

	if(isRepairing()){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nRepairing");
		goRepair();
		return;
	}

	if(gameUnit->isGatheringMinerals() && !gameUnit->isIdle()){
		if(gameUnit->getOrder() == Orders::MiningMinerals){
			Broodwar->drawTextMap(gameUnit->getPosition(),"\nGathering");
			return;
		}
	}

	// Validate actions and status
	if(gameUnit->isConstructing() ){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nConstructing");
		return;
	}
	if(gameUnit->isMoving()){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nMoving");
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

			else if(taskType == Repair){
				int maxDistance = Position(0,0).getDistance(Position(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE));

				for(auto repair = taskList->begin(); repair != taskList->end(); repair++){
					float rNumber = rand() / float(RAND_MAX);
					int dist = gameUnit->getPosition().getApproxDistance(repair->getPosition());
					float diff = 1.0f - gameUnit->getPosition().getApproxDistance(repair->getPosition()) / float(maxDistance);
					TaskAssociation taskA = TaskAssociation(&(*repair), diff);// pow(float(EULER),-dist));
					Broodwar->sendText("Rpr: s:%.3f, T:%.3f, k:%.3f", repair->getIncentive(), taskA.tValue(), diff);// pow(float(EULER),-diff));
					if(rNumber < taskA.tValue()){
						lastChecked = Broodwar->getFrameCount();
						//counts the number of repairers
						
						Broodwar->sendText("Will repair");
						goRepair(repair->getPosition());
						taskList->erase(repair);
						return;
					}
				}
			}

			else if (taskType == Attack){
				int maxDistance = Position(0,0).getApproxDistance(Position(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE));
				for (auto atk = taskList->begin(); atk != taskList->end(); atk++){
					//will not attack outside my view range
					if(atk->getPosition().getApproxDistance(gameUnit->getPosition()) > gameUnit->getType().sightRange()){
						continue; 
					}

					float capability = 1.0f - (gameUnit->getPosition().getApproxDistance(atk->getPosition()) / float(maxDistance));

					if( (rand() / RAND_MAX) < TaskAssociation(&(*atk), capability).tValue()){
						attack(atk->getPosition());
						lastChecked = Broodwar->getFrameCount();
						return;
						//state = ATTACKING;
					}
				
					//taskAssociations.push_back(TaskAssociation(&(*atk), capability));
				}
			}

			else if(taskType == BuildSupplyDepot){
				
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
				//calculates max map distance
				int maxDistance = Position(0,0).getApproxDistance(Position(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE));

				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
					auto task = it;
					float rNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					//distfactor ranges [0:0.5]: when it is 10, agent capability to build cmd center is minimum
					float distFactor = .5f * (gameUnit->getPosition().getApproxDistance(task->getPosition()) / float(maxDistance));

					//divides distfactor
					TaskAssociation taskA = TaskAssociation(&(*task), pow(float(EULER),-distFactor));
					
					if(taskA.tValue() > rNumber){
						Broodwar << "Agent [" << unitId << "] Task barrack " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << std::endl;
						// Check if other SCV near by are constructing the same thing
						Unitset scvAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 20 * TILE_SIZE, Filter::IsWorker);
						int scvNearConstructingSCV = 0;
						/*
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
						*/
						createBarrackNearCommandCenter(it->getPosition());
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

						int isOtherExpanding = 0;
						for(auto agent = scvMap.begin(); agent != scvMap.end(); agent++){
							if(agent->second->unitId != unitId && agent->second->isBuildingExpansion()){
								isOtherExpanding++;
							}
						}

						if(isOtherExpanding  <= 0){
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
	}

	// Default action
	if ( gameUnit->isIdle() && !isBuildingExpansion()) {
		//lastChecked = Broodwar->getFrameCount();
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

void SCVAgent::attack(Position theTarget){
	state = ATTACKING; 
	atkTarget = theTarget;
	attack();
}

void SCVAgent::attack(){
	state = ATTACKING; 
	//gameUnit->attack(atkTarget);

	if(toAttack == NULL || !toAttack->isVisible() || !toAttack->exists()){
		Broodwar->drawTextScreen(gameUnit->getPosition(), "\n\nNEW_TGT");
		Unit closestFoe = Broodwar->getClosestUnit(gameUnit->getPosition(), Filter::IsEnemy && !Filter::IsBuilding);

		if(!closestFoe) return; //sanity check
		
		//discards task if foe is far
		if(gameUnit->getPosition().getApproxDistance(closestFoe->getPosition()) > gameUnit->getType().sightRange()){
			state = NO_TASK;
			toAttack = NULL;
			Broodwar->drawTextMap(gameUnit->getPosition(),"\n\nNO_TGT");
			return;
		}
		else {
			toAttack = closestFoe;
			gameUnit->attack(toAttack);
		}
	}

	
	else{
		//if()
		gameUnit->attack(toAttack);
		Broodwar->drawLineMap(gameUnit->getPosition(), toAttack->getPosition(), Color(Colors::Red));
	}

	
}

void SCVAgent::goRepair(Position dmgUnitPos){
	TilePosition whereIsUnit = TilePosition(dmgUnitPos.x/TILE_SIZE, dmgUnitPos.y / TILE_SIZE);
	Unitset unitsThere = Broodwar->getUnitsOnTile(whereIsUnit);
	if(unitsThere.size() == 1 && unitsThere[0]->getType().isBuilding()){
		repairTarget = unitsThere[0];
		goRepair();
	}
}

void SCVAgent::goRepair(){
	
	
	//checks if other SCV assumed the task
	/*Unitset scvsAround = Broodwar->getUnitsInRadius(repairTarget->getPosition(), gameUnit->getType().sightRange(), IsWorker && IsOwned);
	for(auto other = scvsAround.begin(); other != scvsAround.end(); other++){
		if(other->isRepairing()){
			repairers++;
		}
	}*/

	unordered_map<int, SCVAgent*>& scvMap = _aiModule->getSCVMap();

	bool anotherRepairer = false;
	for(auto scv = scvMap.begin(); scv != scvMap.end(); scv++){
		if(scv->second == this) continue;

		if(scv->second->isRepairing() && scv->second->repairTarget == repairTarget){
			anotherRepairer = true;
			Broodwar->drawTextMap(gameUnit->getPosition(),  "\n\n\nanother repairing");
			break;
		}
		
	}

	
	//if target has more than 80% of energy or another SCV is reparing target or cannot repair, returns to no_task state
	if(repairTarget->getHitPoints() >= .8f * repairTarget->getType().maxHitPoints() ||  anotherRepairer){// || !gameUnit->repair(repairTarget)) {
		state = NO_TASK;
		repairTarget = NULL;
		gameUnit->stop();
		Broodwar->drawTextMap(gameUnit->getPosition(),  "\n\n\nWILL NOT REPAIR");
	}
	else {
		state = REPAIRING;
		gameUnit->repair(repairTarget);
		Broodwar->drawTextMap(gameUnit->getPosition(),  "\n\n\nI AM repairing");
		Broodwar->drawLineMap(gameUnit->getPosition(),repairTarget->getPosition(), Color(Colors::Blue));
	}

	
}

bool SCVAgent::isRepairing(){
	return (gameUnit->isRepairing() || state == REPAIRING) && repairTarget != NULL;
}
/**
  * Implements the decision of building a new command center (cmd).
  * The SCV chooses a location (near mineral field) to build the cmd, moves towards it and
  * places the new cmd close to the location
  */
void SCVAgent::buildCommandCenter(Unitset theMinerals, Unitset commandCenters){
	//Unitset uncoveredMinerals;
	
	Position pos = gameUnit->getPosition();
			Broodwar->registerEvent([pos](Game*)
			{
				Broodwar->drawCircleMap(pos,20,Color(Colors::Red));
			},
				nullptr,  // condition
			100 );  // frames to run

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
		} else {
			Position txtPlace =  gameUnit->getPosition();
			Broodwar->registerEvent([txtPlace, centerType](Game*)
			{
				Broodwar->drawTextMap( txtPlace,
					 Broodwar->getLastError().c_str());
			},
			nullptr,  // condition
			centerType.buildTime() + 100 );
			//Broodwar << Broodwar->getLastError() << std::endl;
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
			Broodwar->sendText("Tgt point inconsistent");
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
	int currentFrameCount = Broodwar->getFrameCount();

	double square_dist = pow((lastPosition.x- myPos.x), 2.0) + pow((lastPosition.y - myPos.y), 2.0);
	if (lastPosition.x != 0 && lastPosition.y != 0 && square_dist >= pow(3*TILE_SIZE, 2.0)){
		//Broodwar << "Agent [" << unitId << "] SCOUT " <<  currentFrameCount - (lastFrameCount + (35 * 24)) << std::endl;

		// If the unit is stuck or takes too long to reach targets (1 min aprox.) return false	
		if(!gameUnit->isStuck() && currentFrameCount < lastFrameCount + (65 * 24)){
			gameUnit->move(lastPosition);
			return true;
		}
		else{
			gameUnit->move(originPosition);
			Broodwar << "Agent [" << unitId << "] Takes too long to scout, removing this task" << std::endl;
			state = NO_TASK;
			return false;
		}
	}

	if ( currentFrameCount >= lastFrameCount + 20){
		lastFrameCount = currentFrameCount;
		// If this SCV is close to enemies, go to the closest barrack
		
		//else{
			Position pos = getPositionToScout();
			//Broodwar << "Agent [" << unitId << "] Scouting to :" << pos << std::endl;
			gameUnit->move(pos);
			return true;
		//}

	}

	return false;
}

Position SCVAgent::getPositionToScout(){
	Position returnPosition = originPosition;
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
		else {
			Position txtPlace =  gameUnit->getPosition();
			Broodwar->registerEvent([txtPlace,supplyProviderType](Game*)
			{
				Broodwar->drawTextMap( txtPlace,
					 Broodwar->getLastError().c_str());
			},
			nullptr,  // condition
			supplyProviderType.buildTime() + 100 );
			//Broodwar << Broodwar->getLastError() << std::endl;
		}
	}
}

void SCVAgent::createBarrackNearCommandCenter(Position commandCenterPos) {
	UnitType barrackType = UnitTypes::Terran_Barracks;
	state = BUILDING_BARRACKS;
	TilePosition cmdTilePos = TilePosition(commandCenterPos.x / TILE_SIZE, commandCenterPos.y / TILE_SIZE);
	//tries to place barrack near the command center
	TilePosition targetBuildLocation = Broodwar->getBuildLocation(barrackType, cmdTilePos);//gameUnit->getTilePosition());
	if ( targetBuildLocation ){
		Broodwar->drawLineMap(gameUnit->getPosition(), Position(commandCenterPos.x, commandCenterPos.y), Color(Colors::Green));
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
	else {
		Position txtPlace =  gameUnit->getPosition();
		Broodwar->registerEvent([txtPlace,barrackType](Game*)
		{
			Broodwar->drawTextMap( txtPlace,
					Broodwar->getLastError().c_str());
		},
		nullptr,  // condition
		barrackType.buildTime() + 100 );
		Broodwar << Broodwar->getLastError() << std::endl;
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