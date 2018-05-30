#include "stdafx.h"
#include "ReadingConfiguration.h"

using namespace std;

ReadingConfiguration::ReadingConfiguration()
{
}

ReadingConfiguration::ReadingConfiguration(int binaryThreshold, bool rotate180Degrees)
{
	this->BinaryThreshold = binaryThreshold;
	this->Rotate180Degrees = rotate180Degrees;
}

ReadingConfiguration::~ReadingConfiguration()
{
}
