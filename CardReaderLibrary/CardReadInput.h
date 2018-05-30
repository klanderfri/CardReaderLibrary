#pragma once
#include "SystemMethods.h"
class CardReadInput
{
public:
	CardReadInput(char const* input, SystemMethods* systemMethods);
	~CardReadInput();

	//Tells if the program should do debugging such as exporting the images to files.
	bool runDebugging;
	//Tells if the program should run without printing any messages.
	bool runSilent;
	//Tells if the program should read each card in a separate thread.
	bool runParallelized;
	//Tells if the program should store the cropped card images.
	bool storeExtractedCardImages;
	//The file path of specific images to decode.
	std::vector<std::wstring> fullFilePathsOfImagesToDecode;
	//Tells the program where output files should be places.
	std::wstring fullOutputFolderPath;

	//The delimiter separating the input data.
	static const char INPUT_DELIMITER = ';';

private:


	void setInputData(char const* input);

	//Pointer to object handling the system dependent methods.
	SystemMethods* systemMethods;
};

