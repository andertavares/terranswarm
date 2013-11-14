#pragma once
#include <BWAPI.h>

class SCVAgent {
	BWAPI::Unit* gameUnit;
public:
	SCVAgent(BWAPI::Unit* scv);
	~SCVAgent(void);
};

