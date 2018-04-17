#pragma once
class ReadingConfiguration
{
public:
	ReadingConfiguration();
	ReadingConfiguration(int binaryThreshold, bool rotate180Degrees);
	~ReadingConfiguration();

	int BinaryThreshold;
	bool Rotate180Degrees;
};

