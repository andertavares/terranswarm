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
#include "GeneticValues.h"
#include "Parameters.h"

using namespace BWAPI;
using namespace Filter;
using namespace std;

//SCVAgent::stateNames[NO_TASK] = NO_TASK;

SCVAgent::SCVAgent(Unit scv, ExampleAIModule* aiModule) : _aiModule(aiModule), repairTarget(NULL), 
atkTarget(BWAPI::Positions::Invalid), toAttack(NULL), newBuildingLocation(BWAPI::TilePositions::Invalid), latencyFrames(10)
{
	
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


//TODO: SCV is building cmd center in invalid position!
//TODO: scvs to build barracks in new cmd center is locking up
void SCVAgent::onFrame(unordered_map<TaskType, vector<Task>*> *taskMap, vector<Position> mineralPositions, Unitset commandCenters, SCVMap scvMap){	

	// Get our starting location
	if (originPosition.x == 0 && originPosition.y == 0) {
		originPosition = gameUnit->getPosition();
	}

	if(Broodwar->getFrameCount() % latencyFrames != 0){
		return;
	}

	Broodwar->drawTextMap(gameUnit->getPosition(),"\n\n%d-%d-%d-%d", state, gameUnit->isGatheringMinerals(), gameUnit->isConstructing(), gameUnit->getOrder());
	//Broodwar->drawCircleMap(gameUnit->getPosition(),gameUnit->getType().sightRange(),Color(Colors::Blue));
	if(!gameUnit->isCompleted()){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nIncomplete");
		return;
	}

	if(state == EXPLORING){
		// Check if are enemies nearby and RUN!
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
				state = NO_TASK;
			}
		}
	}

	if(state == ATTACKING){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nATK");
		attack();
		return;
	}

	if(isBuildingExpansion()){
		buildCommandCenter(mineralPositions, commandCenters);
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

	if(gameUnit->isConstructing() ){
		UnitType what = gameUnit->getBuildType();//gameUnit->getBuildUnit()->getType();
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nConstructing %s", what.getName().c_str());
		return;
	} 
	else if(state == BUILDING_BARRACKS){ //if unit should be building barracks but it isn't, either the job is finished or location is obstructed
		//Broodwar << "could not build barracks" << endl;
		Position pos = gameUnit->getPosition();
		Broodwar->registerEvent(
			[pos](Game*) {
				Broodwar->drawCircleMap(pos,20,Color(Colors::Orange));
			},
			nullptr,  // condition
			50 // frames to run
		);  
		state = NO_TASK;
		newBuildingLocation = BWAPI::TilePositions::Invalid;
	}
	else if(state == BUILDING_BUNKER){ //if unit should be building barracks but it isn't, either the job is finished or location is obstructed
		//Broodwar << "could not build barracks" << endl;
		Position pos = gameUnit->getPosition();
		Broodwar->registerEvent(
			[pos](Game*) {
				Broodwar->drawCircleMap(pos,20,Color(Colors::Grey));
			},
			nullptr,  // condition
			50 // frames to run
		);  
		state = NO_TASK;
		newBuildingLocation = BWAPI::TilePositions::Invalid;
	}


	if(gameUnit->isMoving()){
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nMoving");
		return;	
	}
	
	map<int, double>& parameters = GeneticValues::getMap();
	

	// Simple approach to local incentives
	if ( lastChecked + 75 < Broodwar->getFrameCount() || lastChecked == 0) {
		for(unordered_map<TaskType, vector<Task>*>::iterator iter = taskMap->begin(); iter != taskMap->end(); ++iter){
			TaskType taskType =  iter->first;
			vector<Task>* taskList = iter->second;

			if(taskType == GatherMinerals){
				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
					auto task = it;
					float rNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					TaskAssociation taskA = TaskAssociation(&(*task), parameters[K_SCV_GATHER_MINERALS]);

					if(taskA.tValue() > rNumber){
						state = GATHERING_MINERALS;
						Broodwar->drawTextMap(gameUnit->getPosition(),"\nGathering");
						lastChecked = Broodwar->getFrameCount();
						
						if ( gameUnit->isCarryingGas() || gameUnit->isCarryingMinerals() ) {
							gameUnit->returnCargo();
						}
						else if ( !gameUnit->getPowerUp() ) { 
							Unitset scvAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 8 * TILE_SIZE, Filter::IsWorker);
							int scvNearGatheringGas = 0;
							for(auto scvIt = scvAround.begin(); scvIt != scvAround.end(); ++scvIt){
								if((*scvIt)->isGatheringGas() ){
									scvNearGatheringGas++;
								}
							}
							// Check if there is another ones gathering gas
							// If there is <3 gathering gas then get the closest mineral or gas
							// Else get only closest mineral
							if(scvNearGatheringGas < 2){
								if ( !gameUnit->gather( gameUnit->getClosestUnit( IsMineralField || IsRefinery )) ) {
									// If the call fails, then print the last error message
									Broodwar << Broodwar->getLastError() << "\n";
								}
							}
							else{
								if ( !gameUnit->gather( gameUnit->getClosestUnit( IsMineralField)) ) {
									// If the call fails, then print the last error message
									Broodwar << Broodwar->getLastError() << "\n";
								}
							}
							
							/*if ( !gameUnit->gather( gameUnit->getClosestUnit( IsMineralField || IsRefinery )) ) {
								Broodwar << Broodwar->getLastError() << std::endl;
							}*/
						}
						return;
					}
				}
			}

			else if(taskType == BuildVespeneGas){
				
				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
				//for (auto task = taskList->begin(); task != taskList->end(); task++){		
					auto task = it;
					float rNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					TaskAssociation taskA = TaskAssociation(&(*task), 0.3f);
					
					if(taskA.tValue() > rNumber){
						Broodwar << "Agent [" << unitId << "] Task GAS " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << "\n";
						for (auto c = commandCenters.begin(); c != commandCenters.end(); c++){
							Unitset units = Broodwar->getUnitsInRadius((*c)->getPosition(), BASE_RADIUS, Filter::IsResourceContainer);
							for (auto unit = units.begin(); unit != units.end(); unit++){
								if(!(*unit)->getType().isRefinery()){
									UnitType supplyProviderType = gameUnit->getType().getRace().getRefinery();
									if ( supplyProviderType.isBuilding() ){
										newBuildingLocation = Broodwar->getBuildLocation(supplyProviderType, (*unit)->getTilePosition());
										TilePosition targetBuildLocation = newBuildingLocation;
										if ( newBuildingLocation ){
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

									Broodwar->drawTextMap(gameUnit->getPosition(),"\nRefinery");
									lastChecked = Broodwar->getFrameCount();
									it = taskList->erase(it);
									return;
								}
							}

						}
					}
				}
			}

			else if(taskType == BuildAcademy){
				
				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
					auto task = it;
					float rNumber = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
					TaskAssociation taskA = TaskAssociation(&(*task), 0.3f);
					
					if(taskA.tValue() > rNumber){
						Broodwar << "Agent [" << unitId << "] Task Academy " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << "\n";
						
						UnitType supplyProviderType = UnitTypes::Terran_Academy;
						if ( supplyProviderType.isBuilding() ){
							newBuildingLocation = Broodwar->getBuildLocation(supplyProviderType, gameUnit->getTilePosition());
							TilePosition targetBuildLocation = newBuildingLocation;
							if ( newBuildingLocation ){
								// Register an event that draws the target build location
								Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*){
										Broodwar->drawBoxMap( Position(targetBuildLocation), Position(targetBuildLocation + supplyProviderType.tileSize()),	Colors::Orange);
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

						Broodwar->drawTextMap(gameUnit->getPosition(),"\nAcademy");
						lastChecked = Broodwar->getFrameCount();
						it = taskList->erase(it);
						return;
					}	
				}
			}

			else if(taskType == Repair){
				int maxDistance = Position(0,0).getDistance(Position(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE));

				for(auto repair = taskList->begin(); repair != taskList->end(); repair++){
					float rNumber = rand() / float(RAND_MAX);
					int dist = gameUnit->getPosition().getApproxDistance(repair->getPosition());

					//calculates the 'normalized' distance from the agent to the task (1.0 = length of map diagonal)
					float capability = 0.0f;
					float normDist = (gameUnit->getPosition().getApproxDistance(repair->getPosition()) / float(maxDistance));
					if(normDist < .33) capability = float(parameters[K_SCV_REPAIR_NEAR]);
					else if (normDist < .66) capability = float(parameters[K_SCV_REPAIR_MID]);
					else capability = float(parameters[K_SCV_REPAIR_FAR]);

					//float diff = 1.0f - gameUnit->getPosition().getApproxDistance(repair->getPosition()) / float(maxDistance);

					TaskAssociation taskA = TaskAssociation(&(*repair), capability);// pow(float(EULER),-dist));
					//Broodwar->sendText("Rpr: s:%.3f, T:%.3f, k:%.3f", repair->getIncentive(), taskA.tValue(), diff);// pow(float(EULER),-diff));
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
					TaskAssociation taskA = TaskAssociation(&(*task), 0.3f);
					
					if(taskA.tValue() > rNumber){
						Broodwar << "Agent [" << unitId << "] Task supply dep " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << "\n";
						// Check if other SCV near by are constructing the same thing
						/*Unitset scvAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 20 * TILE_SIZE, Filter::IsWorker);
						int scvNearConstructingSCV = 0;
						for(Unitset::iterator scvIt = scvAround.begin(); scvIt != scvAround.end(); ++scvIt){
							if(scvIt->isConstructing() ){
								//&& scvIt->getBuildUnit() !=NULL
								//&& scvIt->getBuildUnit()->getType() == UnitTypes::Terran_Supply_Depot){
								scvNearConstructingSCV++;
							}
						}
						*/
						//if(scvNearConstructingSCV <= 1){
							Broodwar->drawTextMap(gameUnit->getPosition(),"\nSupply Depot");
							lastChecked = Broodwar->getFrameCount();
							createSupply();
							it = taskList->erase(it);
							return;
						//}
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
					float rNumber = rand() / float(RAND_MAX);
					//distfactor ranges [0:0.5]: when it is .5, agent capability to build cmd center is minimum
					float distFactor = .5f * (gameUnit->getPosition().getApproxDistance(task->getPosition()) / float(maxDistance));

					//agent capability decreases exponentially with the distance to the task
					TaskAssociation taskA = TaskAssociation(&(*task), pow(float(EULER),-distFactor));
					
					if(rNumber < taskA.tValue()){
						//Broodwar << "Agent [" << unitId << "] Task barrack " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << std::endl;
						// Check if other SCV near by are constructing the same thing
						/*
						Unitset scvAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 20 * TILE_SIZE, Filter::IsWorker);
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
						*/
						//it = taskList->erase(it); <<this does not prevent more then one SCV trying to build in the same place
						//it->setIncentive(0); <<this does not prevent more then one SCV trying to build in the same place
						createBarrackNearCommandCenter(it->getPosition());
						taskList->erase(it);
						return;
					}
				}
			}
			else if(taskType == BuildBunker){
				// evaluate incentive
				//TODO: build barracks around new bases
				//calculates max map distance
				int maxDistance = Position(0,0).getApproxDistance(Position(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE));

				for (vector<Task>::iterator it = taskList->begin(); it != taskList->end(); it++){
					auto task = it;
					float rNumber = rand() / float(RAND_MAX);
					//distfactor ranges [0:0.5]: when it is .5, agent capability to build cmd center is minimum
					float distFactor = .5f * (gameUnit->getPosition().getApproxDistance(task->getPosition()) / float(maxDistance));

					//agent capability decreases exponentially with the distance to the task
					TaskAssociation taskA = TaskAssociation(&(*task), pow(float(EULER),-distFactor));
					
					if(rNumber < taskA.tValue()){
						//Broodwar << "Agent [" << unitId << "] Task barrack " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << std::endl;
						// Check if other SCV near by are constructing the same thing
						/*
						Unitset scvAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 20 * TILE_SIZE, Filter::IsWorker);
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
						*/
						//it = taskList->erase(it); <<this does not prevent more then one SCV trying to build in the same place
						//it->setIncentive(0); <<this does not prevent more then one SCV trying to build in the same place
						createBunkerNearCommandCenter(it->getPosition());
						taskList->erase(it);
						return;
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
							Broodwar << "Agent [" << unitId << "] Task command " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << "\n";
							lastChecked = Broodwar->getFrameCount();
							buildCommandCenter(mineralPositions, commandCenters);
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
							Broodwar << "Agent [" << unitId << "] Task explore " << taskA.tValue() << " Incentive " << taskA.task()->getIncentive() << " Numbr " << rNumber << "\n";
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
			Unitset scvAround = Broodwar->getUnitsInRadius(gameUnit->getPosition(), 8 * TILE_SIZE, Filter::IsWorker);
			int scvNearGatheringGas = 0;
			for(auto scvIt = scvAround.begin(); scvIt != scvAround.end(); ++scvIt){
				if((*scvIt)->isGatheringGas() ){
					scvNearGatheringGas++;
				}
			}
			// Check if there is another ones gathering gas
			// If there is <3 gathering gas then get the closest mineral or gas
			// Else get only closest mineral
			if(scvNearGatheringGas < 2){
				if ( !gameUnit->gather( gameUnit->getClosestUnit( IsMineralField || IsRefinery )) ) {
					// If the call fails, then print the last error message
					Broodwar << Broodwar->getLastError() << "\n";
				}
			}
			else{
				if ( !gameUnit->gather( gameUnit->getClosestUnit( IsMineralField)) ) {
					// If the call fails, then print the last error message
					Broodwar << Broodwar->getLastError() << "\n";
				}
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
		
		//discards task if foe is far or is flying unit
		if(gameUnit->getPosition().getApproxDistance(closestFoe->getPosition()) > gameUnit->getType().sightRange() || closestFoe->getType().isFlyer()){
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
	if(unitsThere.size() == 1 && (*(unitsThere.begin()))->getType().isBuilding()){
		repairTarget = *(unitsThere.begin());
		goRepair();
	}
}

void SCVAgent::goRepair(){
	
	//if target has more than 80% of energy or another SCV is reparing target or cannot repair, returns to no_task state
	if(  shouldBeRepaired(repairTarget) ){// || !gameUnit->repair(repairTarget)) {
		state = REPAIRING;
		
		//if(!repairTarget->isBeingConstructed() && !repairTarget->isCompleted()) {
			//gameUnit->repair(repairTarget);
		gameUnit->rightClick(repairTarget);
			//gameUnit->
		//}

		Broodwar->drawTextMap(gameUnit->getPosition(),  "\n\n\nI AM repairing");
		Broodwar->drawLineMap(gameUnit->getPosition(),repairTarget->getPosition(), Color(Colors::Cyan));
	}
	else {
		state = NO_TASK;
		repairTarget = NULL;
		gameUnit->stop();
		Broodwar->drawTextMap(gameUnit->getPosition(),  "\n\n\nWILL NOT REPAIR");
	}

	
}

/**
  * Returns whether the target should be repaired
  * True when target is less than 80% healthy or construction is halted and no other SCV is repairing it
  */
bool SCVAgent::shouldBeRepaired(Unit target){
	if (!target->exists()) return false;

	unordered_map<int, SCVAgent*>& scvMap = _aiModule->getSCVMap();

	bool anotherRepairer = false;
	for(auto scv = scvMap.begin(); scv != scvMap.end(); scv++){
		if(scv->second->gameUnit->getID() == this->gameUnit->getID()) continue;

		if(scv->second->isRepairing() && scv->second->repairTarget == repairTarget){
			anotherRepairer = true;
			//Broodwar->drawTextMap(gameUnit->getPosition(),  "\n\n\nanother repairing");
			//Broodwar << "another repairing, me? ";
			break;
		}
		
	}
	
	if(!anotherRepairer && (!repairTarget->isCompleted() || repairTarget->getHitPoints() < .8f * repairTarget->getType().maxHitPoints() ) ){
		//Broodwar <<  "should repair" << endl;
		return true;
	}
	//Broodwar <<  "should NOT repair" << endl;
	return false;
}


bool SCVAgent::isRepairing(){
	return (gameUnit->isRepairing() || state == REPAIRING) && repairTarget != NULL;
}


bool SCVAgent::isDangerous(Position pos){
	vector<Task>* atkTasks = _aiModule->getTasks()[Attack];

	bool inAttackArea = false;
	for(auto task = atkTasks->begin(); task != atkTasks->end(); task++) {
		if(task->getPosition().getApproxDistance(pos) < 10 * TILE_SIZE){
			inAttackArea = true;
			break;
		}
	}

	return inAttackArea;

}

/**
  * Implements the decision of building a new command center (cmd).
  * The SCV chooses a location (near mineral field) to build the cmd, moves towards it and
  * places the new cmd close to the location
  */
void SCVAgent::buildCommandCenter(vector<Position> theMinerals, Unitset commandCenters){
	//Unitset uncoveredMinerals;
	
	Position pos = gameUnit->getPosition();
	Broodwar->registerEvent(
		[pos](Game*) {
			Broodwar->drawCircleMap(pos,20,Color(Colors::Red));
		},
		nullptr,  // condition
		70 // frames to run
	);  

	if (state == MOVING_TO_NEW_BASE) {
		//checks if destination is valid
		if(!nearBaseArea.isValid() || isDangerous(nearBaseArea)){
			state = NO_TASK;
			gameUnit->stop();
			return;
			Broodwar << "Invalid or dangerous base target" << "\n";
		}
		Broodwar->drawTextMap(gameUnit->getPosition(),"\nMoving to base: %d %d", nearBaseArea.x, nearBaseArea.y);
		Broodwar->drawLineMap(gameUnit->getPosition(),nearBaseArea,Color(Colors::Orange));
		gameUnit->move(nearBaseArea);

		Region targetReg = Broodwar->getRegionAt(nearBaseArea);
		Region myReg = gameUnit->getRegion();

		//Broodwar->drawTextMap(gameUnit->getPosition(),"\nmyReg: %d, tgtReg: %d, d:%d", myReg->getID(), targetReg->getID(), gameUnit->getDistance(nearBaseArea));

		//sometimes regions are buggy, so we test if we are very close to the target point
		if(((myReg && targetReg) && myReg->getID() == targetReg->getID()) || (gameUnit->getPosition().getApproxDistance(nearBaseArea)/TILE_SIZE) <= 2){
			Broodwar << myReg << targetReg << "\n";
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
		
		newBuildingLocation = Broodwar->getBuildLocation(centerType, gameUnit->getTilePosition() );
		

		//TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
		//TODO: insist in case of "something is in the way" error
		if (newBuildingLocation){

			// Register an event that draws the target build location
			TilePosition targetBuildLocation = newBuildingLocation;
			Broodwar->registerEvent([targetBuildLocation,centerType](Game*)
			{
				Broodwar->drawBoxMap( Position(targetBuildLocation),
					Position(targetBuildLocation + centerType.tileSize()),
					Colors::Orange);
			},
			nullptr,  // condition
			centerType.buildTime() + 100 );  // frames to run

			//if ( targetBuildLocation ){

				// Order the builder to construct the supply structure
				gameUnit->build(centerType, newBuildingLocation);
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
		if(nearBaseArea.x != -1 && nearBaseArea.y != -1 && nearBaseArea.isValid()) {
			Broodwar->sendText("Tgt point consistent");
			Broodwar->sendText("Tgt point consistent");
			Broodwar->sendText("Tgt point consistent");
			Broodwar->drawTextMap(gameUnit->getPosition(),"\n\nconsistent tgt");
			state = MOVING_TO_NEW_BASE;
		}
		else {
			//Broodwar->sendText("Tgt point INconsistent");
			Broodwar->drawTextMap(gameUnit->getPosition(),"\n\nINconsistent tgt");
			Broodwar << "Invalid base target" << "\n";
			Broodwar << "Invalid base target" << "\n";
			Broodwar << "Invalid base target" << "\n";
			state = NO_TASK;
			gameUnit->stop();
			return;
		}

	}
}

bool SCVAgent::isBuildingExpansion(){
	return state == MOVING_TO_NEW_BASE || state == IN_BASE_AREA || state == BUILDING_BASE;
}

//TODO: check if mineral field is under one attack task...
Position SCVAgent::pointNearNewBase(vector<Position> theMinerals, Unitset commandCenters){

	Position closestMineral = Positions::Invalid;//NULL;//Position(1,1); //mineral to which the base will be built
	//closestMineralPosition = new Position(0,0);

	int minDistance = INT_MAX;
	/*
	Unitset staticMinerals = Broodwar->getStaticMinerals();
	for(auto m = staticMinerals.begin(); m != staticMinerals.end(); m++){
		if(! m->getPosition().isValid()){
			Broodwar->sendText("INVALID pos: %d,%d", m->getPosition().x, m->getPosition().y);
			return;
		}
	}
	*/

	//finds out which mineral is closest to the scv
	//Unitset::iterator mineral;
	//bool allReachable = true;
	for (auto mineral = theMinerals.begin(); mineral != theMinerals.end(); ++mineral){
		bool covered = false;
		for(auto cmd = commandCenters.begin(); cmd != commandCenters.end(); ++cmd){
			if ((*cmd)->getPosition().getApproxDistance(*mineral) < BASE_RADIUS){
				covered = true;
				break;
			}
		}

		//also tests if point is close to attack area (dangerous, should not go there)
		if (!covered && !isDangerous(*mineral)){
			//Broodwar->sendText("Unreachable pos: %d,%d", (*mineral).x, (*mineral).y);
			//allReachable = false;
			if (gameUnit->getPosition().getApproxDistance((*mineral)) < minDistance){
				minDistance = gameUnit->getPosition().getApproxDistance((*mineral));
				closestMineral = *mineral;
			}
		}
	}

	/*if (! closestMineral.isValid()){
		Broodwar << "Dummy position returned";
		return Positions::Invalid;// (-1,-1);//returns dummy position if all minerals are in range...
	}*/
	return closestMineral;

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
			Broodwar << "Agent [" << unitId << "] Takes too long to scout, removing this task" << "\n";
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
	Region myRegion = Broodwar->getRegionAt( unit->getPosition() );


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
	SCVMap colleagues = _aiModule->getSCVMap();

	//checks if someone else already decided to build the depot
	for(auto scv = colleagues.begin(); scv != colleagues.end(); scv++){
		if(scv->second->state == BUILDING_SUPPLY_DEPOT){
			state = NO_TASK;
			gameUnit->stop(); //become idle
			return;
		}
	}

	state = BUILDING_SUPPLY_DEPOT;
	UnitType supplyProviderType = gameUnit->getType().getRace().getSupplyProvider();
	if ( supplyProviderType.isBuilding() ){
		newBuildingLocation = Broodwar->getBuildLocation(supplyProviderType, gameUnit->getTilePosition());
		TilePosition targetBuildLocation = newBuildingLocation;
		if ( newBuildingLocation ){
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
	
	TilePosition cmdTilePos = TilePosition(commandCenterPos.x / TILE_SIZE, commandCenterPos.y / TILE_SIZE);
	//tries to place barrack near the command center
	newBuildingLocation = Broodwar->getBuildLocation(barrackType, cmdTilePos);//gameUnit->getTilePosition());
	//Broodwar->getb

	if ( newBuildingLocation ){
		state = BUILDING_BARRACKS;
		Broodwar->drawLineMap(gameUnit->getPosition(), Position(commandCenterPos.x, commandCenterPos.y), Color(Colors::Green));
		
		//checks if other scvs are planning to construct a barrack in my area
		SCVMap map = _aiModule->getSCVMap();
		for(auto scv = map.begin(); scv != map.end(); scv++){
			if(scv->second->gameUnit->getID() != gameUnit->getID() &&  scv->second->newBuildingLocation == newBuildingLocation){
				Broodwar << "someone will build there already" << "\n";
				newBuildingLocation = BWAPI::TilePositions::Invalid;
				state = NO_TASK;
				gameUnit->stop();
				return;
			}
		}

		// Order the builder to construct the structure, if it fails, print some info and return
		if(!gameUnit->build( barrackType, newBuildingLocation )) {
			

			if(Broodwar->getLastError().getID() == Errors::Unbuildable_Location){
				Broodwar << "Not buildable!" << "\n";
				gameUnit->move(Position(newBuildingLocation));
				//this error seems to happen when place is unexplored

				/*
				Position myPos = gameUnit->getPosition();
				Position target = Position(newBuildingLocation);
				Broodwar->registerEvent(
					[target,myPos](Game*){
						Broodwar->drawLineMap( myPos, target,	Colors::Yellow);
					},
					nullptr,  // condition
					1000 
				);  // frames to run
				*/
			}

			state = NO_TASK;
			newBuildingLocation = BWAPI::TilePositions::Invalid;
			//gameUnit->stop();
			return;
		}

		// Register an event that draws the target build location
		TilePosition targetBuildLocation = newBuildingLocation;
		Broodwar->registerEvent([targetBuildLocation,barrackType](Game*)
			{
				Broodwar->drawBoxMap( Position(targetBuildLocation),
					Position(targetBuildLocation + barrackType.tileSize()),
					Colors::Blue);
			},
			nullptr,  // condition
			barrackType.buildTime() + 100 // frames to run
		);  

		
		
	}
	else {
		Position txtPlace =  gameUnit->getPosition();
		/*Broodwar->registerEvent([txtPlace,barrackType](Game*)
		{
			Broodwar->drawTextMap( txtPlace,
					Broodwar->getLastError().c_str());
		},
		nullptr,  // condition
		barrackType.buildTime() + 100 );*/
		Broodwar << Broodwar->getLastError().c_str() << "\n";

		
		state = NO_TASK;
		gameUnit->stop();
		return;
	}
}

void SCVAgent::createBunkerNearCommandCenter(Position commandCenterPos) {
	UnitType bunkerType = UnitTypes::Terran_Bunker;
	
	TilePosition cmdTilePos = TilePosition(commandCenterPos.x / TILE_SIZE, commandCenterPos.y / TILE_SIZE);
	//tries to place barrack near the command center
	newBuildingLocation = Broodwar->getBuildLocation(bunkerType, cmdTilePos);//gameUnit->getTilePosition());
	//Broodwar->getb

	if ( newBuildingLocation ){
		state = BUILDING_BUNKER;
		Broodwar->drawLineMap(gameUnit->getPosition(), Position(commandCenterPos.x, commandCenterPos.y), Color(Colors::White));
		
		//checks if other scvs are planning to construct a bunker in my area
		SCVMap map = _aiModule->getSCVMap();
		for(auto scv = map.begin(); scv != map.end(); scv++){
			if(scv->second->gameUnit->getID() != gameUnit->getID() &&  scv->second->newBuildingLocation == newBuildingLocation){
				Broodwar << "someone will build there already" << "\n";
				newBuildingLocation = BWAPI::TilePositions::Invalid;
				state = NO_TASK;
				gameUnit->stop();
				return;
			}
		}

		// Order the builder to construct the structure, if it fails, print some info and return
		if(!gameUnit->build( bunkerType, newBuildingLocation )) {
			

			if(Broodwar->getLastError().getID() == Errors::Unbuildable_Location){
				Broodwar << "Not buildable!" << "\n";
				gameUnit->move(Position(newBuildingLocation));
				//this error seems to happen when place is unexplored

				/*
				Position myPos = gameUnit->getPosition();
				Position target = Position(newBuildingLocation);
				Broodwar->registerEvent(
					[target,myPos](Game*){
						Broodwar->drawLineMap( myPos, target,	Colors::Yellow);
					},
					nullptr,  // condition
					1000 
				);  // frames to run
				*/
			}

			state = NO_TASK;
			newBuildingLocation = BWAPI::TilePositions::Invalid;
			//gameUnit->stop();
			return;
		}

		// Register an event that draws the target build location
		TilePosition targetBuildLocation = newBuildingLocation;
		Broodwar->registerEvent([targetBuildLocation,bunkerType](Game*)
			{
				Broodwar->drawBoxMap( Position(targetBuildLocation),
					Position(targetBuildLocation + bunkerType.tileSize()),
					Colors::White);
			},
			nullptr,  // condition
			bunkerType.buildTime() + 100 // frames to run
		);  

		
		
	}
	else {
		Position txtPlace =  gameUnit->getPosition();
		/*Broodwar->registerEvent([txtPlace,barrackType](Game*)
		{
			Broodwar->drawTextMap( txtPlace,
					Broodwar->getLastError().c_str());
		},
		nullptr,  // condition
		barrackType.buildTime() + 100 );*/
		Broodwar << Broodwar->getLastError().c_str() << "\n";

		
		state = NO_TASK;
		gameUnit->stop();
		return;
	}
}
