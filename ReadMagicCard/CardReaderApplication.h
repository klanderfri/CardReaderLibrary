#pragma once
#include "CardCollectionReader.h"
#include "TimePoint.h"
#include "ApplicationMessages.h"
//Class holding the card reader application.
class CardReaderApplication
{
public:
	CardReaderApplication(char* runArguments[], int numberOfRunArguments);
	~CardReaderApplication();

	//Runs the card reader application.
	int Run();

private:

	//Removes lingering old data from previous runs.
	void removeOldData();
	//Fetches the file paths of the MtG image files.
	std::vector<std::wstring> getMtgImageFilePaths();
	//Creates an collection holding a reader for every card image.
	CardCollectionReader* createCardReaderCollection(const std::vector<std::wstring> filepathsOfImages);
	//Resizes the command window to occupy as little space as possible without creating line breakes in its' output texts. 
	void reziseCommandWindow(size_t numberOfFiles, int lengthOfLongestFilename);
	//Reads the title of the card in the provided image files.
	std::vector<CardNameInfo> readAllCards(const std::vector<std::wstring> filepathsOfImages);

	//The current session.
	Session* session;
	//Object holding methods printing messages for the application.
	ApplicationMessages* messages;
};

