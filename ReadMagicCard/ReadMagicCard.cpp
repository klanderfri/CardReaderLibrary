// ReadMagicCard.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ReadMagicCard.h"
#include "OcvCardReaderApplication.h"

int main(int argc, char** argv)
{
	bool doDebugging = true;
	bool runParallelized = true;

	OcvCardReaderApplication application;
	int result = application.Run(doDebugging, runParallelized);

	return result;
}
