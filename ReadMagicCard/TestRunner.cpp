#include "stdafx.h"
#include "TestRunner.h"
#include "CardTestRunner.h"
#include "AlgorithmTestRunner.h"
#include <iostream>

using namespace std;

TestRunner::TestRunner(Toolbox* toolbox)
{
	this->toolbox = toolbox;
}

TestRunner::~TestRunner()
{
}

void TestRunner::RunTestCases(vector<Card> result) {

	//Set up testers.
	CardTestRunner cardTester(toolbox);
	AlgorithmTestRunner algorithmTester(toolbox);

	//Perform tests.
	bool cardTestsSucceded = cardTester.RunTestCases(result);
	bool algorithmTestsSucceded = algorithmTester.RunTestCases();

	//Check the test results.
	if (cardTestsSucceded && algorithmTestsSucceded) {

		toolbox->SetCommandLineTextColour(10);

		wcout << L"All test cases still works!" << endl << endl;

		toolbox->ResetCommandLineTextColour();
	}
}
