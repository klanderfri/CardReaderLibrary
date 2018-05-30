#include "stdafx.h"
#include "CardReadInput.h"

using namespace std;

CardReadInput::CardReadInput(char const* input, SystemMethods* systemMethods)
{
	this->systemMethods = systemMethods;
	setInputData(input);
}

CardReadInput::~CardReadInput()
{
}

void CardReadInput::setInputData(char const* input) {

	string data(input);

	runSilent = systemMethods->ToBool(data.substr(0, 1));
	runParallelized = systemMethods->ToBool(data.substr(2, 1));
	storeExtractedCardImages = systemMethods->ToBool(data.substr(4, 1));
	runDebugging = systemMethods->ToBool(data.substr(6, 1));

	int index = data.find(INPUT_DELIMITER, 8);
	fullOutputFolderPath = systemMethods->ToWString(data.substr(8, index - 8));

	data = data.substr(index + 1);

	while ((index = data.find(INPUT_DELIMITER)) >= 0) {

		wstring path = systemMethods->ToWString(data.substr(0, index));
		fullFilePathsOfImagesToDecode.push_back(path);

		data = data.substr(index + 1);
	}
}
