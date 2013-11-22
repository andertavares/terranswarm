#include <iostream>
#include <cmath>
#include "ExampleAIModule.h"
#include "Task.h"
#include "PositionTask.h"
#include "CommanderAgent.h"

#define EULER 2.71828182845904523536

using namespace BWAPI;
using namespace Filter;


ExampleAIModule::ExampleAIModule() {
	//does nothing, init is done in onStart()
}

ExampleAIModule::~ExampleAIModule(){
	delete trainMarine;
	delete gatherMinerals;
	delete buildSupplyDepot;
}

void ExampleAIModule::onStart() {
	// Hello World!
	Broodwar->sendText("TerranSwarm by Anderson & Hector is online!");

	// Print the map name.
	// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;
	
  
	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	// Check if this is a replay
	if ( Broodwar->isReplay() ) {

		// Announce the players in the replay
		Broodwar << "The following players are in this replay:" << std::endl;
    
		// Iterate all the players in the game using a std:: iterator
		Playerset players = Broodwar->getPlayers();
		for(auto p = players.begin(); p != players.end(); ++p) {
		  // Only print the player if they are not an observer
		  if ( !p->isObserver() )
			Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
		}
	}
	else {// if this is not a replay
  
		Broodwar->sendText("show me the money");
		Broodwar->sendText("operation cwal");

		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		if ( Broodwar->enemy() ) // First make sure there is an enemy
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
		
		//cleans the list of command centers
		commandCenters.clear();

		//clears the list of discovered minerals
		discoveredMinerals.clear();

		//creates the tasksTypes
		//trainMarine and gatherMinerals are always present, so they're created separately
		trainMarine = new Task(TrainMarine, .8f);
		gatherMinerals = new Task(GatherMinerals, .8f);
		buildCommandCenter = new Task(BuildCommandCenter, 0);
		buildSupplyDepot = new Task(BuildSupplyDepot, 0);
		explore = new Task(Explore, 0);

		//tasks are grouped by taskType
		//insert all TaskTypes into map; all Types will point to empty list
		for(int tt = TrainMarine; tt != GuardBase; ++tt){
			allTasks[static_cast<TaskType>(tt)] = new vector<Task>;
		}

		//TaskTypes with single task instance will point to list with the instance
		//TaskTypes with multiple instances will remain pointing to empty list (will be fulfilled on demand)
		allTasks[TrainMarine]->push_back(*trainMarine); //push_front(*trainMarine);
		allTasks[GatherMinerals]->push_back(*gatherMinerals);
		allTasks[BuildSupplyDepot]->push_back(*buildSupplyDepot);
		allTasks[Explore]->push_back(*explore);
		allTasks[BuildCommandCenter]->push_back(*buildCommandCenter);

	}

	// Create the main agents
	_commanderAgent = new CommanderAgent();
	//scvMap = new unordered_map<int, SCVAgent*>;
}

void ExampleAIModule::onEnd(bool isWinner) {
	// Called when the game ends
	if ( isWinner )  {
		Broodwar->sendText("POWER OVERWHELMING!");
	}
}

void ExampleAIModule::onFrame() {
	// Called once every game frame
	// Return if the game is a replay or is paused
	if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
		return;

	_drawStats();

	// Draw bullets
	Bulletset bullets = Broodwar->getBullets();
	for(Bulletset::iterator i = bullets.begin(); i != bullets.end(); ++i){
		Position p = i->getPosition();
		double velocityX = i->getVelocityX();
		double velocityY = i->getVelocityY();
		Broodwar->drawLineMap(p, p + Position((int)velocityX, (int)velocityY), i->getPlayer() == Broodwar->self() ? Colors::Green : Colors::Red);
		//Broodwar->drawTextMap(p, "%c%s", i->getPlayer() == Broodwar->self() ? Text::Green : Text::Red, i->getType().c_str());
	}

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
		return;

	updateTasks();

	_commanderAgent->onFrame(allTasks);

	//iterates through all marines
	for(auto marine = marines.begin(); marine != marines.end(); marine++){
		marine->second->onFrame(allTasks);
	}

	// Iterate through all the SCV on the map
	int scvCounter = 0;
	unordered_map<int, SCVAgent*>::iterator it = scvMap.begin();
	for(unordered_map<int, SCVAgent*>::iterator iter = scvMap.begin(); iter != scvMap.end(); ++iter){
		scvCounter++;
		int unitId =  iter->first;
		SCVAgent* agent = iter->second;
		Unit u = agent->getUnit();

		if(unitId == 4){
			agent->goScout();
		}

		//Broodwar->drawTextMap(u->getPosition().x, u->getPosition().y, "agentId[%d]", unitId);

		if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() )
			continue;
		
		if ( u->isIdle() ) {
			// Order workers carrying a resource to return them to the center,
			// otherwise find a mineral patch to harvest.
			if ( u->isCarryingGas() || u->isCarryingMinerals() ) {
				u->returnCargo();
			}
			else if ( !u->getPowerUp() ) { // The worker cannot harvest anything if it
											 // is carrying a powerup such as a flag
					// Harvest from the nearest mineral patch or gas refinery
				if ( !u->gather( u->getClosestUnit( IsMineralField || IsRefinery )) ) {
					// If the call fails, then print the last error message
					Broodwar << Broodwar->getLastError() << std::endl;
				}

			} // closure: has no powerup
		} // closure: if idle
		
	}
	
	/*Broodwar->drawTextScreen(20, 90 + yOffset, "Number of SCV in map [%d]", 
		Text::White, scvMap->size()
	);*/

	
	// Iterate through all the units that we own
	/*Unitset myUnits = Broodwar->self()->getUnits();
	for ( Unitset::iterator u = myUnits.begin(); u != myUnits.end(); ++u ) {
		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if ( !u->exists() )
			continue;

		// Ignore the unit if it has one of the following status ailments
		if ( u->isLockedDown() || u->isMaelstrommed() || u->isStasised() )
			continue;

		// Ignore the unit if it is in one of the following states
		if ( u->isLoaded() || !u->isPowered() || u->isStuck() )
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if ( !u->isCompleted() || u->isConstructing() )
			continue;
		

		// Finally make the unit do some stuff
		//code to do distance testing
		//if(u->getType() == UnitTypes::Terran_Marine){
		//	Unit brk = u->getClosestUnit();
		//	
		//	Broodwar->drawLineMap(u->getPosition(), brk->getPosition(), Color(Colors::Cyan));
		//	Broodwar->drawTextScreen(0, 120,"Distance (m,b) = %d",u->getDistance(brk));
		//}
		
		
		// If the unit is a worker unit
		if ( u->getType().isWorker() ) {
			// if our worker is idle
			if ( u->isIdle() ) {
				// Order workers carrying a resource to return them to the center,
				// otherwise find a mineral patch to harvest.
				if ( u->isCarryingGas() || u->isCarryingMinerals() ) {
					u->returnCargo();
				}
				else if ( !u->getPowerUp() ) { // The worker cannot harvest anything if it
											 // is carrying a powerup such as a flag
					// Harvest from the nearest mineral patch or gas refinery
					if ( !u->gather( u->getClosestUnit( IsMineralField || IsRefinery )) ) {
						// If the call fails, then print the last error message
						Broodwar << Broodwar->getLastError() << std::endl;
					}

				} // closure: has no powerup
			} // closure: if idle
		}
		
	} // closure: unit iterator
	*/

}

void ExampleAIModule::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void ExampleAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if ( target )  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else 
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

//BWAPI calls this when a unit becomes accessible
void ExampleAIModule::onUnitDiscover(Unit unit){
	
	Broodwar->sendText("Unit [%s] discovered ID [%d]", unit->getType().getName().c_str(), unit->getID());

	if (unit->getPlayer() == Broodwar->self() && unit->getType() == UnitTypes::Terran_Command_Center) {
		commandCenters.insert(unit);
	}
	else if(unit->getPlayer() == Broodwar->self() && unit->getType() == UnitTypes::Terran_SCV){
		// Add SCV to map
		SCVAgent *agent = new SCVAgent(unit);
		scvMap[unit->getID()] = agent;
		//(*scvMap)[unit->getID()] = agent; When SCVmap is a pointer
	}
	else if(unit->getPlayer() == Broodwar->self() && unit->getType() == UnitTypes::Terran_Marine){
		// Add marine to HashTable
		marines[unit->getID()] = new MarineAgent(unit);
	}

	

	//new mineral discovered, is it at the range of a command center?
	//framecount test prevents checking on unacessible minerals at game begin
	if(Broodwar->getFrameCount() != 0 && unit->getType() == UnitTypes::Resource_Mineral_Field){
		discoveredMinerals.insert(unit);
	}
	
}

//BWAPI calls this right before a unit becomes inaccessible
void ExampleAIModule::onUnitEvade(BWAPI::Unit unit){
	//Broodwar->sendText("Unit [%s] evaded (became unaccessible)", unit->getType().getName().c_str());
}

/*
 * BWAPI calls this when a unit becomes visible. If Complete Map Information is disabled,
 * this also means that the unit has just become accessible
 */
void ExampleAIModule::onUnitShow(BWAPI::Unit unit){
	//Broodwar->sendText("Unit [%s] became visible", unit->getType().getName().c_str());
}

/* 
 * BWAPI calls this right before a unit becomes invisible. If Complete Map Information is disabled, 
 * this also means that the unit is about to become inaccessible
 */
void ExampleAIModule::onUnitHide(BWAPI::Unit unit){
	//Broodwar->sendText("Unit [%s] not visible anymore", unit->getType().getName().c_str());
}

//BWAPI calls this when an accessible unit is created. Note that this is NOT called when a unit changes type
void ExampleAIModule::onUnitCreate(BWAPI::Unit unit) {
	if ( Broodwar->isReplay() ) {
		// if we are in a replay, then we will print out the build order of the structures
		if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() ) {
			int seconds = Broodwar->getFrameCount()/24;
			int minutes = seconds/60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
    }
	/*else{
		if(unit->getType() == UnitTypes::Terran_Command_Center){
			Unitset visibleMinerals = Broodwar->getMinerals();
			
			//when base is created, check if it covers all accessible minerals
			bool allVisible = true;
			for(Unitset::iterator min = visibleMinerals.begin(); min != visibleMinerals.end(); ++min){
				if(unit->getDistance(*min) < BASE_RADIUS){
					//allVisible = false;
					//break;
					mineralsOutOfBaseRange--;
				}
			}
		}
	}*/
}

//BWAPI calls this when a unit dies or otherwise removed from the game (i.e. a mined out mineral patch)
void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit){
	if (unit->getPlayer() == Broodwar->self()){
		Broodwar->sendText("%s lost.", unit->getType().getName().c_str());
		if(unit->getType() == UnitTypes::Terran_SCV){
			// Delete this SCV from the map
			scvMap.erase(unit->getID()); 
		}
		else if(unit->getType() == UnitTypes::Terran_Marine){
			marines.erase(unit->getID());
		}
	}
	else {
		Broodwar->sendText("%s shot down.", unit->getType().getName().c_str());
	}
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit) {
  if ( Broodwar->isReplay() ) {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() ) {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit){
}

void ExampleAIModule::onSaveGame(std::string gameName){
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}


//this is being called at the same time as onUnitCreate...
void ExampleAIModule::onUnitComplete(BWAPI::Unit unit) {
	//BWAPI::UnitType unitType = unit->getType();
	//Broodwar->sendText("New unit [%s] complete! ", unitType.getName().c_str());
}


void ExampleAIModule::updateTasks(){
	updateAttack();
	updateBuildSupplyDepot();
	updateBuildBarracks();
	updateBuildCommandCenter();
	updateTrainSCV();
	updateExplore();
}

/**
  * Create new Tasks for each enemy units not covered by attack tasks
  * Cleans up attack tasks whose targets are not in position anymore
  * AttackTasks are added at onUnitDiscover()
  */
void ExampleAIModule::updateAttack(){
	//traverse the task list to check if positions are visible and still have enemies
	vector<Task>* preserved = new vector<Task>(); //stores the tasks that should not be removed

	for(auto task = allTasks[Attack]->begin(); task != allTasks[Attack]->end(); task++){

		if(Broodwar->isVisible(task->getPosition().x / TILE_SIZE , task->getPosition().y / TILE_SIZE)){
			Unitset inRange = Broodwar->getUnitsInRadius(task->getPosition(), 6*TILE_SIZE, Filter::IsEnemy);
			//Broodwar->sendText("%d in range of attack task.", inRange.size());
			if (inRange.size() == 0) {
				Broodwar->sendText("Attack task removed");
				//toDelete.push_back(*task);
			}
			else {
				preserved->push_back(*task);
			}

		}
		else { //keeps invisible attack targets, so that they can be investigated
			preserved->push_back(*task);
		}
	}
	

	//brings preserved tasks to the main task vector
	allTasks[Attack]->swap(*preserved);
	delete preserved; //hope this doesn't invalidates tasks...
	//in this point, tasks whose targets are not visible were removed

	// --- Now, adds new tasks for enemy units not covered by existent attack tasks ---

	//obtains a list with all enemies from all players
	Playerset foes = Broodwar->enemies();
	Unitset enemyUnits;
	enemyUnits.clear();

	for(auto foe = foes.begin(); foe != foes.end(); ++foe){
		enemyUnits += foe->getUnits();
	}

	//Broodwar->drawTextScreen(250,45, "#foes: %d", enemyUnits.size());

	//adds a task with a position for every enemy unit in the task list
	for(auto foeUnit = enemyUnits.begin(); foeUnit != enemyUnits.end(); ++foeUnit) {
		bool inRange = false;

		//tests if unit is already included in the area of another 'attack' task
		for(auto task = allTasks[Attack]->begin(); task != allTasks[Attack]->end(); task++){
			//task->

			//PositionTask* atk = static_cast<PositionTask* >( &(*task)) ; 
			if(foeUnit->getDistance(task->getPosition()) < 6*TILE_SIZE){
				inRange = true;
				break;
			}
		}

		if(! inRange){
			Task* atk = new Task(Attack, .8f, foeUnit->getPosition());
			allTasks[Attack]->push_back(*atk);
			//Broodwar->sendText("Attack task added, pos=%d,%d // %d,%d ", unit->getPosition().x, unit->getPosition().y, atk->getPosition().x, atk->getPosition().y);
		}
	}

	/*
	//if unit is enemy, adds it to 'attack' task list, if it isn't in range of an attack task
	//it seems that it works only for buildings... must check for mobile enemy units
	if(unit->getPlayer() != Broodwar->self() && unit->getPlayer() != Broodwar->neutral()){ 
		bool inRange = false;

		//tests if unit is already included in the area of another 'attack' task
		for(auto task = allTasks[Attack]->begin(); task != allTasks[Attack]->end(); task++){
			//task->

			//PositionTask* atk = static_cast<PositionTask* >( &(*task)) ; 
			if(unit->getDistance(task->getPosition()) < 6*TILE_SIZE){
				inRange = true;
				break;
			}
		}

		if(! inRange){
			Task* atk = new Task(Attack, .8f, unit->getPosition());
			allTasks[Attack]->push_back(*atk);
			Broodwar->sendText("Attack task added, pos=%d,%d // %d,%d ", unit->getPosition().x, unit->getPosition().y, atk->getPosition().x, atk->getPosition().y);

			for(auto task = allTasks[Attack]->begin(); task != allTasks[Attack]->end(); task++){
				//PositionTask* atk = static_cast<PositionTask* >( &(*task)) ; 
				Broodwar->sendText("pos=%d,%d", task->getPosition().x, task->getPosition().y);
			}
		}

	}
	*/
}

void ExampleAIModule::updateTrainSCV(){

	for(Unitset::iterator cmd = commandCenters.begin(); cmd != commandCenters.end(); ++cmd){	
		
		Unitset mineralsAround = Broodwar->getUnitsInRadius(cmd->getPosition(), BASE_RADIUS, Filter::IsMineralField);
		Unitset scvAround = Broodwar->getUnitsInRadius(cmd->getPosition(), BASE_RADIUS, Filter::IsWorker && Filter::IsOwned);

		trainSCVIncentives[*cmd] = 1.0f - (scvAround.size() / (3.0f * mineralsAround.size()));

	}
}

void ExampleAIModule::updateBuildCommandCenter(){

	if(buildCommandCenter->getIncentive() > 0) {
		scvMap[3]->buildCommandCenter(discoveredMinerals, commandCenters);
	}


	//for every discovered mineral, check if it is in range of a command center
	for(Unitset::iterator mineral = discoveredMinerals.begin(); mineral != discoveredMinerals.end(); ++mineral){
		bool reachable = false;
		for(Unitset::iterator cmd = commandCenters.begin(); cmd != commandCenters.end(); ++cmd){
			if (cmd->getDistance(mineral->getPosition()) < BASE_RADIUS){
				reachable = true;
				break;
			}
		}
		if (!reachable){
			buildCommandCenter->setIncentive(0.8f);
			return;
		}
	}

	buildCommandCenter->setIncentive(0);

}


void ExampleAIModule::updateBuildBarracks(){
	//calculates the number of barracks around the command center
	for (Unitset::iterator c = commandCenters.begin(); c != commandCenters.end(); c++){
		int barracksNumber = calculateBarracksFromCommandCenter(Broodwar->getUnit(c->getID()));

		//updates the number of barracks around all command centers
		builtBarracks[*c] = barracksNumber;
		buildBarracksIncentives[*c] = 1.0f - barracksNumber/4.0f;
		//Broodwar->sendText("emplacing %d // %f", barracksNumber, 1.0f - barracksNumber/4.0f);
		//Broodwar->sendText("emplaced %d // %f", builtBarracks[*c], buildBarracksIncentives[*c]);
		//TODO: call createBarrackNearCommandCenter using SwarmGAP rules

		//Broodwar->drawTextScreen(437,27,"Barracks near command center [%d]", barracksNumber);
		if(barracksNumber < 4){
			//Broodwar->sendText("Creating new barrack");
			createBarrackNearCommandCenter(Broodwar->getUnit(c->getID()));
		}
	}
}

void ExampleAIModule::updateBuildSupplyDepot(){
	//updates the buildSupplyDepots incentive
	int dif = max(0, (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed())/2); //bwapi returns 2*the actual difference...
	//if (dif < 0) dif = 0;
	//incentive is maximum when difference is minimum
	//buildSupplyDepot->setIncentive(pow(EULER,-dif));

	//buildSupplyDepot->setIncentive(pow(EULER,-dif));
	buildSupplyDepot->setIncentive(1.0f - (dif/10.0f)); //linear 'decay'

	//finds a command center to draw a debug text
	Unitset myUnits = Broodwar->self()->getUnits();
	Unitset::iterator u = NULL;
	for ( u = myUnits.begin(); u != myUnits.end(); ++u ) {
		if ( u->getType().isResourceDepot() ) {
			break;
		}
	}
}

void ExampleAIModule::updateExplore(){
	int exploredTiles = 0;
	int width = Broodwar->mapWidth();
	int height = Broodwar->mapHeight();

	//check which tiles were explorated
	for (int hTile = 0; hTile < width; hTile++){ 
		for (int vTile = 0; vTile < height ; vTile++){
			if (Broodwar->isExplored(hTile,vTile)){
				exploredTiles++;
				Broodwar->drawDotMap(hTile*TILE_SIZE+16,vTile*TILE_SIZE+16,Colors::Green);
			}
			else{
				Broodwar->drawDotMap(hTile*TILE_SIZE+16,vTile*TILE_SIZE+16,Colors::Red);
			}
		}
	}
	//explored incentive is % of map unrevealed; if game is more than 5 minutes (doesn't scout too early)
	if (Broodwar->getFrameCount()/24 > 300) {
		explore->setIncentive(1.0f - float(exploredTiles) / (width * height));
	}
    //int minutes = seconds/60;
}

int ExampleAIModule::calculateBarracksFromCommandCenter(Unit cmdCenter) {
	//if(!u->getType().isResourceDepot()){
	//	return 0;
	//}

	Position commandCenterPos = cmdCenter->getPosition();
	Unitset units = Broodwar->getUnitsInRadius(commandCenterPos, BASE_RADIUS);
	//Broodwar->drawCircleMap(commandCenterPos, 20*TILE_SIZE, Color(Colors::Blue));
	//Unitset units = u->getUnitsInRadius(200);
	int counter = 0;
	for ( Unitset::iterator u = units.begin(); u != units.end(); ++u ) {
		if ( u->getType() == UnitTypes::Terran_Barracks ) {
			counter++;
		} 
	}

	return counter;
}

void ExampleAIModule::createBarrackNearCommandCenter(Unit u) {
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


void ExampleAIModule::_drawStats(){
	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(250, 0,  "FPS: %d", Broodwar->getFPS() );
	Broodwar->drawTextScreen(250, 15, "Average FPS: %f", Broodwar->getAverageFPS() );
	Broodwar->drawTextScreen(250, 30, "Frame count: %d", Broodwar->getFrameCount() );

	// display some debug info...
	Broodwar->drawTextScreen(20, 0, "%cSupply Depot incentive = %.3f", 
		Text::White, 
		buildSupplyDepot->getIncentive()
	); 
	Broodwar->drawTextScreen(20, 15, "%cExplore incentive = %.3f", 
		Text::White, 
		explore->getIncentive()
	);

	Broodwar->drawTextScreen(20, 30, "%c#Marines: %d // #atk tasks: %d // Train incentive = %.3f", 
		Text::White, 
		marines.size(),
		allTasks[Attack]->size(),
		trainMarine->getIncentive()
	);

	Broodwar->drawTextScreen(20, 45, "%c#SCVs: %d // Mine incentive = %.3f", 
		Text::White, 
		scvMap.size(),
		gatherMinerals->getIncentive()
	);

	Broodwar->drawTextScreen(20, 60, "%cBuild CMD center incentive: %.3f", 
		Text::White, 
		buildCommandCenter->getIncentive()
	);

	Broodwar->drawTextScreen(20,75, "%c#Brk // Brk inc. // SCV inc:", 
		Text::White 
	);

	int yOffset = 0;
	for (Unitset::iterator cmd = commandCenters.begin(); cmd != commandCenters.end(); ++cmd){
		Broodwar->drawTextScreen(20,90 + yOffset, "%c%d // %.2f // %.3f", 
			Text::White, builtBarracks[*cmd], buildBarracksIncentives[*cmd], trainSCVIncentives[*cmd]
		);
		yOffset += 15;
	}

	Broodwar->drawTextScreen(20, 90 + yOffset, "Number of SCV in map [%d]", 
		scvMap.size()
	);

	//draws the command center 'radius'
	for (Unitset::iterator c = commandCenters.begin(); c != commandCenters.end(); ++c){
		Position commandCenterPos = c->getPosition();
		//Unitset units = Broodwar->getUnitsInRadius(commandCenterPos, BASE_RADIUS*TILE_SIZE);
		Broodwar->drawCircleMap(commandCenterPos, BASE_RADIUS, Color(Colors::Blue));
	}

	//draws a circle around the minerals
	Unitset minerals = Broodwar->getMinerals();
	for (Unitset::iterator m = minerals.begin(); m != minerals.end(); ++m){
		if (m->getType().isMineralField()) {
			Broodwar->drawCircleMap(m->getPosition(), m->getType().dimensionLeft(),Color(Colors::Blue));
		}
	}

	//writes info under SCVS
	for(auto s = scvMap.begin(); s != scvMap.end(); s++){
		Broodwar->drawTextMap(s->second->getUnit()->getPosition(), "ID[%d]", s->second->unitId);
	}

	//writes debug info under marines
	/*for(auto m = marines.begin(); m != marines.end(); ++m){
		MarineAgent* mar = m->second;
		Broodwar->drawTextMap(mar->gameUnit->getPosition(),"%d,%d", mar->gameUnit->getPosition().x, mar->gameUnit->getPosition().y);
	}*/

	//draws circles around Attack targets
	for(auto task = allTasks[Attack]->begin(); task != allTasks[Attack]->end(); task++){
		Broodwar->drawCircleMap(task->getPosition(), 6*TILE_SIZE, Color(Colors::Red));
	}
}