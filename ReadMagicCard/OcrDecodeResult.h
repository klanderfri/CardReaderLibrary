#pragma once
#include <string>
class OcrDecodeResult
{
public:
	OcrDecodeResult() = default;
	~OcrDecodeResult() = default;
	int Confidence = 0;
	std::wstring Text = L"";
};

