#pragma once
#include "SessionBound.h"
//Base class for classes that handles card or part of cards.
class BasicReaderData :
	public SessionBound
{
public:
	BasicReaderData(Session* session, std::wstring imageFilePath, cv::Mat imageData);
	~BasicReaderData();

protected:

	//The name of the file with the card the reader should handle.
	std::wstring imageFileName;
	//The full file path of the file with the card the reader should handle.
	std::wstring imageFilePath;
	//The image data of the image containing the card.
	cv::Mat originalImageData;
};
