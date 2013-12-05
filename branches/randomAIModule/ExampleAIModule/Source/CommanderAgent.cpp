#include "CommanderAgent.h"
#include <BWAPI.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "Task.h"
#include "TaskAssociation.h"
#include "RandomAIModule.h"
#include "util.h"
#include <random>
#include <iostream>

using namespace BWAPI;
using namespace Filter;

CommanderAgent::CommanderAgent() : latencyFrames(10){
	_barracks.clear();
	_commandCenters.clear();
}


CommanderAgent::~CommanderAgent(){

}

void CommanderAgent::onFrame(unordered_map<TaskType, vector<Task>*> tasklist, unordered_map<Unit, float> trainSCVIncentives, Unitset commandCenters) {
	//only acts every 'X' frames (X = latencyFrames)
	if (Broodwar->getFrameCount() % latencyFrames != 0){
		return;
	}

	//also does nothing if there is no supply or minerals available
	int supplyDiff = max(0, (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed())/2);
	if (supplyDiff == 0 || Broodwar->self()->minerals() < 50) return;

	//builds a list with all the tasks
	Task* toPerform;
	vector<Task*> all;
	for(auto taskIter = tasklist.begin(); taskIter != tasklist.end(); ++taskIter){
		if (taskIter->first == TrainMarine || taskIter->first == TrainWorker){
			for (auto task = taskIter->second->begin(); task != taskIter->second->end(); task++){
				all.push_back(&(*task));
			}
		}
	}


	for(Unitset::iterator cmd = commandCenters.begin(); cmd != commandCenters.end(); ++cmd){	
		
		Unitset mineralsAround = Broodwar->getUnitsInRadius(cmd->getPosition(), BASE_RADIUS, Filter::IsMineralField);
		Unitset scvAround = Broodwar->getUnitsInRadius(cmd->getPosition(), BASE_RADIUS, Filter::IsWorker && Filter::IsOwned);

		float incentive = max(0.0f, 1.0f - (scvAround.size() / (2.5f * mineralsAround.size())));
		
		if(incentive > 0) {
			all.push_back(new Task(TrainWorker, incentive, cmd->getPosition()));
		}

	}

	/*
	for(unordered_map<Unit, float>::iterator iter = trainSCVIncentives.begin(); iter != trainSCVIncentives.end(); ++iter){
		Unit u =  iter->first;
		
		if(!u || !u->exists()) continue;
		//Broodwar << u << endl;
		u->getPosition();

		float incentive = iter->second;
		//double uniformOn01 = dis(gen);
		
	}
	*/
	int index = randomInRange(0, all.size());
	toPerform = all[index];
	all.clear();
	//sanity check, does not perform tasks that cannot/should not be done
	if (toPerform->getIncentive() <= 0) {
		return;
	}

	if(toPerform->getTaskType() == TrainWorker){
		Unitset providers = Broodwar->getUnitsOnTile(TilePosition(toPerform->getPosition()), Filter::IsResourceDepot);

		if(providers.size() <= 0){
			Broodwar << "Error, no CMD center in task position" << endl;
		}

		Unit u = providers[0];

		//if(uniformOn01 <= incentive){
		if ( u->getType().isResourceDepot() ) {
			// Order the depot to construct more workers! But only when it is idle.
			if ( u->isIdle() && !u->train(u->getType().getRace().getWorker()) ) {
				Error lastErr = Broodwar->getLastError();
				if(lastErr == Errors::Insufficient_Supply){
					//Broodwar->sendText("SVC can't be created - %s", lastErr.toString().c_str());	
					//CommanderAgent::createSupply(Broodwar->getUnit(u->getID()));
				}
			} // closure: failed to train idle unit
		}
	}


	if(toPerform->getTaskType() == TrainMarine){
	//iterates through the barracks, finds one idle and trains the marine in it
		Unitset myUnits = Broodwar->self()->getUnits();
		for ( Unitset::iterator u = myUnits.begin(); u != myUnits.end(); ++u ) {
			if ( u->getType() == UnitTypes::Terran_Barracks ) {
			
				if (u->isIdle()){
					if( !u->train(UnitTypes::Terran_Marine)) {
						Error lastErr = Broodwar->getLastError();
						if(lastErr == Errors::Insufficient_Supply){
							//Broodwar->sendText("Marine can't be created - %s", lastErr.toString().c_str());	
							//CommanderAgent::createSupply(Broodwar->getUnit(u->getID()));
						}
					}
					return;
				}
			} //closure
		}
	}
}

void CommanderAgent::createSupply(Unit u){
	Position pos = u->getPosition();
	Error lastErr = Broodwar->getLastError();
	Broodwar->registerEvent([pos, lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
		nullptr,    // condition
		Broodwar->getLatencyFrames());
	
	UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
	static int lastChecked = 0;

	// If we are supply blocked and haven't tried constructing more recently
	if (  lastErr == Errors::Insufficient_Supply && lastChecked + 400 < Broodwar->getFrameCount() && Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0 ) {

			lastChecked = Broodwar->getFrameCount();

			// Retrieve a unit that is capable of constructing the supply needed
			Unit supplyBuilder = u->getClosestUnit(  GetType == supplyProviderType.whatBuilds().first &&
				(IsIdle || IsGatheringMinerals) &&
				IsOwned);
			// If a unit was found
			if ( supplyBuilder ){

				if ( supplyProviderType.isBuilding() ){

					TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
					if ( targetBuildLocation ){

						// Register an event that draws the target build location
						Broodwar->registerEvent([targetBuildLocation,supplyProviderType](Game*)
						{
							Broodwar->drawBoxMap( Position(targetBuildLocation),
								Position(targetBuildLocation + supplyProviderType.tileSize()),
								Colors::Blue);
						},
							nullptr,  // condition
							supplyProviderType.buildTime() + 100 );  // frames to run

						// Order the builder to construct the supply structure
						supplyBuilder->build( supplyProviderType, targetBuildLocation );
					}
				}
				else {
					// Train the supply provider (Overlord) if the provider is not a structure
					supplyBuilder->train( supplyProviderType );
				}
			} // closure: supplyBuilder is valid
	} // closure: insufficient supply
}

