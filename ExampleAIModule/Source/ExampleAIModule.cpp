#include <iostream>
#include <cmath>
#define EULER 2.71828182845904523536
#include "ExampleAIModule.h"
#include "Task.h"

using namespace BWAPI;
using namespace Filter;


ExampleAIModule::ExampleAIModule(){
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
	
		//creates the tasksTypes
		//trainMarine and gatherMinerals are always present, so they're created separately
		trainMarine = new Task(TrainMarine, .8f);
		gatherMinerals = new Task(GatherMinerals, .8f);
		buildSupplyDepot = new Task(BuildSupplyDepot, 0);
		explore = new Task(Explore, 0);

		//other tasks are grouped by taskType; multiple occurrences may appear, so they're stored on lists
		for(int tt = TrainMarine; tt != GuardBase; ++tt){
			if (tt == TrainMarine || tt == GatherMinerals || tt == BuildSupplyDepot || tt==Explore) {
				continue;
			}
			otherTasks.insert(make_pair(static_cast<TaskType>(tt), new list<Task>));
		}
	}

	// Create the main agents
	_commanderAgent = new CommanderAgent();
}

void ExampleAIModule::onEnd(bool isWinner)
{
  // Called when the game ends
  if ( isWinner )
  {
    // Log your win here!
  }
}

void ExampleAIModule::onFrame() {
	// Called once every game frame
	// Return if the game is a replay or is paused
	if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
		return;

	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );

	// display some debug info...
	Broodwar->drawTextScreen(20,0, "%cSupply Depot incentive = %.3f", 
		Text::White, 
		buildSupplyDepot->getIncentive()
	); 
	Broodwar->drawTextScreen(20,15, "%cExplore incentive = %.3f", 
		Text::White, 
		explore->getIncentive()
	);

	Broodwar->drawTextScreen(20,30, "%cMap Size: %d x %d", 
		Text::White, 
		Broodwar->mapWidth(),
		Broodwar->mapHeight()
	);

	//draws a circle around the minerals
	Unitset minerals = Broodwar->getMinerals();
	//Broodwar->sendText("#minerals: %d", minerals.size());
	for (Unitset::iterator m = minerals.begin(); m != minerals.end(); ++m){
		if (m->getType().isMineralField()) {
			//Broodwar->sendText( "Drawing circle...");
			Broodwar->drawCircleMap(m->getPosition(), m->getType().dimensionLeft(),Color(Colors::Blue));
		}
	}

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
		return;

	_commanderAgent->onFrame();

	updateTasks();

	// Iterate through all the units that we own
	Unitset myUnits = Broodwar->self()->getUnits();
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

		// If the unit is a worker unit
		if ( u->getType().isWorker() ) {
			// if our worker is idle
			if ( u->isIdle() ) {
				// Order workers carrying a resource to return them to the center,
				// otherwise find a mineral patch to harvest.
				if ( u->isCarryingGas() || u->isCarryingMinerals() )
				{
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
  if ( target )
  {
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

void ExampleAIModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit){
	BWAPI::UnitType unitType = unit->getType();
	
	if(unitType == UnitTypes::Terran_Marine){
		Broodwar->sendText("Marine down.");
	}
	if(unitType == UnitTypes::Terran_SCV){
		Broodwar->sendText("SCV down.");
	}
	if(unitType == UnitTypes::Terran_Barracks){
		Broodwar->sendText("Barracks destroyed!");
	}
	if(unitType == UnitTypes::Terran_Supply_Depot){
		Broodwar->sendText("Supply Depot destroyed!");
	}
	if(unitType == UnitTypes::Terran_Command_Center){
		Broodwar->sendText("Command Center destroyed!");
	}
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void ExampleAIModule::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit unit)
{
	BWAPI::UnitType unitType = unit->getType();
	Broodwar->sendText("New unit [%s] created ", unitType.getName().c_str());
}


void ExampleAIModule::updateTasks(){
	updateBuildSupplyDepot();
	updateExplore();

}

void ExampleAIModule::updateBuildSupplyDepot(){
	//updates the buildSupplyDepots incentive
	int dif = (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed())/2; //bwapi returns 2*the actual difference...
	if (dif < 0) dif = 0;
	//incentive is maximum when difference is minimum
	//buildSupplyDepot->setIncentive(pow(EULER,-dif));

	//buildSupplyDepot->setIncentive(pow(EULER,-dif));
	buildSupplyDepot->setIncentive(1.0 -(dif/10.0)); //linear 'decay'

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

	for (int hTile = 0; hTile < width; hTile++){ //*4 to get size in Walk Tiles
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

	//explored incentive is % of map unrevealed
	explore->setIncentive(1.0f - float(exploredTiles) / (width * height));
}