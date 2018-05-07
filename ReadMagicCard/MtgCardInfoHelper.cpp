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

	if (title.size() < (size_t)LettersInShortestCardName()) { return false; }

	int lettersInName = 0;

	for (wchar_t character : title) {

		bool isLetter = !AlgorithmHelper::VectorContains(GetAllowedNonLetterCharacters(), character);

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
	tmp_name1 = RemoveCharactersNotRelevantForNameSorting(tmp_name1);
	tmp_name2 = RemoveCharactersNotRelevantForNameSorting(tmp_name2);

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

wstring MtgCardInfoHelper::RemoveCharactersNotRelevantForNameSorting(const wstring cardName) {

	wstring cleanedName;

	for (size_t i = 0; i < cardName.size(); i++)
	{
		wchar_t letter = cardName[i];

		bool isIrrelevant = AlgorithmHelper::VectorContains(GetAllowedNotRelevantForSortingCharacters(), letter);
		if (isIrrelevant) { continue; }

		bool isSecondWhitespace = (i > 0 && cleanedName[cleanedName.size() - 1] == L' ' && cardName[i] == L' ');
		if (isSecondWhitespace) { continue; }

		cleanedName += letter;
	}

	return cleanedName;
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

	allowedCharacters = AlgorithmHelper::JoinVectors(GetAllowedNonLetterCharacters(), allowedCharacters);

	return allowedCharacters;
}

vector<wchar_t> MtgCardInfoHelper::GetAllowedNonLetterCharacters() {

	return AlgorithmHelper::JoinVectors(vector<wchar_t> { L' ' }, GetAllowedNotRelevantForSortingCharacters());
}

vector<wchar_t> MtgCardInfoHelper::GetAllowedNotRelevantForSortingCharacters() {

	return vector<wchar_t> { L'-', L'\'', L',', L'/' };
}

bool MtgCardInfoHelper::IsEmblem(const wstring title) {

	//Emblems could be considered a subset of tokens.
	//Perhaps not from a judge's point of view but
	//both token and emblems are uppercase on black background.
	if (!IsToken(title)) { return false; }

	//Titles at emblems are always at least six letters long.
	size_t titleSize = title.size();
	if (titleSize < 6) { return false; }

	//All emblems has a title starting with the word "Emblem".
	wstring titleBeginning = title.substr(0, 6);
	boost::algorithm::to_lower(titleBeginning);
	bool isEmblem = (titleBeginning == L"emblem");
	
	//Handle the "Emblem of the Warmind" special case.
	wstring emblemOfTheWarmind = L"Emblem of the Warmind";
	size_t emblemOfTheWarmindSize = emblemOfTheWarmind.size();
	if (titleSize >= emblemOfTheWarmindSize) {
		wstring lowercaseTitle = title;
		boost::algorithm::to_lower(lowercaseTitle);
		wstring titleBeginning = lowercaseTitle.substr(0, emblemOfTheWarmindSize);
		if (titleBeginning == emblemOfTheWarmind) {
			return false;
		}
	}

	return isEmblem;
}

bool MtgCardInfoHelper::IsToken(const wstring title) {

	if (!IsNameLongEnough(title)) { return false; }
	if (ContainsInvalidCharacters(title)) { return false; }
	if (!IsMostlyUppercase(title)) { return false; }

	return true;
}

bool MtgCardInfoHelper::IsMostlyUppercase(const wstring title) {

	vector<wchar_t> nonLetters = GetAllowedNonLetterCharacters();
	int lowercase = 0;
	int uppercase = 0;

	for (wchar_t letter : title) {

		if (isupper(letter)) {
			uppercase++;
		}
		else if (islower(letter)) {
			lowercase++;
		}
		else if (AlgorithmHelper::VectorContains(nonLetters, letter)) {
			continue;
		}
		else {
			string letterNumber = to_string(letter);
			string message = "Could not determine if the letter number '" + letterNumber + "' is uppercase or lowercase!";
			throw OperationException(message);
		}
	}

	return uppercase > lowercase;
}
