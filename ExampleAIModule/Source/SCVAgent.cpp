#include "SCVAgent.h"
#include <BWAPI.h>
#include <iostream>

using namespace BWAPI;
using namespace std;

SCVAgent::SCVAgent(BWAPI::Unit scv){
	srand ( time(NULL) );
	gameUnit = scv;
	unitId = gameUnit->getID();
	lastPosition = Position(0,0);
	lastFrameCount = Broodwar->getFrameCount();
}


SCVAgent::~SCVAgent(void){

}

Unit SCVAgent::getUnit(){
	//Broodwar << "Returning id " << unitId << std::endl;
	return gameUnit;
}

void SCVAgent::onTask(unordered_map<TaskType, list<Task>*> taskMap){

	unordered_map<TaskType, list<Task>*>::iterator it = taskMap.begin();
	for(unordered_map<TaskType, list<Task>*>::iterator iter = taskMap.begin(); iter != taskMap.end(); ++iter){
		TaskType taskType =  iter->first;
		list<Task>* taskList = iter->second;

		// Task SCV can do = BuildSupplyDepot, BuildBarracks, BuildCommandCenter, Fix, GatherMinerals, Explore
		// TaskType from Task.h
		if(taskType == BuildSupplyDepot){
			// Avaliate incentive
			for (list<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				
			}
		}
		else if(taskType == BuildBarracks){
			// Avaliate incentive
			for (list<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				
			}
		}
		else if(taskType == BuildCommandCenter){
			// Calculate how far is the new command center
			// Calculate how many minerals are near the command center
			// Avaliate incentive
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
		Broodwar << "Agent [" << unitId << "] Moving to :" << pos << std::endl;

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

		Broodwar << "positionList [" << positionListInRadious.size() << "]" << std::endl;
		Broodwar << "Agent [" << unitId << "] returning random option in Radious:" << returnPosition << std::endl;
		lastPosition = returnPosition;
	}
	else if(positionListInMap.size() > 0){
		int randomIndex = rand() % positionListInMap.size();
		returnPosition = positionListInMap[randomIndex];

		Broodwar << "positionListInMap [" << positionListInMap.size() << "]" << std::endl;
		Broodwar << "Agent [" << unitId << "] returning random option in Map :" << returnPosition << std::endl;
		lastPosition = returnPosition;
	}
	
	return returnPosition;
}