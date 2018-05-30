#pragma once
#include "SystemMethods.h"
#include "ImageHelper.h"
#include "MtgCardInfoHelper.h"
#include "CardReadInput.h"
#include "FileSystemHelper.h"
//Class for the session of card reading.
class Session
{
public:
	Session(char const* input);
	~Session();

	//The height of the shrinked card image we work with to maximize performance.
	const int WORKING_CARD_HEIGHT;

	//Pointer to object handling the system dependent methods.
	SystemMethods* systemMethods;
	//Pointer to object containing image methods.
	ImageHelper* imageHelper;
	//Pointer to object holding methods giving information about the MtG card environment.
	MtgCardInfoHelper* cardInfo;
	//Pointer to data holding information about how the card reading should be done.
	CardReadInput* inputData;
	//Pointer to object providing methods for handling files.
	FileSystemHelper* fileSystem;

};

