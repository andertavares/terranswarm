#include <BWAPI.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <climits>
#include <deque>
//#include <random>

#include "FirebatAgent.h"
#include "Task.h"
#include "Parameters.h"
#include "GeneticValues.h"
#include "TaskAssociation.h"
#include "util.h"


using namespace BWAPI;
using namespace std;

FirebatAgent::FirebatAgent(Unit u) : gameUnit(u), state(NO_TASK), latencyFrames(10){
	lastPosition = Position(0,0);
	lastFrameCount = Broodwar->getFrameCount();
}

FirebatAgent::~FirebatAgent(void){
}


void FirebatAgent::onFrame(unordered_map<TaskType, vector<Task>*> taskMap, unordered_map<int, FirebatAgent*> colleagues){

	//draws circle if on stim packs
	if(gameUnit->isStimmed()){
		Broodwar->drawCircleMap(gameUnit->getPosition(), 10, Color(Colors::Orange), true); //crazy stimmed \o/ 
	}

	//skips if not completed or current frame is not the one to think in
	if(!gameUnit->isCompleted() || Broodwar->getFrameCount() % latencyFrames != 0) {
		return;
	}

	map<int, double>& parameters = GeneticValues::getMap();



	//if is already engaged in task, continues it. also, tries to use stim packs while attacking
	if(! gameUnit->isIdle() ){

		//if attacking, check for stim packs!
		Unitset friendsAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), gameUnit->getType().groundWeapon().maxRange() - 20, Filter::IsAlly);
		//Broodwar->drawCircleMap(lastPosition,gameUnit->getType().groundWeapon().maxRange(),Color(Colors::Blue));

		//checks if there is a medic around
		bool medicAround = false;
		for (auto unit = friendsAround.begin(); unit != friendsAround.end(); unit++){
			if (unit->getType() == UnitTypes::Terran_Medic){
				medicAround = true;
				break;
			}
		}

		if (Broodwar->self()->hasResearched(TechTypes::Stim_Packs) && medicAround) {
			Broodwar->drawTextMap(gameUnit->getPosition(), "\n\nCan Stim");
 			//if (foesAround.size() > 0 && !gameUnit->isStimmed() && gameUnit->getHitPoints() > 20 && gameUnit->isAttacking()) {
			if (!gameUnit->isStimmed() && gameUnit->getHitPoints() > 25 && gameUnit->isAttacking()) {
 				int maxRange = gameUnit->getType().groundWeapon().maxRange();
				BWAPI::Position p = gameUnit->getPosition();
				Broodwar->registerEvent(
							[p,maxRange](Game*){
								Broodwar->drawCircleMap( p, maxRange -20,	Colors::Blue);
							},
							nullptr,  // condition
							1000 
				);  // frames to run
				gameUnit->useTech(TechTypes::Stim_Packs);
 			}
 		}

		return;
	}

	
	//else, pick something to do
	state = NO_TASK;
	vector<TaskAssociation> taskAssociations;

	int maxDistance = Position(0,0).getApproxDistance(Position(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE));
	Task* toPerform = NULL;
	for(auto taskIter = taskMap.begin(); taskIter != taskMap.end(); ++taskIter){
		
		if (toPerform != NULL) break; //gets out if i have chosen a task
		//if( == 
		float capability = 0;

		switch(taskIter->first){
		
		case Attack:
			//inserts an TaskAssociation for each Attack task; 
			//capability depends whether task is near, mid distance or far

			for (auto atk = taskIter->second->begin(); atk != taskIter->second->end(); atk++){
				//capability = 1.0f - (gameUnit->getPosition().getApproxDistance(atk->getPosition()) / float(maxDistance));
				
				//calculates the 'normalized' distance from the agent to the task (1.0 = length of map diagonal)
				float normDist = (gameUnit->getPosition().getApproxDistance(atk->getPosition()) / float(maxDistance));

				if(normDist < .33) capability = float(parameters[K_MARINE_ATTACK_NEAR])/2;
				else if (normDist < .66) capability = float(parameters[K_MARINE_ATTACK_NEAR])/2;
				else capability = float(parameters[K_MARINE_ATTACK_NEAR])/2;
				

				if( (rand() / RAND_MAX) < TaskAssociation(&(*atk), capability).tValue()){
					toPerform = &(*atk);
					//state = ATTACKING;
				}
				
				//taskAssociations.push_back(TaskAssociation(&(*atk), capability));
			}
			break;

		} //closure: switch

		if (capability == 0){
			continue; //agent is not able to perform tasks of this type, go to next
		}
		
	}
	
	//Task* toPerform = weightedSelection(taskAssociations);
	//Broodwar->drawTextMap(gameUnit->getPosition(),"\n\nTAsz: %d", taskAssociations.size());
	if (toPerform == NULL){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nINV");
		return;
	}
	
	else if(toPerform->getTaskType() == Attack){
		//state = ATTACKING;
		attack(toPerform->getPosition(), colleagues);
		//gameUnit->attack(toPerform->getPosition());
		
	}
}

void FirebatAgent::attack(Position theTarget, unordered_map<int, FirebatAgent*> colleagues){
	state = PACKING; //prepares to join colleagues and then attacks
	target = theTarget;
	attack(colleagues);
}

void FirebatAgent::attack(unordered_map<int, FirebatAgent*> colleagues){
	Broodwar->drawCircleMap(gameUnit->getPosition(),MEETING_RADIUS,Color(Colors::Green));
	//if(state == PACKING) {

	//first, packs with colleagues to build an effective force
	//Broodwar->drawTextMap(gameUnit->getPosition(),"\nPCK");
	int packSize = 0;
	int colleaguesAround = 0;

	//counts how many colleagues are close and attacking
	for(auto colleague = colleagues.begin(); colleague != colleagues.end(); colleague++){
		int dist = gameUnit->getPosition().getApproxDistance((colleague->second->gameUnit->getPosition()));

		if (dist < MEETING_RADIUS){
			colleaguesAround++;
		}

		if (colleague->second->isOnAttack() &&  dist < gameUnit->getType().sightRange()){
			packSize++;
		}
	}
	Broodwar->drawTextMap(gameUnit->getPosition(),"\nPCK sz:%d; meetRadius:%d", packSize, colleaguesAround);

	//retrieves the enemies around
	Unitset enemiesInSight = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 7 * TILE_SIZE, Filter::IsEnemy);
	
	//if pack size is enough or has not enough colleagues around to pack or has enemy in sight, attacks
	map<int, double>& parameters = GeneticValues::getMap();
	if(packSize >= parameters[M_PACK_SIZE] || colleaguesAround == packSize || enemiesInSight.size() > 0) {
		// Check for barracks
		state = ATTACKING;
	}
	else{ //tries to pack-up with near colleagues
		//tries to get close to the Firebat with the lowest ID around
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
	//}//closure: state==PACKING

	if(state == ATTACKING){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nATK");
		gameUnit->attack(target);
	}

	

}

Unit FirebatAgent::oldestColleagueAround(){
	int minID = INT_MAX;
	Unit oldest = NULL;
	
	Unitset colleagues = Broodwar->getUnitsInRadius(gameUnit->getPosition(), MEETING_RADIUS, Filter::IsOwned);
	
	for(auto colleague = colleagues.begin(); colleague != colleagues.end(); colleague++){
		if(colleague->getType() == UnitTypes::Terran_Firebat && colleague->getID() < minID){
			minID = colleague->getID();
			oldest = *colleague;
		}
	}

	return oldest;

}

bool FirebatAgent::isOnAttack(){
	return state == PACKING || state == ATTACKING;
}
