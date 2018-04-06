#pragma once
#include "BasicReaderData.h"
#include <tesseract\baseapi.h>
//Class for decoding the characters in a card title image.
class ImageOcrHelper
{
public:
	ImageOcrHelper();
	~ImageOcrHelper();

	//Decodes the title image and turns it into a wstring. The result also contains the Tesseract confidence.
	static std::pair<std::wstring, int> DecodeTitle(cv::Mat originalImageData, SystemMethods* systemMethods);

private:

	//Removes the newlines from the end of the title.
	static std::wstring removeTrailingNewline(std::wstring title);
};
