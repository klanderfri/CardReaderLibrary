#include "stdafx.h"
#include "ApplicationMessages.h"
#include "CardCollectionReader.h"
#include <iostream>

using namespace std;

ApplicationMessages::ApplicationMessages(SystemMethods * systemMethods, const bool runSilent)
	: runSilent(runSilent)
{
	this->systemMethods = systemMethods;
}

ApplicationMessages::~ApplicationMessages()
{
}

void ApplicationMessages::printWelcomeMessage() {

	if (!runSilent) {
		wcout << L"Welcome to the eminent MtG card reader! Let's read some cards! :-D" << endl << endl;
	}
}

void ApplicationMessages::printWorkingFolderMessage(wstring mtgFolderPath) {

	if (!runSilent) {
		wcout << L"Looking for card images in folders . . ." << endl;
		wcout << mtgFolderPath << endl << endl;
	}
}

void ApplicationMessages::printResultMessage(int numberOfErrors, wstring pathToResultFile) {
	
	if (!runSilent) {
		wstring message;

		if (numberOfErrors == 0) {
			systemMethods->SetCommandLineTextColour(Colour::Green);
			message = L"All images was successfully read! Yay! :-)";
		}
		else {
			systemMethods->SetCommandLineTextColour(Colour::Red);
			wstring amountOfErrorsStr = to_wstring(numberOfErrors);
			wstring errorWord = (numberOfErrors == 1) ? L"error" : L"errors";
			message = L"Oh no! There was " + amountOfErrorsStr + L" " + errorWord + L" when reading the cards! :-(";
		}
		wcout << endl << message << endl;
		systemMethods->ResetCommandLineTextColour();

		wcout << endl << L"The results has been written to a result file:" << endl << pathToResultFile << endl;
	}
}

void ApplicationMessages::printNoImagesMessage() {

	if (!runSilent) {
		systemMethods->SetCommandLineTextColour(Colour::Yellow);
		wcout
			<< L"Hmm, it semms like there are no cards... :-|"
			<< endl;
		systemMethods->ResetCommandLineTextColour();
	}
}

void ApplicationMessages::printToManyFilesMessage() {

	if (!runSilent) {
		systemMethods->SetCommandLineTextColour(Colour::Red);
		wcout
			<< L"Glunck! There were to many cards for me to handle! Don't feed me more than "
			+ to_wstring(CardCollectionReader::MaxSize())
			+ L" card image files!"
			<< endl;
		systemMethods->ResetCommandLineTextColour();
	}
}

void ApplicationMessages::printExecutionTimeMessage(TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds) {

	if (!runSilent) {
		auto executionDurationTime = TimePoint::GetExecutionTime(startTime, endTime);

		//Print total execution time.
		float totalExecutionTime = executionDurationTime / (float)(showTimeInSeconds ? 1000000 : 1);
		wstring exeTime = systemMethods->ToWString(totalExecutionTime, showTimeInSeconds ? 1 : 0);
		wstring timeUnit = showTimeInSeconds ? L"seconds" : L"microseconds";
		wcout << endl << L"The reading took " + exeTime + L" " + timeUnit + L" to execute." << endl;

		//Print card average execution time.
		if (numberOfFilesExecuted > 0) {
			float executionTimePerCard = executionDurationTime / numberOfFilesExecuted / (float)1000000;
			wstring averageExecutionTimePerCard = systemMethods->ToWString(executionTimePerCard, showTimeInSeconds ? 2 : 8);
			wcout << L"That's " + averageExecutionTimePerCard + L" seconds per card on average!" << endl;
		}

		wcout << endl;
	}
}