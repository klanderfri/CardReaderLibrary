#include "stdafx.h"
#include "ApplicationMessages.h"
#include "CardCollectionReader.h"
#include <iostream>

using namespace std;

ApplicationMessages::ApplicationMessages(Session* session) :
	SessionBound(session)
{
}

ApplicationMessages::~ApplicationMessages()
{
}

void ApplicationMessages::printWelcomeMessage() {

	if (!session->runSilent) {
		wcout << L"Welcome to the eminent MtG card reader! Let's read some cards! :-D" << endl << endl;
	}
}

void ApplicationMessages::printWorkingFolderMessage(wstring mtgFolderPath) {

	if (!session->runSilent) {
		wcout << L"Looking for card images in folders . . ." << endl;
		wcout << mtgFolderPath << endl << endl;
	}
}

void ApplicationMessages::printSavingResultsToDiskMessage() {

	if (!session->runSilent) {
		wcout << L"Saving the results to disk . . ." << endl;
	}
}

void ApplicationMessages::printResultsHasBeenSavedToDiskMessage(wstring pathToResultFolder) {

	if (!session->runSilent) {
		wcout << L"The results has been written to a folder on disk:" << endl << pathToResultFolder << endl << endl;
	}
}

void ApplicationMessages::printResultMessage(int numberOfErrors) {
	
	if (!session->runSilent) {
		wstring message;

		if (numberOfErrors == 0) {
			session->systemMethods->SetCommandLineTextColour(Colour::Green);
			message = L"All images was successfully read! Yay! :-)";
		}
		else {
			session->systemMethods->SetCommandLineTextColour(Colour::Red);
			wstring amountOfErrorsStr = to_wstring(numberOfErrors);
			wstring errorWord = (numberOfErrors == 1) ? L"error" : L"errors";
			message = L"Oh no! There was " + amountOfErrorsStr + L" " + errorWord + L" when reading the cards! :-(";
		}
		wcout << message << endl;
		session->systemMethods->ResetCommandLineTextColour();
	}
}

void ApplicationMessages::printNoImagesMessage() {

	if (!session->runSilent) {
		session->systemMethods->SetCommandLineTextColour(Colour::Yellow);
		wcout
			<< L"Hmm, it semms like there are no cards... :-|"
			<< endl;
		session->systemMethods->ResetCommandLineTextColour();
	}
}

void ApplicationMessages::printToManyFilesMessage() {

	if (!session->runSilent) {
		session->systemMethods->SetCommandLineTextColour(Colour::Red);
		wcout
			<< L"Glunck! There were to many cards for me to handle! Don't feed me more than "
			+ to_wstring(CardCollectionReader::MaxSize())
			+ L" card image files!"
			<< endl;
		session->systemMethods->ResetCommandLineTextColour();
	}
}

void ApplicationMessages::printExecutionTimeMessage(TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds) {

	if (!session->runSilent) {
		auto executionDurationTime = TimePoint::GetExecutionTime(startTime, endTime);

		//Print total execution time.
		float totalExecutionTime = executionDurationTime / (float)(showTimeInSeconds ? 1000000 : 1);
		wstring exeTime = session->systemMethods->ToWString(totalExecutionTime, showTimeInSeconds ? 1 : 0);
		wstring timeUnit = showTimeInSeconds ? L"seconds" : L"microseconds";
		wcout << endl << L"The reading took " + exeTime + L" " + timeUnit + L" to execute." << endl;

		//Print card average execution time.
		if (numberOfFilesExecuted > 0) {
			float executionTimePerCard = executionDurationTime / numberOfFilesExecuted / (float)1000000;
			wstring averageExecutionTimePerCard = session->systemMethods->ToWString(executionTimePerCard, showTimeInSeconds ? 2 : 8);
			wcout << L"That's " + averageExecutionTimePerCard + L" seconds per card on average!" << endl;
		}

		wcout << endl;
	}
}