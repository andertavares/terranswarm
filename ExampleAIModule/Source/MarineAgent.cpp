#include "MarineAgent.h"
#include <BWAPI.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include "Task.h"

using namespace BWAPI;
using namespace std;

MarineAgent::MarineAgent(Unit* u){
	_marine = u;
}

MarineAgent::~MarineAgent(void){
}

void MarineAgent::onTask(unordered_map<TaskType, list<Task>*> taskMap){

	unordered_map<TaskType, list<Task>*>::iterator it = taskMap.begin();
	for(unordered_map<TaskType, list<Task>*>::iterator iter = taskMap.begin(); iter != taskMap.end(); ++iter){
		TaskType taskType =  iter->first;
		list<Task>* taskList = iter->second;

		// Task Marine can do = Explore, Attack, GuardBase
		// Task from Task.h
		if(taskType == Explore){
			// Calculate how far this agent is from the near non explored area
			// Avaliate incentive
			for (list<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				
			}
		}
		else if(taskType == Attack){
			// Calculate how and who is beign attacked
			// Calculate how far this agent is from the enemy base
			// Avaliate incentive
			for (list<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				
			}
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

Position MarineAgent::getPositionToScout(Region* myRegion){
	Position returnPosition;
	//_marine->
	/*int maxDist = 17;
	Broodwar->drawCircleMap(_marine.x(),basePos.y(),maxDist*TILE_SIZE,Colors::Yellow,false);
	TilePosition seedTilePos = TilePosition(myPos);
	TilePosition baseTilePos = TilePosition(basePos);
	int x      = seedTilePos.x();
	int y      = seedTilePos.y();
	int length = 1;
	int j      = 0;
	bool first = true;
	int dx     = 0;
	int dy     = 1;	
	while (length < Broodwar->mapWidth()) {
		returnPosition = Position(x*TILE_SIZE, y*TILE_SIZE);
		//check max distance
		if (returnPosition.getDistance(myPos) > maxDist*TILE_SIZE) {
		//if (x > baseTilePos.x()+maxDist && y > baseTilePos.y()+maxDist) {
			if (!checkVisible) return getPositionToScout(myPos, myRegion, basePos, true);
			else return basePos;
		}

		if (x >= 0 && x < Broodwar->mapWidth() && y >= 0 && y < Broodwar->mapHeight() && 
			myRegion == BWTA::getRegion(x,y) && Broodwar->hasPath(myPos,returnPosition) ) {
				//if (x <= baseTilePos.x()+maxDist && y <= baseTilePos.y()+maxDist) {
					if (!checkVisible) {
						if (!Broodwar->isExplored(x,y)) return returnPosition;
					} else {
						if (!Broodwar->isVisible(x,y)) return returnPosition;
					}
				//}
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
	}*/

	return returnPosition;
}