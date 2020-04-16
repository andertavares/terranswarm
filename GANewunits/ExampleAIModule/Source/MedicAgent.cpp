#include <BWAPI.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include <climits>
#include <deque>
//#include <random>

#include "MedicAgent.h"
#include "Task.h"
//#include "PositionTask.h"
#include "TaskAssociation.h"
#include "util.h"
#include <set>

#include "GeneticValues.h"


using namespace BWAPI;
using namespace std;

// make sure medics stay with the group:
#define MAX_DISTNACE_TO_NONMEDIC	64
// go close to a medic or backwards when low health:
#define MAX_DISTANCE_TO_MEDIC	64

MedicAgent::MedicAgent(Unit u) : gameUnit(u), state(NO_TASK), latencyFrames(10){
	lastPosition = Position(0,0);
	lastFrameCount = Broodwar->getFrameCount();
	lastHealedId = -1;
	lastHealedHP = 0;
}

MedicAgent::~MedicAgent(void){
}

void MedicAgent::onFrame(unordered_map<TaskType, vector<Task>*> taskMap){
	//if is already engaged in task, continues it
	if(!gameUnit->isCompleted()){
		//|| Broodwar->getFrameCount() % latencyFrames != 0) {
		return;
	}

	//Broodwar << GeneticValues::getValue("") << endl;

	/*
	int currentFrameCount = Broodwar->getFrameCount();
	if ( currentFrameCount >= lastFrameCount + 20){
		lastFrameCount = currentFrameCount;
	}
	else{
		return;
	}

	//Broodwar->drawLineMap(gameUnit->getPosition(), lastPosition, Colors::Red);

	// Get our starting location
	if (originPosition.x == 0 && originPosition.y == 0) {
		originPosition = gameUnit->getPosition();
	}


	Unitset closeUnits = Broodwar->getUnitsInRadius(lastPosition, 6 * TILE_SIZE, Filter::IsOwned);
	for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
		if(unit->getType() == UnitTypes::Terran_Marine && unit->getHitPoints() < unit->getInitialHitPoints()){
			gameUnit->attack(unit->getPosition());
			return;
		}
	}

	if(state == CURE_MARINE){
		/*Unitset closeUnits = Broodwar->getUnitsInRadius(lastPosition, 6 * TILE_SIZE, Filter::IsOwned);
		for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Marine && unit->getHitPoints() < unit->getInitialHitPoints()){
				gameUnit->attack(unit->getPosition());
				return;
			}
		}*/

	/*
		Unitset units = Broodwar->getUnitsInRadius(lastPosition, 2 * TILE_SIZE, Filter::IsOwned);
		Broodwar->drawCircleMap(lastPosition,2 * TILE_SIZE,Color(Colors::White));
		int marines = 0;
		for (auto unit = units.begin(); unit != units.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Marine){
				marines++;
			}
		}

		Broodwar->drawTextMap(gameUnit->getPosition(),"\nMarines %d", marines);

		if(marines <= 0) {
			updatePositionToCure();
		}
		else{
			gameUnit->attack(lastPosition);
		}
	}
	else{
		updatePositionToCure();
	}

	gameUnit->attack(lastPosition);
	Broodwar->drawLineMap(gameUnit->getPosition(), lastPosition, Colors::Green);
	*/

	// First, use optical flare:
	Unitset UnitsInRange;
	if (Broodwar->self()->hasResearched(TechTypes::Optical_Flare) && gameUnit->getEnergy()>75) {
		UnitsInRange = gameUnit->getUnitsInRadius(WeaponTypes::Lockdown.maxRange());
		
		for (auto unit = UnitsInRange.begin(); unit != UnitsInRange.end(); unit++){
			if ( Broodwar->self()->isEnemy((unit)->getPlayer()) && !(unit)->isBlind() && !allreadyFired(*unit)) {
				gameUnit->useTech(TechTypes::Optical_Flare, *unit);
			}
		}
	}

	Unit lastHealed = Broodwar->getUnit(lastHealedId);

	/*if(lastHealedId > 0 &&  
		lastHealed != NULL && 
		lastHealed->exists() &&
		lastHealed->getPosition().getApproxDistance(gameUnit->getPosition()) < 40 && 
		lastHealed->getHitPoints() <
		lastHealed->getInitialHitPoints() - 20) 
	{
		//gameUnit->attack(Broodwar->getUnit(lastHealedId)->getPosition());		
		//gameUnit->rightClick(Broodwar->getUnit(lastHealedId));
		gameUnit->useTech(TechTypes::Healing, Broodwar->getUnit(lastHealedId));
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nCONT HEAL MRN");
	}*/

	// compute distance to nearest healable non-medic unit (for now, ignore the fact that medics can heal each other):
	int distance = 0, newDistance = 0;

	Unit u = NULL;

	int hitPoints = 9999, newHitPoints = 0;
	Broodwar->drawCircleMap(gameUnit->getPosition(), 5 * TILE_SIZE ,Color(Colors::White));
	Unitset closeUnits = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 5 * TILE_SIZE, Filter::IsOwned);
	if(closeUnits.size() > 0){
		for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Marine){
				newHitPoints = unit->getHitPoints();
				if (newHitPoints < unit->getInitialHitPoints() && (u == NULL || newHitPoints < hitPoints)) {
					hitPoints = newHitPoints;
					u = *unit;
				}
			}	
			else if(unit->getType() == UnitTypes::Terran_Firebat){
				newHitPoints = unit->getHitPoints();
				if (newHitPoints < unit->getInitialHitPoints() && (u == NULL || newHitPoints < hitPoints)) {
					hitPoints = newHitPoints;
					u = *unit;
				}
			}
		}
	}

	if(u == NULL){
		Unitset closeUnits = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 45 * TILE_SIZE, Filter::IsOwned);
		for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Marine){
				newDistance = unit->getPosition().getApproxDistance(gameUnit->getPosition());
				if (u == NULL || newDistance < distance) {
					distance = newDistance;
					u = *unit;
				}
			}
			else if(unit->getType() == UnitTypes::Terran_Firebat){
				newDistance = unit->getPosition().getApproxDistance(gameUnit->getPosition());
				if (u == NULL || newDistance < distance) {
					distance = newDistance;
					u = *unit;
				}
			}
		}
	}

	

	// if it-s beyond the threshold, find closest unit, and send there:
	if (u != NULL){
		//&& distance > MAX_DISTNACE_TO_NONMEDIC) {
		/*if (u->getPosition().getApproxDistance(gameUnit->getPosition()) > 10) { //tries to not suffocate the marine
			//gameUnit->move(u->getPosition()); 
			//gameUnit->attack(u->getPosition()); 
			gameUnit->rightClick(u);
			//gameUnit->useTech(TechTypes::Healing, u);
			Broodwar->drawTextMap(gameUnit->getPosition(),"\nHEAL MRN1 %u", u->getID());
			return;
		}*/
		
		//Broodwar << "Distance:" << distance << std::endl;
		
		if(u->getPosition().getApproxDistance(gameUnit->getPosition()) <= 50){
			//gameUnit->attack(u->getPosition());
			//gameUnit->rightClick(u);
			gameUnit->useTech(TechTypes::Healing, u);
			Broodwar->drawTextMap(gameUnit->getPosition(),"\nHEAL MRN");
			lastHealedId = u->getID();
		}
		else{
			gameUnit->rightClick(u);
			Broodwar->drawTextMap(gameUnit->getPosition(),"\nMov to MRN");
		}
	}
	else{
		// Ugly and repeated code to does not let the medics standing after all the marines died
		// TODO: Clean this
		u = NULL;
		Unitset closeUnits = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 45 * TILE_SIZE, Filter::IsOwned);
		for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Marine){
				newDistance = unit->getPosition().getApproxDistance(gameUnit->getPosition());
				if (u == NULL || newDistance < distance) {
					distance = newDistance;
					u = *unit;
				}
			}
			else if(unit->getType() == UnitTypes::Terran_Firebat){
				newDistance = unit->getPosition().getApproxDistance(gameUnit->getPosition());
				if (u == NULL || newDistance < distance) {
					distance = newDistance;
					u = *unit;
				}
			}
		}
		if (u != NULL){
			gameUnit->rightClick(u);
			Broodwar->drawTextMap(gameUnit->getPosition(),"\nMov to MRN1");
		}
	}
}


void MedicAgent::updatePositionToCure(){
	double closestDist = 100000;
	int closestId = 0;

	Unitset units = Broodwar->self()->getUnits();
	for (Unitset::iterator unit = units.begin(); unit != units.end(); unit++){
		if(unit->getType() == UnitTypes::Terran_Marine && unit->exists() && unit->isCompleted()){
			double dist = unit->getDistance(unit->getPosition());
			if (dist < closestDist){
				closestDist = dist;
				closestId = unit->getID();
			}
		}
		else if(unit->getType() == UnitTypes::Terran_Firebat && unit->exists() && unit->isCompleted()){
			double dist = unit->getDistance(unit->getPosition());
			if (dist < closestDist){
				closestDist = dist;
				closestId = unit->getID();
			}
		}
	}

	if(closestId != 0){
		state = CURE_MARINE;
		lastPosition = Broodwar->getUnit(closestId)->getPosition();
	}
	else{
		state = NO_TASK;
		lastPosition = originPosition;
	}
}

void MedicAgent::cureInPos(){
	gameUnit->attack(originPosition);
}


bool MedicAgent::isOnAttack(){
	return state == PACKING || state == ATTACKING;
}

bool MedicAgent::allreadyFired(Unit enemy){
	
	Bulletset bullets = Broodwar->getBullets();
	for(Bulletset::iterator i = bullets.begin(); i != bullets.end(); ++i){
		if ( (i)->getSource() == gameUnit && (i)->getTarget() == enemy) {
			return true;
		}
	}

	return false;
}