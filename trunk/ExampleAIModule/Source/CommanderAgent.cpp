#include "CommanderAgent.h"
#include <BWAPI.h>
#include <iostream>

using namespace BWAPI;
using namespace Filter;

CommanderAgent::CommanderAgent(void)
	//: _barracks(0),
	//_commandCenters(0)
{
	_barracks.clear();
	_commandCenters.clear();
}


CommanderAgent::~CommanderAgent(void)
{
}

void CommanderAgent::onFrame(void)
{
	// TODO : Use probabilistic approach to this 
	Unitset myUnits = Broodwar->self()->getUnits();
	for ( Unitset::iterator u = myUnits.begin(); u != myUnits.end(); ++u ) {
		if ( u->getType().isResourceDepot() ) {
			// Order the depot to construct more workers! But only when it is idle.
			if ( u->isIdle() && !u->train(u->getType().getRace().getWorker()) ) {
				Error lastErr = Broodwar->getLastError();
				if(lastErr == Errors::Insufficient_Supply){
					Broodwar->sendText("SVC cant be created %s", lastErr.toString());	
					CommanderAgent::createSupply(Broodwar->getUnit(u->getID()));
				}
			} // closure: failed to train idle unit


			// Calculate the barracks around the command center
			int barracksNumber = calculateBarracksFromCommandCenter(Broodwar->getUnit(u->getID()));
			Broodwar->drawTextScreen(437,27,"Barracks near command center [%d]", barracksNumber);
			if(barracksNumber < 4){
				Broodwar->sendText("Creating new barrack");
				createBarrackNearCommandCenter(Broodwar->getUnit(u->getID()));
			}

		} //closure: isResourceDepot
		else if ( u->getType() == UnitTypes::Terran_Barracks ) {
			if ( u->isIdle() && !u->train(UnitTypes::Terran_Marine)) {
				Error lastErr = Broodwar->getLastError();
				if(lastErr == Errors::Insufficient_Supply){
					Broodwar->sendText("Marine cant be created %s", lastErr.toString());	
					CommanderAgent::createSupply(Broodwar->getUnit(u->getID()));
				}			
			}
		} //clousure: 
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

int CommanderAgent::calculateBarracksFromCommandCenter(Unit u){
	//if(!u->getType().isResourceDepot()){
	//	return 0;
	//}

	Position commandCenterPos = u->getPosition();
	Unitset units = Broodwar->getUnitsInRadius(commandCenterPos, 150*TILE_SIZE);
	//Unitset units = u->getUnitsInRadius(200);
	int counter = 0;
	for ( Unitset::iterator u = units.begin(); u != units.end(); ++u ) {
		if ( u->getType() == UnitTypes::Terran_Barracks ) {
			counter++;
		} //clousure:
	}

	return counter;
}

void CommanderAgent::createBarrackNearCommandCenter(Unit u){
	Position pos = u->getPosition();
	
	UnitType barrackType = UnitTypes::Terran_Barracks;
	static int lastChecked = 0;

	// If we are supply blocked and haven't tried constructing more recently
	if ( lastChecked + 300 < Broodwar->getFrameCount() && Broodwar->self()->incompleteUnitCount(barrackType) == 0 ) {

			lastChecked = Broodwar->getFrameCount();

			// Retrieve a unit that is capable of constructing the supply needed
			Unit supplyBuilder = u->getClosestUnit(  GetType == barrackType.whatBuilds().first && (IsIdle || IsGatheringMinerals) && IsOwned);
			// If a unit was found
			if ( supplyBuilder ){

				if ( barrackType.isBuilding() ){

					TilePosition targetBuildLocation = Broodwar->getBuildLocation(barrackType, supplyBuilder->getTilePosition());
					if ( targetBuildLocation ){

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
						supplyBuilder->build( barrackType, targetBuildLocation );
					}
				}
				
			} // closure: supplyBuilder is valid
	} // closure: insufficient supply
}