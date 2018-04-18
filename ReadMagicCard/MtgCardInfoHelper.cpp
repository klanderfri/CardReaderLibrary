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

bool MtgCardInfoHelper::IsNameLongEnough(const wstring title) {

	int lettersInName = 0;

	for (wchar_t character : title) {

		bool isLetter = true;

		vector<wchar_t> nonLetterCharacters = GetAllowedNonLetterCharacters();
		for (wchar_t nonLetter : nonLetterCharacters) {
			if (character == nonLetter) {
				isLetter = false;
				break;
			}
		}

		if (isLetter) {
			lettersInName++;
		}

		if (lettersInName >= LettersInShortestCardName()) {
			return true;
		}
	}

	return false;
}

Rect MtgCardInfoHelper::GetNormalTitleSectionBox(Size cardSize) {

	double xCoordinateFactor = 0.037;
	double yCoordinateFactor = 0.045;
	double widthFactor = 0.62839;
	double heightFactor = 0.068;

	return getSectionBox(cardSize, xCoordinateFactor, yCoordinateFactor, widthFactor, heightFactor);
}

Rect MtgCardInfoHelper::GetSplitTitleSectionBox(Size cardSize) {

	double xCoordinateFactor = 0.0;
	double yCoordinateFactor = 0.081;
	double widthFactor = 0.6085;
	double heightFactor = 0.088;

	return getSectionBox(cardSize, xCoordinateFactor, yCoordinateFactor, widthFactor, heightFactor);
}

Rect MtgCardInfoHelper::GetAmonkhetSplitTitleSectionBox(Size cardSize) {

	double xCoordinateFactor = 0.065;
	double yCoordinateFactor = 0.061;
	double widthFactor = 0.540;
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

bool MtgCardInfoHelper::ContainsInvalidCharacters(const wstring title) {

	//Make the title lowercase to make it easier to work with.
	wstring titleCopy = title;
	boost::algorithm::to_lower(titleCopy);

	//Check for illegal characters.
	for (wchar_t letter : titleCopy) {
		if (!AlgorithmHelper::VectorContains(GetAllowedCharacters(), letter)) {
			return true;
		}
	}

	return false;
}

vector<wchar_t> MtgCardInfoHelper::GetAllowedCharacters() {

	vector<wchar_t> allowedCharacters
	{
		L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm',
		L'n', L'o', L'p', L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y', L'z', L'ö'
		//Damn those Scandinavians whith their fancy Ö!
		//At least Wizards stoped using Æ. ;-)
		//http://markrosewater.tumblr.com/post/144471532728/this-is-a-bit-weird-but-i-notice-its-the-aether
	};

	vector<wchar_t> nonLetters = GetAllowedNonLetterCharacters();
	for (wchar_t character : nonLetters) {

		allowedCharacters.push_back(character);
	}

	return allowedCharacters;
}


vector<wchar_t> MtgCardInfoHelper::GetAllowedNonLetterCharacters() {

	return vector<wchar_t> { L' ', L'-', L'\'', L',', L'/' };
}