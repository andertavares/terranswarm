#include <BWAPI.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <climits>
#include <deque>
//#include <random>

#include "MarineAgent.h"
#include "Task.h"
//#include "PositionTask.h"
#include "TaskAssociation.h"
#include "util.h"


using namespace BWAPI;
using namespace std;

MarineAgent::MarineAgent(Unit u) : gameUnit(u), state(NO_TASK){
	lastPosition = Position(0,0);
	lastFrameCount = Broodwar->getFrameCount();
}

MarineAgent::~MarineAgent(void){
}

void MarineAgent::onFrame(unordered_map<TaskType, vector<Task>*> taskMap, unordered_map<int, MarineAgent*> colleagues){
	//if is already engaged in task, does nothing
	
	if(! gameUnit->isIdle() || !gameUnit->isCompleted() ) return;

	state = NO_TASK;
	vector<TaskAssociation> taskAssociations;

	int maxDistance = Position(0,0).getDistance(Position(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE));
	Task* toPerform = NULL;
	for(auto taskIter = taskMap.begin(); taskIter != taskMap.end(); ++taskIter){
		
		if (toPerform != NULL) break; //gets out if i have chosen a task
		//if( == 
		float capability = 0;

		switch(taskIter->first){
		
		case Attack:
			//inserts an TaskAssociation for each Attack task; capability is proportional to distance to the task
			//TODO: increase capability with the number of colleagues attacking

			for (auto atk = taskIter->second->begin(); atk != taskIter->second->end(); atk++){
				capability = 1.0f - (gameUnit->getDistance(atk->getPosition()) / float(maxDistance));

				if( (rand() / RAND_MAX) < TaskAssociation(&(*atk), capability).tValue()){
					toPerform = &(*atk);
					//state = ATTACKING;
				}
				
				//taskAssociations.push_back(TaskAssociation(&(*atk), capability));
			}
			break;

		case Explore:
			capability = 0.3f;
			Task* exp = &(taskIter->second->at(0));
			if( (rand() / RAND_MAX) < TaskAssociation(exp, capability).tValue()){
				toPerform = exp;
				state = EXPLORING;
			}
			//taskAssociations.push_back(TaskAssociation(&(taskIter->second->at(0)), capability));
			break;

		} //closure: switch

		if (capability == 0){
			continue; //agent is not able to perform tasks of this type, go to next
		}
		
	}
	/*if(gameUnit->getID() == 1){
		int offset = 0;
		//Broodwar->drawTextScreen(200,115,"%d items", feasibleTasks.size());
		for (auto ta = taskAssociations.begin(); ta != taskAssociations.end(); ta++){
			Broodwar->drawTextScreen(200,115+offset,"%d - %d", ta->task()->getTaskType(), ta->task()->getIncentive());
			offset += 15;
		}
		
	}*/
	//Broodwar->sendText("TA sz: %d", taskAssociations.size());
	/*
	if (gameUnit->getID() == 85){
		std::string tsk = "";
		int offset = 40;
		//Broodwar->drawTextMap(gameUnit->getPosition(),"\n\n\nTA:");
		for(auto ta = taskAssociations.begin(); ta != taskAssociations.end(); ++ta){
			//std::string fmt = "%d - %.2f";
			Broodwar->drawTextMap(gameUnit->getPosition().x, gameUnit->getPosition().y+offset,"%d-%.2f - %.2f - %d-%d",ta->task()->getTaskType(), ta->task()->getIncentive(), ta->tValue(), gameUnit->getDistance(ta->task()->getPosition()), maxDistance);
			offset += 10;
		}
		
		

	}
	*/
	//Task* toPerform = weightedSelection(taskAssociations);
	//Broodwar->drawTextMap(gameUnit->getPosition(),"\n\nTAsz: %d", taskAssociations.size());
	if (toPerform == NULL){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nINV");
		return;
	}
	
	
	if(toPerform->getTaskType() == Explore){
		state = EXPLORING;
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nEX");
		if(!goScout()){ //if scouting does not work out, do something else
			gameUnit->stop(); //become idle
			state = NO_TASK;
		}
	}

	else if(toPerform->getTaskType() == Attack){
		//state = ATTACKING;
		attack(toPerform->getPosition(), colleagues);
		//gameUnit->attack(toPerform->getPosition());
		
	}
}

void MarineAgent::attack(Position theTarget, unordered_map<int, MarineAgent*> colleagues){
	state = PACKING; //prepares to join colleagues and then attacks
	target = theTarget;
	attack(colleagues);
}

void MarineAgent::attack(unordered_map<int, MarineAgent*> colleagues){
	Broodwar->drawCircleMap(gameUnit->getPosition(),MEETING_RADIUS,Color(Colors::Green));
	//if(state == PACKING) {
		//first, packs with colleagues to build an effective force
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nPCK");
		int packSize = 0;
		int colleaguesAround = 0;

		//counts how many colleagues are close and attacking
		for(auto colleague = colleagues.begin(); colleague != colleagues.end(); colleague++){
			int dist = gameUnit->getDistance(colleague->second->gameUnit);

			if (dist < MEETING_RADIUS){
				colleaguesAround++;
			}

			if (colleague->second->isOnAttack() &&  dist < gameUnit->getType().sightRange()){
				packSize++;
			}
		}

		//if pack size is enough or has not enough colleagues around to pack, attacks
		if(packSize >= 6 || colleaguesAround == packSize) {
			state = ATTACKING;
		}
		else{ //tries to pack-up with near colleagues
			//tries to get close to the marine with the lowest ID around
			Unit oldestColleague = oldestColleagueAround();
			if (oldestColleague == NULL){
				//nobody found, attacks alone
				state = ATTACKING;
			}
			else {
				//found a buddy, moves towards him firing up anything in the way
				gameUnit->attack(oldestColleague->getPosition());
				Broodwar->drawLineMap(gameUnit->getPosition(), oldestColleague->getPosition(), Color(Colors::Green));
			}
		}
	//}

	if(state == ATTACKING){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nATK");
		gameUnit->attack(target);
	}

	

}

Unit MarineAgent::oldestColleagueAround(){
	int minID = INT_MAX;
	Unit oldest = NULL;
	
	Unitset colleagues = Broodwar->getUnitsInRadius(gameUnit->getPosition(), MEETING_RADIUS, Filter::IsOwned);
	
	for(auto colleague = colleagues.begin(); colleague != colleagues.end(); colleague++){
		if(colleague->getType() == UnitTypes::Terran_Marine && colleague->getID() < minID){
			minID = colleague->getID();
			oldest = *colleague;
		}
	}

	return oldest;

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




bool MarineAgent::isOnAttack(){
	return state == PACKING || state == ATTACKING;
}

bool MarineAgent::goScout(){

	Position myPos = gameUnit->getPosition();
	Broodwar->drawLineMap(myPos, lastPosition, Colors::Blue);

	double square_dist = pow((lastPosition.x- myPos.x), 2.0) + pow((lastPosition.y - myPos.y), 2.0);
	if (lastPosition.x != 0 && lastPosition.y != 0 && square_dist >= pow(3*TILE_SIZE, 2.0)){
		gameUnit->attack(lastPosition);
		return false;
	}

	int currentFrameCount = Broodwar->getFrameCount();
	if ( currentFrameCount >= lastFrameCount + 20){
		lastFrameCount = currentFrameCount;
		Position pos = getPositionToScout();
		Broodwar << "Agent [" << unitId << "] Scouting to :" << pos << std::endl;

		gameUnit->attack(pos);
		return true;
	}

	return false;
}

Position MarineAgent::getPositionToScout(){
	Position returnPosition;
	Unit unit = gameUnit;//((Unit) gameUnit);

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