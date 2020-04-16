#include <vector>
#include <algorithm>
#include <cstdlib>
#include <BWAPI.h>
#include <deque>
#include "TaskAssociation.h"
#include <string>
#include <ctime>

using namespace std;


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

/**
  * Selects a Task from the list with chance in proportion to its incentive value
  */
Task* weightedSelection(vector<TaskAssociation>& theTasks){
	sort(theTasks.begin(), theTasks.end());

	float sum = 0.f;

	for(auto ta = theTasks.begin(); ta != theTasks.end(); ++ta){
		sum += ta->tValue();
	}

	//generates a pseudo-random number between 0 and sum
	float random = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/sum));
	
	//traverses the list until it finds the task that matches the random number
	float acc = 0;
	for(auto ta = theTasks.begin(); ta != theTasks.end(); ++ta){
		if(acc + ta->tValue() < random){
			return ta->task(); 
		}
	}
	return NULL;
	//Broodwar->sendText("Weighted selection error! %d items; %f sum", theTasks.size(), sum);
}

Position getPositionToScout(Unit unit, Position& lastPosition){
	Position returnPosition;
	//Unit unit = ((Unit) gameUnit);

	int radiousInPixels = 120*TILE_SIZE;
	std::deque<Position> positionListInRadious;
	std::deque<Position> positionListInMap;
	Position myPos = unit->getPosition();
	//Broodwar->getr
	Region myRegion = Broodwar->getRegionAt( myPos );

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

bool goScout(Unit gameUnit, Position& lastPosition, int lastFrameCount){

	Position myPos = gameUnit->getPosition();
	Broodwar->drawLineMap(myPos, lastPosition, Colors::Blue);

	double square_dist = pow((lastPosition.x- myPos.x), 2.0) + pow((lastPosition.y - myPos.y), 2.0);
	if (lastPosition.x != 0 && lastPosition.y != 0 && square_dist >= pow(3*TILE_SIZE, 2.0)){
		gameUnit->move(lastPosition);
		return false;
	}

	int currentFrameCount = Broodwar->getFrameCount();
	if ( currentFrameCount >= lastFrameCount + 20){
		lastFrameCount = currentFrameCount;
		Position pos = getPositionToScout(gameUnit, lastPosition);
		Broodwar << "Agent [" << gameUnit->getID() << "] Scouting to :" << pos << "\n";

		gameUnit->move(pos);
		return true;
	}

	return false;
}

