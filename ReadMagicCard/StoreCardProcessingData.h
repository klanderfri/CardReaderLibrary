#pragma once
#include "Card.h"
//Class storing information about the card processing into files on disk.
class StoreCardProcessingData
{
public:
	StoreCardProcessingData();
	~StoreCardProcessingData();

	//Stores the final result.
	std::wstring StoreFinalResult(std::vector<Card> result);
};

