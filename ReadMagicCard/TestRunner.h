#pragma once
#include "Toolbox.h"
#include "Card.h"
//Class running tests of the program.
class TestRunner
{
public:
	TestRunner(Toolbox* toolbox);
	~TestRunner();

	//Runs tests to see if there are any broken code.
	void RunTestCases(std::vector<Card> result);

private:

	//Pointer to object holding useful data/methods.
	Toolbox* toolbox;
};

