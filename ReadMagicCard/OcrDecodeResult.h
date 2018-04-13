#pragma once
#include "SystemMethods.h"
//Class holding information about the OCR decoding result.
class OcrDecodeResult
{
public:
	OcrDecodeResult();
	~OcrDecodeResult();

	int Confidence = 0;
	std::wstring Text = L"";

	//Checks if we got a confident decode of a MtG title.
	bool IsConfidentMtgTitle(SystemMethods* systemMethods, int minimumConfidence = 75);
};

