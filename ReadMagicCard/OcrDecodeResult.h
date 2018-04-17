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
};

