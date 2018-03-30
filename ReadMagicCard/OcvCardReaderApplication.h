#pragma once
class OcvCardReaderApplication
{
public:
	OcvCardReaderApplication();
	~OcvCardReaderApplication();

	//Runs the card reader application.
	int Run(const bool doDebugging, const bool runParallelized);
};

