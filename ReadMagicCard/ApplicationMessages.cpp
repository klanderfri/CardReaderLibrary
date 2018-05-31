#include "stdafx.h"
#include "ApplicationMessages.h"
#include <iostream>

using namespace std;

ApplicationMessages::ApplicationMessages(Toolbox* toolbox)
{
	this->toolbox = toolbox;
}

ApplicationMessages::~ApplicationMessages()
{
}

void ApplicationMessages::printWelcomeMessage() {

	if (!toolbox->runSilent) {
		wcout << L"Welcome to the eminent MtG card reader! Let's read some cards! :-D" << endl << endl;
	}
}

void ApplicationMessages::printWorkingFolderMessage(wstring mtgFolderPath) {

	if (!toolbox->runSilent) {
		wcout << L"Looking for card images in folders . . ." << endl;
		wcout << mtgFolderPath << endl << endl;
	}
}

void ApplicationMessages::printResultMessage(int numberOfErrors) {
	
	if (!toolbox->runSilent) {
		wstring message;

		if (numberOfErrors == 0) {
			toolbox->SetCommandLineTextColour(10);
			message = L"All images was successfully read! Yay! :-)";
		}
		else {
			toolbox->SetCommandLineTextColour(12);
			wstring amountOfErrorsStr = to_wstring(numberOfErrors);
			wstring errorWord = (numberOfErrors == 1) ? L"error" : L"errors";
			message = L"Oh no! There was " + amountOfErrorsStr + L" " + errorWord + L" when reading the cards! :-(";
		}
		wcout << message << endl;
		toolbox->ResetCommandLineTextColour();
	}
}

void ApplicationMessages::printNoImagesMessage() {

	if (!toolbox->runSilent) {
		toolbox->SetCommandLineTextColour(14);
		wcout
			<< L"Hmm, it semms like there are no cards... :-|"
			<< endl;
		toolbox->ResetCommandLineTextColour();
	}
}

void ApplicationMessages::printToManyFilesMessage() {

	if (!toolbox->runSilent) {
		toolbox->SetCommandLineTextColour(12);
		wcout
			<< L"Glunck! There were to many cards for me to handle! Don't feed me more than "
			+ to_wstring(toolbox->cardReader->GetMaxCardAmount())
			+ L" card image files!"
			<< endl;
		toolbox->ResetCommandLineTextColour();
	}
}

void ApplicationMessages::printExecutionTimeMessage(TimePoint startTime, TimePoint endTime, int numberOfFilesExecuted, bool showTimeInSeconds) {

	if (!toolbox->runSilent) {
		auto executionDurationTime = TimePoint::GetExecutionTime(startTime, endTime);

		//Print total execution time.
		float totalExecutionTime = executionDurationTime / (float)(showTimeInSeconds ? 1000000 : 1);
		wstring exeTime = toolbox->converter->ToWString(totalExecutionTime, showTimeInSeconds ? 1 : 0);
		wstring timeUnit = showTimeInSeconds ? L"seconds" : L"microseconds";
		wcout << endl << L"The reading took " + exeTime + L" " + timeUnit + L" to execute." << endl;

		//Print card average execution time.
		if (numberOfFilesExecuted > 0) {
			float executionTimePerCard = executionDurationTime / numberOfFilesExecuted / (float)1000000;
			wstring averageExecutionTimePerCard = toolbox->converter->ToWString(executionTimePerCard, showTimeInSeconds ? 2 : 8);
			wcout << L"That's " + averageExecutionTimePerCard + L" seconds per card on average!" << endl;
		}

		wcout << endl;
	}
}