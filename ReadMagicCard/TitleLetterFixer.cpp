#include "stdafx.h"
#include "TitleLetterFixer.h"

using namespace std;

TitleLetterFixer::TitleLetterFixer(SystemMethods* systemMethods)
{
	this->systemMethods = systemMethods;
}

TitleLetterFixer::~TitleLetterFixer()
{
}

wstring TitleLetterFixer::FixTitleLetters(const wstring title) {

	string outText = systemMethods->ToString(title);

	outText = changeNumbersToLetters(outText);
	outText = fixLtoIJ(outText);

	return systemMethods->ToWString(outText);
}

string TitleLetterFixer::changeNumbersToLetters(const string title) {

	string outText = title;

	outText = replaceCharacters(outText, '0', 'O', 'o');
	outText = replaceCharacters(outText, '1', 'I', 'l');
	outText = replaceCharacters(outText, '2', 'Z', 'z');
	outText = replaceCharacters(outText, '5', 'S', 's');
	outText = replaceCharacters(outText, '—', '-', '-');
	
	return outText;
}

string TitleLetterFixer::replaceCharacters(const string text, char toReplace, char replaceWithUpper, char replaceWithLower) {

	string outText = text;
	size_t index = outText.find(toReplace);

	while (index != string::npos) {

		bool isCapitalLetter = isFirstWordLetter(index, outText);
		char replaceWith = isCapitalLetter ? replaceWithUpper : replaceWithLower;
		outText.replace(index, 1, 1, replaceWith);

		index = outText.find(toReplace);
	}

	return outText;
}

string TitleLetterFixer::fixLtoIJ(const string title) {

	string outText = title;

	for (size_t i = 0; i < outText.length(); i++) {

		char letter = outText[i];
		if (letter != 'l') { continue; }
		if (!isFirstWordLetter(i, outText)) { continue; }

		//Fix single letter word last in title.
		if (i == outText.length() - 1) {
			outText[i] = 'I';
			continue;
		}

		char nextLetter = outText[i + 1];

		//Fix single letter word in the middle.
		if (nextLetter == ' ') {
			outText[i] = 'I';
			continue;
		}

		if (i <= outText.length() - 2) {

			char secondNextLetter = outText[i + 2];

			if (nextLetter == 'o' &&
				secondNextLetter == 'n') {
				
				outText[i] = 'I';
				continue;
			}
		}

		if (nextLetter == 'a' ||
			nextLetter == 'e' ||
			nextLetter == 'h' ||
			nextLetter == 'i' ||
			nextLetter == 'o' ||
			nextLetter == 'u' ||
			nextLetter == 'w' ||
			nextLetter == 'ö') {

			outText[i] = 'J';
			continue;
		}

		outText[i] = 'I';
	}

	return outText;
}

bool TitleLetterFixer::isFirstWordLetter(size_t indexOfLetter, string text) {

	if (indexOfLetter == 0) { return true; }

	char previousLetter = text[indexOfLetter - 1];
	bool isFirstInWord = (previousLetter == ' ');

	return isFirstInWord;
}

