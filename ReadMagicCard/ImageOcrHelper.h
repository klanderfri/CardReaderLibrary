#pragma once
#include "BasicReaderData.h"
#include "OcrDecodeResult.h"
#include <tesseract\baseapi.h>
//Class for decoding the characters in a card title image.
class ImageOcrHelper
{
public:
	ImageOcrHelper(SystemMethods* systemMethods);
	~ImageOcrHelper();

	//Decodes the image and turns it into a wstring. The result also contains the OCR API confidence.
	OcrDecodeResult DecodeImage(cv::Mat originalImageData);

private:

	//Gets the path to the folder containing the trained language data files.
	std::string getTrainedDataPath();
	//Sets up the OCR API so it can be used for reading images.
	void setupTessBaseAPI(std::string trainedDataPath);
	//Sets the current image to read.
	void setImage(cv::Mat originalImageData);
	//Gets the text in the image (formated as UTF8).
	std::wstring getUTF8Text(SystemMethods* systemMethods);
	//Removes the newlines from the end of the title.
	std::wstring removeTrailingNewline(std::wstring title);

	//The OCR API used for reading images.
	tesseract::TessBaseAPI ocr;
	//The path to the folder containing the trained language data files.
	std::string pathToTrainedData;
	//Pointer to object handling the system dependent methods.
	SystemMethods* systemMethods;
};
