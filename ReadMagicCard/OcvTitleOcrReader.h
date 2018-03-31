#pragma once
#include "OcvCardHandler.h"
#include <tesseract\baseapi.h>
//Class for decoding the characters in a card title image.
class OcvTitleOcrReader :
	OcvCardHandler
{
public:
	OcvTitleOcrReader(std::wstring imageFileName, cv::Mat originalImageData, OcvSystemDependencyClass* systemMethods, bool doDebugging);
	~OcvTitleOcrReader();

	//Decodes the title image and turns it into a wstring. The result also contains the Tesseract confidence.
	std::pair<std::wstring, int> DecodeTitle();

private:

	//Removes the newlines from the end of the title.
	std::wstring removeTrailingNewline(std::wstring title);

	//The Tesseract API to use for decoding the title.
	tesseract::TessBaseAPI ocr;
};
