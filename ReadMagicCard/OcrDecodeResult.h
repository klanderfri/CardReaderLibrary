#pragma once
#include <string>
//Class holding information about the OCR decoding result.
class OcrDecodeResult
{
public:
	OcrDecodeResult() = default;
	~OcrDecodeResult() = default;
	int Confidence = 0;
	std::wstring Text = L"";
};

