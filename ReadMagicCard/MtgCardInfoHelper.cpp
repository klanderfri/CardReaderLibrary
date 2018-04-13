#include "stdafx.h"
#include "MtgCardInfoHelper.h"
#include "AlgorithmHelper.h"
#include <cwchar>
#include "boost\algorithm\string.hpp"

using namespace std;
using namespace cv;

MtgCardInfoHelper::MtgCardInfoHelper()
{
}

MtgCardInfoHelper::~MtgCardInfoHelper()
{
}

int MtgCardInfoHelper::LettersInShortestCardName() {

	return 3;
}

Rect MtgCardInfoHelper::GetNormalTitleSectionBox(Size cardSize) {

	double xCoordinateFactor = 0.037;
	double yCoordinateFactor = 0.045;
	double widthFactor = 0.62839;
	double heightFactor = 0.068;

	return getSectionBox(cardSize, xCoordinateFactor, yCoordinateFactor, widthFactor, heightFactor);
}


Rect MtgCardInfoHelper::GetSplitTitleSectionBox(Size cardSize) {

	double xCoordinateFactor = 0.005;
	double yCoordinateFactor = 0.081;
	double widthFactor = 0.6085;
	double heightFactor = 0.088;

	return getSectionBox(cardSize, xCoordinateFactor, yCoordinateFactor, widthFactor, heightFactor);
}

Rect MtgCardInfoHelper::getSectionBox(Size cardSize, double xCoordinateFactor, double yCoordinateFactor, double widthFactor, double heightFactor) {

	int imgHeight = cardSize.height;
	int imgWidth = cardSize.width;

	int xCoordinate = (int)round(imgHeight * xCoordinateFactor);
	int yCoordinate = (int)round(imgHeight * yCoordinateFactor);
	int titleWidth = (int)round(imgHeight * widthFactor);
	int titleHeight = (int)round(imgHeight * heightFactor);

	Rect titleBox(xCoordinate, yCoordinate, titleWidth, titleHeight);
	return titleBox;
}

double MtgCardInfoHelper::CompareCardNames(const wstring name1, const wstring name2) {

	//We dont care about lower case/upper case when it comes to sorting card names.
	wstring tmp_name1 = name1;
	wstring tmp_name2 = name2;
	boost::algorithm::to_lower(tmp_name1);
	boost::algorithm::to_lower(tmp_name2);
	RemoveCharactersNotRelevantForNameSorting(tmp_name1, tmp_name1);
	RemoveCharactersNotRelevantForNameSorting(tmp_name2, tmp_name2);

	if (tmp_name1 == tmp_name2) { return 0; }

	for (size_t i = 0; i < tmp_name1.length(); i++) {

		if (i >= tmp_name2.length()) {

			//The names has the same beginning, but name2 is shorter.
			return 1; //name2 comes first.
		}

		wchar_t char1 = tmp_name1[i];
		wchar_t char2 = tmp_name2[i];

		if (char1 == char2) { continue; }

		return (char1 < char2) ? -1 : 1;
	}

	//The names has the same beginning, but name1 is shorter.
	return -1; //name1 comes first.
}

void MtgCardInfoHelper::RemoveCharactersNotRelevantForNameSorting(const wstring cardName, wstring& result) {

	vector<wchar_t> cleanedName;

	for (wchar_t letter : cardName) {

		if (letter == L'\'') { continue; }
		if (letter == L'-') { continue; }
		if (letter == L',') { continue; }

		cleanedName.push_back(letter);
	}

	cleanedName.push_back(L'\0');
	wchar_t* resultPtr = &cleanedName[0];
	result = wstring(resultPtr);
}

bool MtgCardInfoHelper::ContainsInvalidCharacters(SystemMethods* systemMethods, const wstring title) {

	//Make the title lowercase to make it easier to work with.
	string titleStr = systemMethods->ToString(title);
	boost::algorithm::to_lower(titleStr);

	vector<char> allowedCharacters
	{
		' ', '-', '\'', ',', '/', '�',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
		'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
	};

	//Check for illegal characters.
	for (char letter : titleStr) {
		if (!AlgorithmHelper::VectorContains(allowedCharacters, letter)) {
			return true;
		}
	}

	return false;
}

wstring MtgCardInfoHelper::ChangeNumbersToLetters(SystemMethods* systemMethods, const wstring title) {

	string outText = systemMethods->ToString(title);

	outText = replaceCharacters(outText, '0', 'O', 'o');
	outText = replaceCharacters(outText, '1', 'I', 'l');
	outText = replaceCharacters(outText, '2', 'Z', 'z');
	outText = replaceCharacters(outText, '5', 'S', 's');

	return systemMethods->ToWString(outText);
}

string MtgCardInfoHelper::replaceCharacters(const string text, char toReplace, char replaceWithUpper, char replaceWithLower) {

	string outText = text;
	size_t index = outText.find(toReplace);

	while (index != string::npos) {

		bool isCapitalLetter = (index == 0 || outText[index - 1] == ' ');
		char replaceWith = isCapitalLetter ? replaceWithUpper : replaceWithLower;
		outText.replace(index, 1, 1, replaceWith);

		index = outText.find(toReplace);
	}

	return outText;
}
