#include "stdafx.h"
#include "Session.h"

using namespace std;

Session::Session()
	: WORKING_CARD_HEIGHT(680)
{
}

Session::Session(SystemMethods* systemMethods, const bool runSilent, const bool runParallelized, const bool runDebugging)
	: Session()
{
	this->runDebugging = runDebugging;
	this->systemMethods = systemMethods;
	this->runSilent = runSilent;
	this->runParallelized = runParallelized;
}

Session::~Session()
{
	//Go memory! Be FREEEE!!
	delete systemMethods;
}