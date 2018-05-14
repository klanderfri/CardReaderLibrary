#include "stdafx.h"
#include "ImageOcrHelper.h"

using namespace std;
using namespace cv;
using namespace tesseract;

ImageOcrHelper::ImageOcrHelper(Session* session) :
	SessionBound(session)
{
	//Setup the OCR API.
	pathToTrainedData = getTrainedDataPath();
	setupTessBaseAPI(pathToTrainedData);
}

ImageOcrHelper::~ImageOcrHelper()
{
}

OcrDecodeResult ImageOcrHelper::DecodeImage(Mat originalImageData) {

	//Set the current image to read.
	setImage(originalImageData);

	//Read the image.
	OcrDecodeResult result;
	result.Text = getUTF8Text();
	result.Confidence = ocr.MeanTextConf();
	result.Image = originalImageData;

	return result;
}

string ImageOcrHelper::getTrainedDataPath() {

	wstring exePath = session->systemMethods->GetPathToExeParentDirectory();
	string tesseractDataPath = session->systemMethods->ToString(exePath) + "tessdata";
	
	return tesseractDataPath;
}

void ImageOcrHelper::setupTessBaseAPI(string trainedDataPath) {

	ocr.Init(trainedDataPath.c_str(), "eng", OEM_TESSERACT_ONLY);
	ocr.SetPageSegMode(PSM_SINGLE_LINE);
}

void ImageOcrHelper::setImage(Mat originalImageData) {

	ocr.SetImage((uchar*)originalImageData.data, originalImageData.cols, originalImageData.rows, originalImageData.channels(), originalImageData.step1());
}

wstring ImageOcrHelper::getUTF8Text() {

	char* rawOutText = ocr.GetUTF8Text();
	wstring outText = session->systemMethods->UTF8ToWstring(rawOutText);
	outText = removeTrailingNewline(outText);

	delete[] rawOutText; //As instructed by documentation in the GetUTF8Text method.

	return outText;
}

wstring ImageOcrHelper::removeTrailingNewline(wstring title) {

	if (title.empty()) { return title; }

	int lastIndex = title.length() - 1;
	wchar_t lastCharacter = title[lastIndex];

	if (lastCharacter == '\n') {

		title = title.substr(0, lastIndex);
		return removeTrailingNewline(title);
	}

	return title;
}
