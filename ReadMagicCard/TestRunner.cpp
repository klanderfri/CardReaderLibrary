#include "stdafx.h"
#include "TestRunner.h"
#include "CardTestRunner.h"
#include "AlgorithmTestRunner.h"
#include <iostream>

using namespace std;

TestRunner::TestRunner(SystemMethods* systemMethods)
{
	this->systemMethods = systemMethods;
}

TestRunner::~TestRunner()
{
}

void TestRunner::RunTestCases(vector<CardNameInfo> result) {

	//Set up testers.
	CardTestRunner cardTester(systemMethods);
	AlgorithmTestRunner algorithmTester(systemMethods);

	//Perform tests.
	bool cardTestsSucceded = cardTester.RunTestCases(result);
	bool algorithmTestsSucceded = algorithmTester.RunTestCases();

	//Check the test results.
	if (cardTestsSucceded && algorithmTestsSucceded) {

		systemMethods->SetCommandLineTextColour(Colour::Green);

		wcout << L"All test cases still works!" << endl << endl;

		systemMethods->ResetCommandLineTextColour();
	}
}
