#pragma once
#include "SystemMethods.h"
#include "CardNameInfo.h"
//Class running tests of the program.
class TestRunner
{
public:
	TestRunner(SystemMethods* systemMethods);
	~TestRunner();

	//Runs tests to see if there are any broken code.
	void RunTestCases(std::vector<CardNameInfo> result);

private:

	//An object holding OS-specific methods.
	SystemMethods* systemMethods;
};

