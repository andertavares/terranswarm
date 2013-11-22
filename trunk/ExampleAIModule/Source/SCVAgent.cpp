#pragma once

#include <BWAPI.h>
#include <deque>
#include <unordered_map>
#include "Task.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h> 
#include <random>
#include <iostream>
#include "SCVAgent.h"
#include "ExampleAIModule.h"

using namespace BWAPI;
using namespace Filter;
using namespace std;

SCVAgent::SCVAgent(BWAPI::Unit scv){
	srand ( time(NULL) );
	gameUnit = scv;
	unitId = gameUnit->getID();
	lastPosition = Position(0,0);
	lastFrameCount = Broodwar->getFrameCount();
	state = CREATED;
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

void SCVAgent::onFrame(unordered_map<TaskType, list<Task>*> taskMap, Unitset theMinerals, Unitset commandCenters){
	
}

void SCVAgent::onTask(unordered_map<TaskType, list<Task>*> taskMap){

	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);

	unordered_map<TaskType, list<Task>*>::iterator it = taskMap.begin();
	for(unordered_map<TaskType, list<Task>*>::iterator iter = taskMap.begin(); iter != taskMap.end(); ++iter){
		TaskType taskType =  iter->first;
		list<Task>* taskList = iter->second;

		// Task SCV can do = BuildSupplyDepot, BuildBarracks, BuildCommandCenter, Fix, GatherMinerals, Explore
		// TaskType from Task.h
		if(taskType == BuildSupplyDepot){
			// evaluate incentive
			for (list<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				
				double uniformOn01 = dis(gen);
				if(uniformOn01 <= it->getIncentive()){
					createSupply();
					Broodwar->drawTextMap(gameUnit->getPosition(),"\nBuilding supply");
					Broodwar << "Agent [" << unitId << "] building supply depot" << std::endl;
					return;
				}
			}
		}
		else if(taskType == BuildBarracks){
			// evaluate incentive
			for (list<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				
			}
		}
		else if(taskType == BuildCommandCenter){
			// Calculate how far is the new command center
			// Calculate how many minerals are near the command center
			// evaluate incentive
			for (list<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				
			}
		}
		else if(taskType == GatherMinerals){
			// If we are not doing anything
			// Go gather minerals
		}
		/*
		* TODO: See if this method is needed.
		else if(taskType == GuardBase){
			// Calculate how far from this base this agent is
			// Calculate how many enemies are close to the base
			// Avaliate incentive
			for (list<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				
			}
		}*/
		else {
			continue;
		}
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
		if(myReg->getID() == targetReg->getID() || (gameUnit->getDistance(nearBaseArea)/TILE_SIZE) <= 1){
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
			state = NOT_BUILDING_BASE;
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
		Broodwar << "Agent [" << unitId << "] Scouting to :" << pos << std::endl;

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
	state = BUILDING_SUPPLY;
	UnitType supplyProviderType = gameUnit->getType().getRace().getSupplyProvider();
	if ( supplyProviderType.isBuilding() ){
		TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, gameUnit->getTilePosition());
		if ( targetBuildLocation ){
			// Register an event that draws the target build location
			Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*){
					Broodwar->drawBoxMap( Position(targetBuildLocation), Position(targetBuildLocation + supplyProviderType.tileSize()),	Colors::Blue);
				},
				nullptr,  // condition
				supplyProviderType.buildTime() + 100 
			);  // frames to run

			// Order the builder to construct the supply structure
			gameUnit->build( supplyProviderType, targetBuildLocation );
		}
	}
}