#pragma once
#include "Toolbox.h"
#include "ApplicationMessages.h"
#include "Card.h"
#include "TimePoint.h"
//Class holding the card reader application.
class CardReaderApplication
{
public:
	CardReaderApplication(char* runArguments[], int numberOfRunArguments);
	~CardReaderApplication();

	//Runs the card reader application.
	int Run();

private:


	std::vector<Card> readAllCards(std::vector<std::wstring> filepathsOfImages);

	std::vector<Card> extractCardData(std::string readDataOutput);
	
	std::string createReadingParameter(std::vector<std::wstring> filepathsOfImages);
	//Removes lingering old data from previous runs.
	void removeOldData();
	//Fetches the file paths of the MtG image files.
	std::vector<std::wstring> getMtgImageFilePaths();
	//Resizes the command window to occupy as little space as possible without creating line breakes in its' output texts. 
	void reziseCommandWindow(size_t numberOfFiles, int lengthOfLongestFilename);

	//Object holding methods printing messages for the application.
	ApplicationMessages* messages;

	Toolbox* toolbox;
};

