#pragma once
//Class holding information about how a card image should be preprocessed.
class ReadingConfiguration
{
public:
	ReadingConfiguration();
	ReadingConfiguration(int binaryThreshold, bool rotate180Degrees);
	~ReadingConfiguration();

	int BinaryThreshold;
	bool Rotate180Degrees;
};

