#pragma once
#include "SystemMethods.h"
//Class holding information about the OCR decoding result.
class OcrDecodeResult
{
public:
	OcrDecodeResult();
	~OcrDecodeResult();

	//The confidence of the decode.
	int Confidence = 0;
	//The decoded text.
	std::wstring Text = L"";
	//The image the text was decoded from.
	cv::Mat Image;
};

