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

MarineAgent::MarineAgent(Unit u) : gameUnit(u), state(NO_TASK), latencyFrames(10){
	lastPosition = Position(0,0);
	lastFrameCount = Broodwar->getFrameCount();
	bunkerToMove = NULL;
}

MarineAgent::~MarineAgent(void){
}

/*---- code snippet for stimpack use, from nova bot
// Use stimpacks when there are medics around:
void CombatAgent::inCombatMarine(Unit *bestTarget, const UnitSet &enemies, SquadAgent *squad)
{
	if (Broodwar->self()->hasResearched(TechTypes::Stim_Packs) && squad->hasUnitOfType(UnitTypes::Terran_Medic)) {
 		if (bestTarget!=0 && !_unit->isStimmed() && _unit->getHitPoints() > 20 && _unit->isAttacking()) {
 			_unit->useTech(TechTypes::Stim_Packs);
 		}
 	}
}---*/


void MarineAgent::onFrame(unordered_map<TaskType, vector<Task>*> taskMap, unordered_map<int, MarineAgent*> colleagues){

	//draws circle if on stim packs
	if(gameUnit->isStimmed()){
		Broodwar->drawCircleMap(gameUnit->getPosition(), 10, Color(Colors::Orange), true); //crazy stimmed \o/ 
	}

	//skips if not completed or current frame is not the one to think in
	if(!gameUnit->isCompleted() || Broodwar->getFrameCount() % latencyFrames != 0) {
		return;
	}
	/*
	if(state == MOVE_BUNKER){
		if(bunkerToMove != NULL){
			if(bunkerToMove->getLoadedUnits() >= 4){
				state = NO_TASK;
				bunkerToMove == NULL;
				attack(target, colleagues);
			}
		}
	}
	*/

	//checks if there is an non-full bunker around and enters it
	Unit u = NULL;
	int prevState = state;
	Unitset closeUnits = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 20 * TILE_SIZE, Filter::IsOwned && Filter::IsBuilding);
	//Broodwar->drawTextMap(gameUnit->getPosition(), "\n\nCLU=%d", closeUnits.size());
	for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
		if(unit->getType() == UnitTypes::Terran_Bunker && unit->isVisible()){
			//Broodwar->sendText("Bunker found");
			if (unit->getLoadedUnits().size() < 4) {
				u = *unit;
				break;
			}
		}
	}

	if(u != NULL){ //found non-full bunker, moving to it
		state = MOVE_BUNKER;
		gameUnit->rightClick(u);
		bunkerToMove = u;
		return;
	}
	else { //bunkers do not exist or are full, return to what i was doing
		state = prevState;
	}


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
			if (!gameUnit->isStimmed() && gameUnit->getHitPoints() > 20 && gameUnit->isAttacking()) {
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

	//builds a list with all the tasks
	vector<Task*> all;
	for(auto taskIter = taskMap.begin(); taskIter != taskMap.end(); ++taskIter){
		for (auto task = taskIter->second->begin(); task != taskIter->second->end(); task++){
			all.push_back(&(*task));
		}
	}

	int index = randomInRange(0, all.size());
	if (all.size() == 0) return;

	toPerform = all[index]; //TODO: error here!

		
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
			Broodwar << "not scouting anymore" << endl;
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
	if(packSize >= 8 || colleaguesAround == packSize || enemiesInSight.size() > 0) {
		// Check for barracks
		state = ATTACKING;
		/*
		//MOVE_BUNKER
		int distance = 0, newDistance = 0;

		Unit u = NULL;

		Unitset closeUnits = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 45 * TILE_SIZE, Filter::IsOwned);
		for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Bunker){
				//unit->getType() == UnitTypes::Terran_SCV){ 
				//&& unit->getHitPoints() < unit->getInitialHitPoints()){
				newDistance = unit->getPosition().getApproxDistance(gameUnit->getPosition());
				if (unit->getLoadedUnits() < 4 && (u == NULL || newDistance < distance)) {
					distance = newDistance;
					u = *unit;
				}
			}
		}

		if(u != NULL){
			state = MOVE_BUNKER;
			gameUnit->rightClick(u);
			bunkerToMove = u;
		}
		else{
			state = ATTACKING;
		}
		*/
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
	//}//closure: state==PACKING

	if(state == ATTACKING){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nATK");
		
		// TESTING
		/*int hitPoints = gameUnit->getHitPoints();

		//if there are targets nearby, choose one appropriately
		Unitset foesAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), gameUnit->getType().groundWeapon().maxRange(), Filter::IsEnemy);
		Unit victim = NULL;
		float minHpRate = 1.0f;
		for(auto foe = foesAround.begin(); foe != foesAround.end(); ++foe){
			//shot workers or medics is priority
			//if (foe->getType().isWorker() || foe->getType() == UnitTypes::Terran_Medic){
			//if (foe->getType() == UnitTypes::Terran_Medic){
			//	victim = *foe;
			//	break;
			//}

			//else, shoots the enemy unit closest to death
			float hpRate = foe->getHitPoints() + foe->getShields() / float(foe->getType().maxHitPoints() + foe->getType().maxShields());
			if (hpRate < minHpRate){
				victim = *foe;
				minHpRate = hpRate;
			}

		}

		Unitset closeUnits = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 7 * TILE_SIZE, Filter::IsOwned);
		int midRange = 0;
		for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Marine){
				midRange++;
			}
		}

		closeUnits = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 3 * TILE_SIZE, Filter::IsOwned);
		int closeRange = 0;
		for (auto unit = closeUnits.begin(); unit != closeUnits.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Marine){
				closeRange++;
			}
		}

		closeUnits = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 6 * TILE_SIZE, Filter::IsEnemy);

		if (victim == NULL) {
			//if(midRange > closeRange && closeUnits.size() <= 1 && (gameUnit->isAttacking() || gameUnit->isMoving())){
			//	gameUnit->stop();
			//}
			//else{
				gameUnit->attack(target);
				Broodwar->drawLineMap(gameUnit->getPosition(),target,Color(Colors::Red));
			//}
		}
		else {
			gameUnit->attack(victim);
			Broodwar->drawLineMap(gameUnit->getPosition(),victim->getPosition(),Color(Colors::Red));
		}

		//if(!gameUnit->isStimmed()){
		//	gameUnit->useTech(BWAPI::TechTypes::Stim_Packs);
			//Broodwar->useTech(command.getUnitID(), command.getArg0());
		//}
		// TESTING END
		*/
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
/*
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
		else if(taskType == GuardBase){
			// Calculate how far from this base this agent is
			// Calculate how many enemies are close to the base
			// Avaliate incentive
			for (list<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				
			}
		}*
		else {
			continue;
		}
	}
}
*/



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

		Broodwar->drawTextMap(myPos, "\n\n%d,%d,%d", Broodwar->hasPath(myPos,pos), Broodwar->isExplored(TilePosition(pos)), Broodwar->isWalkable(WalkPosition(pos)));// {

		Broodwar << "Agent [" << gameUnit->getID() << "] Scouting to :" << pos << std::endl;

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