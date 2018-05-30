#include "stdafx.h"
#include "Session.h"
#include "WindowsMethods.h"

using namespace std;

Session::Session(char const* input)
	: WORKING_CARD_HEIGHT(680)
{
	this->systemMethods = new WindowsMethods();
	this->inputData = new CardReadInput(input, this->systemMethods);
	this->imageHelper = new ImageHelper(this->systemMethods);
	this->cardInfo = new MtgCardInfoHelper();
	this->fileSystem = new FileSystemHelper(this->systemMethods, this->inputData);
}

Session::~Session()
{
	//Go memory! Be FREEEE!!
	delete imageHelper;
	delete cardInfo;
	delete fileSystem;
	delete inputData;
	delete systemMethods;
}