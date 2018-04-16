#pragma once
#include "SystemMethods.h"
class TitleLetterFixer
{
public:
	TitleLetterFixer(SystemMethods* systemMethods);
	~TitleLetterFixer();

	std::wstring FixTitleLetters(const std::wstring title);

private:

	//Changes numbers in a title to letters (0 to O, 1 to l or I and so on).
	std::string changeNumbersToLetters(const std::string title);
	//Method replacing a specified character with another.
	std::string replaceCharacters(const std::string text, char toReplace, char replaceWithUpper, char replaceWithLower);
	//Replaces cases of lowercase L that should be I or J.
	std::string fixLtoIJ(const std::string title);
	//Checks if a letter is the first letter in a word.
	bool isFirstWordLetter(char letter, size_t index, std::string text);

	//Pointer to object handling the system dependent methods.
	SystemMethods* systemMethods;
};

