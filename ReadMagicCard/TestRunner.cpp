#include "stdafx.h"
#include "TestRunner.h"
#include "CardTestRunner.h"
#include "AlgorithmTestRunner.h"
#include <iostream>

using namespace std;

TestRunner::TestRunner(Session* session) :
	SessionBound(session)
{
}

TestRunner::~TestRunner()
{
}

void TestRunner::RunTestCases(vector<CardNameInfo> result) {

	//Set up testers.
	CardTestRunner cardTester(session);
	AlgorithmTestRunner algorithmTester(session);

	//Perform tests.
	bool cardTestsSucceded = cardTester.RunTestCases(result);
	bool algorithmTestsSucceded = algorithmTester.RunTestCases();

	//Check the test results.
	if (cardTestsSucceded && algorithmTestsSucceded) {

		session->systemMethods->SetCommandLineTextColour(Colour::Green);

		wcout << L"All test cases still works!" << endl << endl;

		session->systemMethods->ResetCommandLineTextColour();
	}
}
