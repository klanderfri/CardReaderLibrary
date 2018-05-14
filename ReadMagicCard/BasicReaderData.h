#pragma once
#include "SessionBound.h"
//Base class for classes that handles card or part of cards.
class BasicReaderData :
	public SessionBound
{
public:
	BasicReaderData(Session* session, std::wstring imageFileName, cv::Mat imageData);
	~BasicReaderData();

protected:

	//The name of the file with the card the reader should handle.
	std::wstring imageFileName;
	//The image data of the image containing the card.
	cv::Mat originalImageData;
};
