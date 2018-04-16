#include "stdafx.h"
#include "OcrDecodeResult.h"
#include "MtgCardInfoHelper.h"

OcrDecodeResult::OcrDecodeResult()
{
}

OcrDecodeResult::~OcrDecodeResult()
{
}

bool OcrDecodeResult::IsConfidentMtgTitle(SystemMethods* systemMethods, int minimumConfidence) {

	//The Tesseract algorithm needs to be confident with the result otherwise we shouldn't either.
	if (Confidence < minimumConfidence) { return false; }

	//The shortest card name is three letters.
	if (!MtgCardInfoHelper::IsNameLongEnough(Text)) { return false; }

	//Check for illegal characters.
	if (MtgCardInfoHelper::ContainsInvalidCharacters(systemMethods, Text)) { return false; }

	return true;
}
