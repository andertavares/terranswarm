#pragma once

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include "ExampleAIModule.h"
#include "Task.h"
#include "CommanderAgent.h"

#define EULER 2.71828182845904523536

using namespace BWAPI;
using namespace Filter;


ExampleAIModule::ExampleAIModule() {
	//init of game structure is done in onStart()
	timeOver = false;
	lastScan = 0;
	trainMarine = NULL;
	gatherMinerals = NULL;
	buildSupplyDepot = NULL;
	ourComSat = NULL;
	srand(time(0));
}

ExampleAIModule::~ExampleAIModule(){
	delete trainMarine;
	delete gatherMinerals;
	delete buildSupplyDepot;
}

void ExampleAIModule::onStart() {
	// Hello World!
	Broodwar->sendText("RandomMedic is online!");

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

	Broodwar->setGUI(false); //disables gui drawing (better performance)
	Broodwar->setLocalSpeed(0); //fastest speed, rock on!

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
  
		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		if ( Broodwar->enemy() ) // First make sure there is an enemy
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
		
		//cleans the list of command centers
		commandCenters.clear();

		//clears the list of discovered minerals
		discoveredMineralPositions.clear();
		
		//initializes the map of task lists
		//tasks are grouped by taskType
		//insert all TaskTypes into map; all Types will point to empty list
		for(int tt = TrainMarine; tt <= BuildComSat; tt++){
			allTasks[static_cast<TaskType>(tt)] = new vector<Task>;
		}

		//TaskTypes with single task instance will point to list with the instance
		//TaskTypes with multiple instances will remain pointing to empty list (will be fulfilled on demand)
		allTasks[TrainMarine]->push_back(Task(TrainMarine, .8f)); 
		allTasks[GatherMinerals]->push_back(Task(GatherMinerals, .6f));
		allTasks[BuildSupplyDepot]->push_back(Task(BuildSupplyDepot, .0f));
		allTasks[Explore]->push_back(Task(Explore, .0f));
		allTasks[BuildCommandCenter]->push_back(Task(BuildCommandCenter, .0f));
		allTasks[BuildAcademy]->push_back(Task(BuildAcademy, .0f));

		// Vespene Gas
		allTasks[BuildVespeneGas]->push_back(Task(BuildVespeneGas, .0f));
		allTasks[TrainMedic]->push_back(Task(TrainMedic, .8f)); 
		
		// Upgrades
		allTasks[ResearchAcademyLongRange]->push_back(Task(ResearchAcademyLongRange, .0f)); 
		allTasks[ResearchAcademyStimPack]->push_back(Task(ResearchAcademyStimPack, .0f)); 

		// New Buildings EXPERIMENTAL
		allTasks[BuildBunker]->push_back(Task(BuildBunker, .0f)); 

		//retrieves the single-instance tasks and stores them in pointers for easier remembering
		trainMarine = &allTasks[TrainMarine]->at(0);
		gatherMinerals = &allTasks[GatherMinerals]->at(0);
		buildCommandCenter = &allTasks[BuildCommandCenter]->at(0);
		buildSupplyDepot = &allTasks[BuildSupplyDepot]->at(0);
		explore = &allTasks[Explore]->at(0);
		
		// single instance -- vespene Gas
		buildVespeneGas = &allTasks[BuildVespeneGas]->at(0);
		buildAcademy = &allTasks[BuildAcademy]->at(0);
		trainMedic = &allTasks[TrainMedic]->at(0);

		// single instance -- upgrades
		researchAcademyLongRange = &allTasks[ResearchAcademyLongRange]->at(0);
		researchAcademyStimpack = &allTasks[ResearchAcademyStimPack]->at(0);

		buildBunker = &allTasks[BuildBunker]->at(0);
	}

	// Create the main agents
	_commanderAgent = new CommanderAgent();
	//scvMap = new unordered_map<int, SCVAgent*>;
}

void ExampleAIModule::onEnd(bool isWinner) {
	// Called when the game ends
	if (Broodwar->isReplay()){
		return;
	}
	if (isWinner)  {
		Broodwar->sendText("POWER OVERWHELMING!");
	}

	string gameResult = "win";
	if (! isWinner){
		gameResult = (timeOver ? "draw" : "loss");
	}

	Player me = Broodwar->self();
	Player enemy = Broodwar->enemy();
	//Broodwar->
	//opens a file and writes the results
	//file layout is: Map Duration Win? Units Structure Resources EnemyRace Units Structure Resources (enemy)
	ofstream resultFile;
	Broodwar->enableFlag(Flag::CompleteMapInformation);
	resultFile.open ("results.txt", std::ios_base::app);
	resultFile << "RM," << Broodwar->mapName() << "," << Broodwar->elapsedTime() << "," << gameResult << ",";
	resultFile << me->getUnitScore() << "," << me->getBuildingScore() << "," << me->gatheredMinerals() + me->gatheredGas() << ",";
	resultFile << enemy->getRace().getName() << "," << enemy->getUnitScore() << "," << enemy->getBuildingScore() << "," << enemy->gatheredMinerals() + enemy->gatheredGas() << endl;
	resultFile.close();
	Broodwar << "Results written" << endl;
}

void ExampleAIModule::onFrame() {
	// Called once every game frame
	// Return if the game is paused
	if ( Broodwar->isPaused() )// || !Broodwar->self() )
		return;

	if (Broodwar->isReplay()){

		Playerset plrs = Broodwar->getPlayers();
		Playerset::iterator plr;

		Broodwar->drawTextScreen(290, 20, "Time: ~ %dh%dm%ds", Broodwar->elapsedTime() / 3600, Broodwar->elapsedTime() / 60, Broodwar->elapsedTime() % 60);
		
		int pCount = 1;
		for (plr = plrs.begin(); plr != plrs.end(); plr++, pCount++){
			Broodwar->drawTextScreen(290, 20 + (20 * pCount), "Score p%d - Unit, Building, Resource = %d, %d, %d ", pCount, plr->getUnitScore(), plr->getBuildingScore(), plr->gatheredMinerals() + plr->gatheredGas());
		}
		return;
	}

	//checks if game is lasting too long. If so, ends the game as a draw
	if(Broodwar->elapsedTime() >= 3600){ //1h of duration
		timeOver = true;
		Broodwar->leaveGame();
	}

	_drawStats();

	// Draw bullets
	/*Bulletset bullets = Broodwar->getBullets();
	for(Bulletset::iterator i = bullets.begin(); i != bullets.end(); ++i){
		Position p = i->getPosition();
		double velocityX = i->getVelocityX();
		double velocityY = i->getVelocityY();
		Broodwar->drawLineMap(p, p + Position((int)velocityX, (int)velocityY), i->getPlayer() == Broodwar->self() ? Colors::Green : Colors::Red);
		//Broodwar->drawTextMap(p, "%c%s", i->getPlayer() == Broodwar->self() ? Text::Green : Text::Red, i->getType().c_str());
	}*/

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
		return;

	// reinsert tasks to prevent errors
	if(allTasks[BuildCommandCenter]->size() <= 0){
		allTasks[BuildCommandCenter]->push_back(Task(BuildCommandCenter, 0));
		buildCommandCenter = &allTasks[BuildCommandCenter]->at(0);
	}
	if(allTasks[BuildSupplyDepot]->size() <= 0){
		allTasks[BuildSupplyDepot]->push_back(Task(BuildSupplyDepot, 0));
		buildSupplyDepot = &allTasks[BuildSupplyDepot]->at(0);
	}
	if(allTasks[Explore]->size() <= 0){
		allTasks[Explore]->push_back(Task(Explore, 0));
		explore = &allTasks[Explore]->at(0);
	}
	// Vespene Gas
	if(allTasks[BuildVespeneGas]->size() <= 0){
		allTasks[BuildVespeneGas]->push_back(Task(BuildVespeneGas, 0));
		buildVespeneGas = &allTasks[BuildVespeneGas]->at(0);
	}
	// Academy
	if(allTasks[BuildAcademy]->size() <= 0){
		allTasks[BuildAcademy]->push_back(Task(BuildAcademy, 0));
		buildAcademy = &allTasks[BuildAcademy]->at(0);
	}
	// Upgrades
	if(allTasks[ResearchAcademyLongRange]->size() <= 0){
		allTasks[ResearchAcademyLongRange]->push_back(Task(ResearchAcademyLongRange, 0));
		researchAcademyLongRange = &allTasks[ResearchAcademyLongRange]->at(0);
	}
	
	//sets all rally points of barracks to the nearest command center
	for(auto c = commandCenters.begin(); c != commandCenters.end(); c++){
		Unitset brk = c->getUnitsInRadius(BASE_RADIUS, Filter::IsOwned);

		for (auto b = brk.begin(); b != brk.end(); b++){
			if(b->getType() != UnitTypes::Terran_Barracks || !b->isCompleted()){
				continue;
			}
			b->setRallyPoint(*c);
		}

	}

	//gives up the construction of buildings about to be destroyed
	Unitset all = Broodwar->self()->getUnits();
	for(auto b = all.begin(); b != all.end(); b++){
		//cancels construction if building is incomplete, is under attack and its hit points are low
		if (b->getType().isBuilding() && !b->isCompleted() && b->isUnderAttack() && b->getHitPoints() < .15f * b->getType().maxHitPoints()){
			b->cancelConstruction();
			//Broodwar << "construction canceled" << endl;
		}
	}

	//builds com-sat if we have more than 15 minutes of game played
	if(Broodwar->elapsedTime() / 60 >= 15){
		if(ourComSat == NULL && 
			Broodwar->self()->minerals() > UnitTypes::Terran_Comsat_Station.mineralPrice() &&
			Broodwar->self()->gas() > UnitTypes::Terran_Comsat_Station.gasPrice()
		){
			//Broodwar->sendText("Wanna build ComSat!");
			commandCenters[0]->buildAddon(UnitTypes::Terran_Comsat_Station); //builds comsat at 1st cmd center
		}
	}

	//tries to reveal hidden units
	//Broodwar->sendText << "will reveal" << endl;
	revealHiddenUnits();

	//Broodwar << "after trying to reveal" << endl;
	updateTasks();

	_commanderAgent->onFrame(allTasks, trainSCVIncentives, commandCenters);

	//iterates through all marines
	for(auto marine = marines.begin(); marine != marines.end(); marine++){
		marine->second->onFrame(allTasks, marines);
	}

	//iterates through all medics
	for(auto medic = medics.begin(); medic != medics.end(); medic++){
		medic->second->onFrame(allTasks);
	}

	// Iterate through all SCVs
	unordered_map<int, SCVAgent*>::iterator it = scvMap.begin();
	for(unordered_map<int, SCVAgent*>::iterator iter = scvMap.begin(); iter != scvMap.end(); ++iter){
		int unitId =  iter->first;
		SCVAgent* agent = iter->second;
		Unit u = agent->getUnit();

		agent->onFrame(&allTasks, discoveredMineralPositions, commandCenters, scvMap);

	}
	
}

unordered_map<TaskType, vector<Task>*>& ExampleAIModule::getTasks(){
	return allTasks;
}

unordered_map<int, SCVAgent*>& ExampleAIModule::getSCVMap(){
	return scvMap;
}

void ExampleAIModule::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());

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
	
	//Broodwar->sendText("Unit [%s] discovered ID [%d]", unit->getType().getName().c_str(), unit->getID());

	if (unit->getPlayer() == Broodwar->self() && unit->getType() == UnitTypes::Terran_Command_Center) {
		commandCenters.insert(unit);
	}
	else if(unit->getPlayer() == Broodwar->self() && unit->getType() == UnitTypes::Terran_SCV){
		// Add SCV to map
		SCVAgent *agent = new SCVAgent(unit, this);
		scvMap[unit->getID()] = agent;
		//(*scvMap)[unit->getID()] = agent; When SCVmap is a pointer
	}
	else if(unit->getPlayer() == Broodwar->self() && unit->getType() == UnitTypes::Terran_Marine){
		// Add marine to HashTable
		marines[unit->getID()] = new MarineAgent(unit);
	}
	else if(unit->getPlayer() == Broodwar->self() && unit->getType() == UnitTypes::Terran_Medic){
		medics[unit->getID()] = new MedicAgent(unit);
	}

	

	//new mineral discovered, is it at the range of a command center?
	//framecount test prevents checking on unacessible minerals at game begin
	if(Broodwar->getFrameCount() != 0 && unit->getType() == UnitTypes::Resource_Mineral_Field){
		discoveredMineralPositions.push_back(unit->getPosition());
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
	if (unit->getPlayer() == Broodwar->self() && unit->getType() == UnitTypes::Terran_Comsat_Station){
		ourComSat = unit;
	}

}

//BWAPI calls this when a unit dies or otherwise removed from the game (i.e. a mined out mineral patch)
void ExampleAIModule::onUnitDestroy(BWAPI::Unit unit){
	if (unit->getPlayer() == Broodwar->self()){
		Broodwar->sendText("%s lost.", unit->getType().getName().c_str());
		if(unit->getType() == UnitTypes::Terran_SCV){
			scvMap.erase(unit->getID()); 
		}
		else if(unit->getType() == UnitTypes::Terran_Marine){
			marines.erase(unit->getID());
		}
		else if (unit->getType() == UnitTypes::Terran_Command_Center){
			commandCenters.erase(unit);
		}
		else if(unit->getType() == UnitTypes::Terran_Medic){
			medics.erase(unit->getID());
		}
		else if (unit->getType() == UnitTypes::Terran_Comsat_Station){
			ourComSat = NULL;
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
	updateRepair();
	updateAttack();
	updateBuildSupplyDepot();
	updateBuildBarracks();
	updateBuildCommandCenter();
	updateTrainMarine();
	updateTrainSCV();
	updateExplore();
	updateBuildVespeneGas();
	updateBuildAcademy();
	updateTrainMedic();
	updateResearchLongRange();
	updateResearchStimPack();
	updateBuildBunker();
}

void ExampleAIModule::updateRepair(){
	//clears all repair tasks and insert new ones relative to buildings needing repair
	allTasks[Repair]->clear();

	for(auto bldg = Broodwar->self()->getUnits().begin(); bldg != Broodwar->self()->getUnits().end(); bldg++){
		//discards non buildings and unfinished buildings
		if (!bldg->getType().isBuilding() || bldg->isBeingConstructed()){
			continue;
		}
		float hpRate = bldg->getHitPoints() / float(bldg->getType().maxHitPoints());
		if ( hpRate < .6f || !bldg->isCompleted()){ //starts repairing when HP is less than 60% or its construction halted somehow
			float incentive = 1.0f - hpRate;

			//incentive is zero if target is being repaired
			for(auto scv = scvMap.begin(); scv != scvMap.end(); scv++){
				if(scv->second->repairTarget == *bldg){
					incentive = 0;
					break;
				}
			}

			allTasks[Repair]->push_back(Task(Repair, incentive, bldg->getPosition()));
		}
	}
}

void ExampleAIModule::revealHiddenUnits(){

	TechType scan = TechTypes::Scanner_Sweep;
	int cooldown = Broodwar->getFrameCount() - lastScan;

	//does not scan if there is no comsat, of energy is low, or cooldown has not been reached
	if(ourComSat == NULL ||  ourComSat->getEnergy() < scan.energyCost() || cooldown < 160 ){ //scan sweep lasts about 160 frames
		//Broodwar->sendText("NULL? %d, sc cooldown: %d, act cooldown: %d, cost:%d, energy:%d", ourComSat == NULL, scan.getWeapon().damageCooldown(), cooldown, scan.energyCost(), ourComSat->getEnergy());
		return;
	}

	Unitset allunits = Broodwar->getAllUnits();
	for(auto unit = allunits.begin(); unit != allunits.end(); unit++){
		if(unit->getPlayer() != Broodwar->self() && (unit->isCloaked() || unit->isBurrowed())){
			//hidden unit detected, scan it!
			ourComSat->useTech(scan, unit->getPosition());
			lastScan = Broodwar->getFrameCount();
		}
	}

	// Get random attack area to do a scan!
	if(ourComSat->getEnergy() >= 170 && allTasks[Attack]->size() > 0){
		vector<Task> *attackTaskVector = allTasks[Attack];
		vector<Task>::iterator attackIt = (*attackTaskVector).begin();
		int index = min(int((*attackTaskVector).size() - 1), max(0, generateRandomnteger(0, (*attackTaskVector).size()))); //validation
		std::advance( attackIt, index );
		ourComSat->useTech(scan, attackIt->getPosition()); //TODO: error here!
		lastScan = Broodwar->getFrameCount();
		Broodwar << "Random attack position revealed!" << endl;
	}
}

/**
  * Create new Tasks for each enemy units not covered by attack tasks
  * Cleans up attack tasks whose targets are not in position anymore
  * AttackTasks are added at onUnitDiscover()
  */
void ExampleAIModule::updateAttack(){
	//traverse the task list to check if positions are visible and still have enemies
	vector<Task>* preserved = new vector<Task>(); //stores the tasks that should not be removed
	UnitType marineType = UnitTypes::Terran_Marine;

	//Broodwar->drawTextScreen(200,120,"marine seek: %d // sight: %d", marineType.seekRange(), marineType.sightRange());
	
	for(auto task = allTasks[Attack]->begin(); task != allTasks[Attack]->end(); task++){

		if(Broodwar->isVisible(task->getPosition().x / TILE_SIZE , task->getPosition().y / TILE_SIZE)){
			Unitset inRange = Broodwar->getUnitsInRadius(task->getPosition(),  marineType.groundWeapon().maxRange(), Filter::IsEnemy);
			//Broodwar->sendText("%d in range of attack task.", inRange.size());
			
			// Check if the unit can be reached by the marines
			// Sometimes cloacked or burrowed units can be marked as enemy but it cannot be attacked
			bool onlyCloackedUnits = true;
			for(auto u = inRange.begin(); u != inRange.end(); ++u) {
				if(!u->isCloaked() && !u->isBurrowed() & !u->isInvincible()){
					onlyCloackedUnits = false;
					break;
				}
			}
			if (inRange.size() == 0 || onlyCloackedUnits) {
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
			//foeUnit->getType().groundWeapon().maxRange();
			//PositionTask* atk = static_cast<PositionTask* >( &(*task)) ; 
			if(foeUnit->getDistance(task->getPosition()) < marineType.groundWeapon().maxRange()){
				inRange = true;
				break;
			}
		}

		// Hector : extra validation to ignore unreachable targets
		//TODO: test is reachable 
		Position foePos = foeUnit->getPosition();
		if(! inRange && !foeUnit->isCloaked() && !foeUnit->isBurrowed() && !foeUnit->isInvincible() && Broodwar->isWalkable(foePos.x / 8, foePos.y / 8)){
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

void ExampleAIModule::updateTrainMarine(){
	//tries to make a 12 marines per base (attempts to save some money to expansions)
	if (Broodwar->self()->minerals() < 500){
		if(commandCenters.size() < 2){
			trainMarine->setIncentive(max(0.0f,  (0.1f/max(1.0f, marines.size() * 1.0f))));
		}
		else{
			trainMarine->setIncentive(max(0.0f, 1.0f - (marines.size() / max(30.0f, ( 300.0f * (5 - commandCenters.size()))))));
		}
	}
	else {
		//if i have money, produce more
		trainMarine->setIncentive(.8f);
	}
	//trainMarine->setIncentive(.8f);
}

void ExampleAIModule::updateTrainSCV(){

	for(Unitset::iterator cmd = commandCenters.begin(); cmd != commandCenters.end(); ++cmd){	
		
		Unitset mineralsAround = Broodwar->getUnitsInRadius(cmd->getPosition(), BASE_RADIUS, Filter::IsMineralField);
		Unitset scvAround = Broodwar->getUnitsInRadius(cmd->getPosition(), BASE_RADIUS, Filter::IsWorker && Filter::IsOwned);

		trainSCVIncentives[*cmd] = max(0.0f, 1.0f - (scvAround.size() / (2.5f * mineralsAround.size())));
		
		/*
		if(scvMap.size() < 110){
			trainSCVIncentives[*cmd] = max(0.0f, 1.0f - (scvAround.size() / (2.5f * mineralsAround.size())));
		}
		else{
			trainSCVIncentives[*cmd] = 0.0f;
		}*/

	}
}

void ExampleAIModule::updateBuildCommandCenter(){

	//if we don't have enough minerals for a CMD center, sets zero incentive
	if (Broodwar->self()->minerals() < UnitTypes::Terran_Command_Center.mineralPrice()){
		buildCommandCenter->setIncentive(0.0f);
		return;
	}

	//for every discovered mineral, check if it is in range of a command center
	for(auto mPos = discoveredMineralPositions.begin(); mPos != discoveredMineralPositions.end(); ++mPos){
		bool reachable = false;
		for(Unitset::iterator cmd = commandCenters.begin(); cmd != commandCenters.end(); ++cmd){
			if (cmd->getPosition().getApproxDistance(*mPos) < BASE_RADIUS){
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
	vector<Task>* newBarracksNeeded = new vector<Task>();

	for (Unitset::iterator c = commandCenters.begin(); c != commandCenters.end(); c++){
		int barracksNumber = calculateBarracksFromCommandCenter(Broodwar->getUnit(c->getID()));
		float incentive =  max(0.0f, 1.0f - barracksNumber/3.0f);

		//sets incentive to ZERO if we have not enough minerals
		if(Broodwar->self()->minerals() < UnitTypes::Terran_Barracks.mineralPrice()){
			incentive = 0.0f;
		}

		//updates the number of barracks around all command centers
		//builtBarracks[*c] = barracksNumber;
		//buildBarracksIncentives[*c] = incentive;

		newBarracksNeeded->push_back(Task(BuildBarracks, incentive, c->getPosition()));
	}

	allTasks[BuildBarracks]->swap(*newBarracksNeeded);
	delete newBarracksNeeded; //hope this doesn't invalidates the barracks
}

/**
  * Updates the incentive for the Task buildSupplyDepot
  **/
void ExampleAIModule::updateBuildSupplyDepot(){
	
	if(Broodwar->self()->supplyTotal() == 400){
		//max supply reached, no need to build more depots
		buildSupplyDepot->setIncentive(0);
		return;
	}

	if(Broodwar->self()->minerals() < UnitTypes::Terran_Supply_Depot.mineralPrice()){
		//not enough minerals, can't build more depots
		buildSupplyDepot->setIncentive(0);
		return;
	}

	int availableSupply = Broodwar->self()->supplyTotal();
	//increases available supply if we are already building a supply depot

	for (auto scv = scvMap.begin(); scv != scvMap.end(); scv++){
		if(scv->second->state == BUILDING_SUPPLY_DEPOT){
			availableSupply += 16; //adds 2*supply provided by a depot
		}
	}

	int dif = max(0, (availableSupply - Broodwar->self()->supplyUsed())/2); //bwapi returns 2*the actual difference...
	
	//incentive is maximum when difference is minimum
	//buildSupplyDepot->setIncentive(max(0.0f,1.0f - (dif/10.0f))); //linear decay
	buildSupplyDepot->setIncentive(max(0.0f, pow(float(EULER),-dif/2.0f))); //exponential decay

}

void ExampleAIModule::updateExplore(){
	int exploredTiles = 0;
	int width = Broodwar->mapWidth();
	int height = Broodwar->mapHeight();

	//check which tiles were explored
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
	if (Broodwar->getFrameCount()/24 > 60) {
		explore->setIncentive(1.0f - float(exploredTiles) / (width * height));
	}
    //int minutes = seconds/60;
}

/**
  * Counts the barracks constructed or under construction around a command center
  */
int ExampleAIModule::calculateBarracksFromCommandCenter(Unit cmdCenter) {

	//first: count built barracks around the command center
	Position commandCenterPos = cmdCenter->getPosition();
	
	Unitset units = Broodwar->getUnitsInRadius(commandCenterPos, BASE_RADIUS);
	int builtBarracks = 0;
	for ( Unitset::iterator u = units.begin(); u != units.end(); ++u ) {
		if ( u->getType() == UnitTypes::Terran_Barracks && u->isCompleted()) {
			builtBarracks++;
		}
	}
	
	//second: count the SCVs constructing around the command center
	int scheduledForConstruction = 0;
	for(auto scv = scvMap.begin(); scv != scvMap.end(); scv++){
		//if SCV is not constructing the barracks but is moving towards it...
		if(scv->second->state == BUILDING_BARRACKS && Position(scv->second->newBuildingLocation).getApproxDistance(commandCenterPos) < BASE_RADIUS ){
			
			Broodwar->drawCircleMap(scv->second->gameUnit->getPosition(),20,Color(Colors::Cyan));
			scheduledForConstruction++;
		}
	}
	//Broodwar->sendText("%d - %d",builtBarracks,scheduledForConstruction);
	return builtBarracks + scheduledForConstruction;
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
	Broodwar->drawTextScreen(290, 0,  "FPS: %d", Broodwar->getFPS() );
	Broodwar->drawTextScreen(290, 15, "Average FPS: %f // Frame count: %d", Broodwar->getAverageFPS(), Broodwar->getFrameCount() );
	Broodwar->drawTextScreen(290, 45, "Time: ~ %dh%dm%ds", Broodwar->elapsedTime() / 3600, Broodwar->elapsedTime() / 60, Broodwar->elapsedTime() % 60);
	_drawExploredStats();

	// display some debug info...
	Broodwar->drawTextScreen(20, 0, "%cSupply Depot incentive = %.3f", 
		Text::White, 
		buildSupplyDepot->getIncentive()
	); 

	//Broodwar->sendText("%d", &allTasks[BuildSupplyDepot]->at(0) == buildSupplyDepot);

	Broodwar->drawTextScreen(20, 15, "%cExplore incentive = %.3f", 
		Text::White, 
		explore->getIncentive()
	);

	Broodwar->drawTextScreen(20, 30, "%c#Marines: %d // #atk tasks: %d // Train incentive = %.3f // #Medics :%d Incentive :%.3f", 
		Text::White, 
		marines.size(),
		allTasks[Attack]->size(),
		trainMarine->getIncentive(),
		medics.size(),
		trainMedic->getIncentive()
	);

	Broodwar->drawTextScreen(20, 45, "%c#SCVs: %d // Mine incentive = %.3f", 
		Text::White, 
		scvMap.size(),
		gatherMinerals->getIncentive()
	);

	Broodwar->drawTextScreen(20, 60, "%cBuild CMD [%.3f] Academy [%.3f] Gas [%.3f] U-238 [%.3f] Stim [%.3f]", 
		Text::White, 
		buildCommandCenter->getIncentive(),
		buildAcademy->getIncentive(),
		buildVespeneGas->getIncentive(),
		researchAcademyLongRange->getIncentive(),
		researchAcademyStimpack->getIncentive()
	);

	Broodwar->drawTextScreen(20, 75, "%cRepair tasks: %d", 
		Text::White, 
		allTasks[Repair]->size()
	);

	Broodwar->drawTextScreen(20,90, "%cBrk inc. // SCV inc:", 
		Text::White 
	);

	int yOffset = 0;
	/*for (Unitset::iterator cmd = commandCenters.begin(); cmd != commandCenters.end(); ++cmd){
		Broodwar->drawTextScreen(20,90 + yOffset, "%c%d // %.2f // %.3f", 
			Text::White, builtBarracks[*cmd], buildBarracksIncentives[*cmd], trainSCVIncentives[*cmd]
		);
		yOffset += 15;
	}*/

	for(auto brkTask = allTasks[BuildBarracks]->begin(); brkTask != allTasks[BuildBarracks]->end(); ++brkTask){
		Unit cmdCenterAtPos = Broodwar->getUnitsInRadius(brkTask->getPosition(), 5)[0];
		Broodwar->drawTextScreen(20,105 + yOffset, "%c%.2f // %.3f", 
			Text::White, brkTask->getIncentive(), trainSCVIncentives[cmdCenterAtPos]
		);
		yOffset += 15;
	}

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

	//writes info under marines
	for(auto m = marines.begin(); m != marines.end(); m++){
		Broodwar->drawTextMap(m->second->gameUnit->getPosition(), "ID[%d]", m->second->gameUnit->getID());
	}

	//writes debug info under marines
	/*for(auto m = marines.begin(); m != marines.end(); ++m){
		MarineAgent* mar = m->second;
		Broodwar->drawTextMap(mar->gameUnit->getPosition(),"%d,%d", mar->gameUnit->getPosition().x, mar->gameUnit->getPosition().y);
	}*/

	//draws circles around Attack targets
	for(auto task = allTasks[Attack]->begin(); task != allTasks[Attack]->end(); task++){
		Broodwar->drawCircleMap(task->getPosition(), UnitTypes::Terran_Marine.groundWeapon().maxRange(), Color(Colors::Red));
	}

	//draws info under the buildings
	Unitset all = Broodwar->self()->getUnits();
	for(auto bldg = all.begin(); bldg != all.end(); bldg++){
		if( ! bldg->getType().isBuilding()){
			continue;
		}
		string stat = "";
		if(bldg->isBeingConstructed()){
			stat = "Men at work";
		}
		else if (!bldg->isCompleted()) {
			stat = "HALTED!";
		}

		Broodwar->drawTextMap(bldg->getPosition(), stat.c_str());
	}
}

/**
  * Draws green dots on explored tiles and red on unexplored ones
  */
void ExampleAIModule::_drawExploredStats(){
	int width = Broodwar->mapWidth();
	int height = Broodwar->mapHeight();

	//check which tiles were explored
	for (int hTile = 0; hTile < width; hTile++){ 
		for (int vTile = 0; vTile < height ; vTile++){
			if (Broodwar->isExplored(hTile,vTile)){
				//exploredTiles++;
				Broodwar->drawDotMap(hTile*TILE_SIZE+16,vTile*TILE_SIZE+16,Colors::Green);
			}
			else{
				Broodwar->drawDotMap(hTile*TILE_SIZE+16,vTile*TILE_SIZE+16,Colors::Red);
			}
		}
	}
}

/**
  * Updates the incentive for the Task buildVespeneGas
  **/
void ExampleAIModule::updateBuildVespeneGas(){
	
	if (Broodwar->getFrameCount()/24 < 120) {
		buildVespeneGas->setIncentive(0);
		return;
	}

	if(Broodwar->self()->minerals() < UnitTypes::Terran_Refinery.mineralPrice()){
		//not enough minerals, can't build more vespene gas
		buildVespeneGas->setIncentive(0);
		return;
	}

	for (Unitset::iterator c = commandCenters.begin(); c != commandCenters.end(); c++){
		Unitset units = Broodwar->getUnitsInRadius(c->getPosition(), BASE_RADIUS, Filter::IsResourceContainer);
		bool constructVespene = false;
		for (auto unit = units.begin(); unit != units.end(); unit++){
			if(!unit->getType().isRefinery()){
				constructVespene = true;
				break;
			}
		}

		if(constructVespene){
			buildVespeneGas->setIncentive(1.0f);
		}
	}
}

/**
  * Updates the incentive for the Task buildAcademy
  **/
void ExampleAIModule::updateBuildAcademy(){
	
	if(Broodwar->self()->minerals() < UnitTypes::Terran_Academy.mineralPrice()){
		//not enough minerals, can't build more vespene gas
		buildAcademy->setIncentive(0.0f);
		return;
	}

	Unitset units = Broodwar->self()->getUnits();
	bool hasAcademy = false;

	//UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
	//static int lastChecked = 0;

	// If we are supply blocked and haven't tried constructing more recently
	if (Broodwar->self()->incompleteUnitCount(UnitTypes::Terran_Academy) > 0) {
		hasAcademy = true;
	}

	if(!hasAcademy){
		for (Unitset::iterator unit = units.begin(); unit != units.end(); unit++){
			if(unit->getType() == UnitTypes::Terran_Academy && unit->exists()){
				hasAcademy = true;
				break;
			}
		}
	}

	if(!hasAcademy){
		//&& commandCenters.size() >= 2){
		buildAcademy->setIncentive(.6f);
	}
	else{
		buildAcademy->setIncentive(.0f);
	}
}

void ExampleAIModule::updateTrainMedic(){
	//tries to make a 12 marines per base (attempts to save some money to expansions)
	int numberOfMedics = medics.size();
	int numberOfMarines = max(1.0f, marines.size() + 0.0f);

	/*if (Broodwar->self()->minerals() < 450 
		&& commandCenters.size() < 2){
		trainMedic->setIncentive(0.001f);
	}
	else {*/
		//if((numberOfMarines / 4) > numberOfMedics && Broodwar->self()->gas() >= 25){
			int expectedMedicNumbers = max(1.0f, (numberOfMarines / 3) + 0.0f);
			//trainMedic->setIncentive(max(0.2f, numberOfMedics / (expectedMedicNumbers) + 0.0f));
			if(expectedMedicNumbers > numberOfMedics){
				trainMedic->setIncentive(1.0f - (numberOfMedics / (expectedMedicNumbers) + 0.0f));
			}
			else{
				trainMedic->setIncentive(.0f);
			}
		//}
		//else{
		//	trainMedic->setIncentive(.0f);
		//}
		
	//}
}

void ExampleAIModule::updateResearchLongRange(){
	
	if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::U_238_Shells) > 0) {
		researchAcademyLongRange->setIncentive(0.0f);
		return;
	}

	if(Broodwar->self()->minerals() < UpgradeTypes::U_238_Shells.mineralPrice() ||
		Broodwar->self()->gas() < UpgradeTypes::U_238_Shells.gasPrice()){
		//not enough minerals, can't build more vespene gas
		researchAcademyLongRange->setIncentive(0.0f);
		return;
	}

	Unitset units = Broodwar->self()->getUnits();
	bool hasAcademy = false;

	for (Unitset::iterator unit = units.begin(); unit != units.end(); unit++){
		if(unit->getType() == UnitTypes::Terran_Academy && unit->exists()){
			hasAcademy = true;
			break;
		}
	}

	if(hasAcademy && commandCenters.size() >= 2){
		researchAcademyLongRange->setIncentive(.8f);
	}
	else{
		researchAcademyLongRange->setIncentive(.0f);
	}
}

void ExampleAIModule::updateResearchStimPack(){
	TechType stim = TechTypes::Stim_Packs;

	if (Broodwar->self()->hasResearched(stim)){
		researchAcademyStimpack->setIncentive(0.0f);
		return;
	}
	
	if(Broodwar->self()->minerals() < stim.mineralPrice() || Broodwar->self()->gas() < stim.gasPrice()){
		//not enough resources
		researchAcademyStimpack->setIncentive(0.0f);
		return;
	}

	Unitset units = Broodwar->self()->getUnits();
	bool hasAcademy = false;


	for (Unitset::iterator unit = units.begin(); unit != units.end(); unit++){
		if(unit->getType() == UnitTypes::Terran_Academy && unit->exists()){
			hasAcademy = true;
			break;
		}
	}

	if(hasAcademy){
		researchAcademyStimpack->setIncentive(.8f);
	}
	else{
		researchAcademyStimpack->setIncentive(.0f);
	}
}

void ExampleAIModule::updateBuildBunker(){
	//calculates the number of barracks around the command center
	vector<Task>* newBunkersNeeded = new vector<Task>();

	for (Unitset::iterator c = commandCenters.begin(); c != commandCenters.end(); c++){
		int bunkerNumber = calculateBunkersFromCommandCenter(Broodwar->getUnit(c->getID()));
		float incentive =  max(0.0f, 1.0f - bunkerNumber/1.0f);

		//sets incentive to ZERO if we have not enough minerals
		if(Broodwar->self()->minerals() < UnitTypes::Terran_Bunker.mineralPrice()){
			incentive = 0.0f;
		}

		//updates the number of barracks around all command centers
		//builtBarracks[*c] = barracksNumber;
		//buildBarracksIncentives[*c] = incentive;

		newBunkersNeeded->push_back(Task(BuildBunker, incentive, c->getPosition()));
	}

	allTasks[BuildBunker]->swap(*newBunkersNeeded);
	delete newBunkersNeeded; //hope this doesn't invalidates the barracks :)
}

/**
  * Counts the bunkers constructed or under construction around a command center
  */
int ExampleAIModule::calculateBunkersFromCommandCenter(Unit cmdCenter) {

	//first: count built barracks around the command center
	Position commandCenterPos = cmdCenter->getPosition();
	
	Unitset units = Broodwar->getUnitsInRadius(commandCenterPos, BASE_RADIUS);
	int builtBunkers = 0;
	for ( Unitset::iterator u = units.begin(); u != units.end(); ++u ) {
		if ( u->getType() == UnitTypes::Terran_Bunker && u->isCompleted()) {
			builtBunkers++;
		}
	}
	
	//second: count the SCVs constructing around the command center
	int scheduledForConstruction = 0;
	for(auto scv = scvMap.begin(); scv != scvMap.end(); scv++){
		//if SCV is not constructing the barracks but is moving towards it...
		if(scv->second->state == BUILDING_BUNKER && Position(scv->second->newBuildingLocation).getApproxDistance(commandCenterPos) < BASE_RADIUS ){
			
			Broodwar->drawCircleMap(scv->second->gameUnit->getPosition(),20,Color(Colors::Cyan));
			scheduledForConstruction++;
		}
	}
	//Broodwar->sendText("%d - %d",builtBarracks,scheduledForConstruction);
	return builtBunkers + scheduledForConstruction;
}

int ExampleAIModule::generateRandomnteger(int nMin, int nMax)
{
    return nMin + (int)((double)rand() / (RAND_MAX+1) * (nMax-nMin+1));
}