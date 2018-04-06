#include "stdafx.h"
#include "ImageOcrHelper.h"

using namespace std;
using namespace cv;
using namespace tesseract;

ImageOcrHelper::ImageOcrHelper()
{
}

ImageOcrHelper::~ImageOcrHelper()
{
}

pair<wstring, int> ImageOcrHelper::DecodeTitle(Mat originalImageData, SystemMethods* systemMethods) {

	string trainedDataFolderPath = systemMethods->ToString(systemMethods->GetPathToExeParentDirectory()) + "tessdata";
	const char* path = trainedDataFolderPath.c_str();
	tesseract::TessBaseAPI ocr;
	ocr.Init(path, "eng", OEM_TESSERACT_ONLY);
	ocr.SetPageSegMode(PSM_SINGLE_LINE);
	ocr.SetImage((uchar*)originalImageData.data, originalImageData.cols, originalImageData.rows, originalImageData.channels(), originalImageData.step1());

	char* rawOutText = ocr.GetUTF8Text();
	wstring outText = systemMethods->ToWString(string(rawOutText));
	delete[] rawOutText; //As instructed by documentation in the GetUTF8Text method.
	outText = removeTrailingNewline(outText);

	int confidence = ocr.MeanTextConf();

	return make_pair(outText, confidence);
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
