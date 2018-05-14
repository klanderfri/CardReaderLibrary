#pragma once
#include "SessionBound.h"
#include "CardNameInfo.h"
//Class running tests of the program.
class TestRunner :
	public SessionBound
{
public:
	TestRunner(Session* session);
	~TestRunner();

	//Runs tests to see if there are any broken code.
	void RunTestCases(std::vector<CardNameInfo> result);
};

