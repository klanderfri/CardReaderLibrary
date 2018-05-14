#pragma once
#include "SessionBound.h"
class TitleLetterFixer :
	public SessionBound
{
public:
	TitleLetterFixer(Session* session);
	~TitleLetterFixer();

	//Fixes incorrectly decoded characters in a title.
	std::wstring FixTitleLetters(const std::wstring title);

private:

	//Changes numbers in a title to letters (0 to O, 1 to l or I and so on).
	std::string changeNumbersToLetters(const std::string title);
	//Method replacing a specified character with another.
	std::string replaceCharacters(const std::string text, char toReplace, char replaceWithUpper, char replaceWithLower);
	//Replaces cases of lowercase L that should be I or J.
	std::string fixLtoIJ(const std::string title);
	//Replaces 'Æ' with "Ae".
	std::string fixLigature(const std::string title);
	//Checks if a letter is the first letter in a word.
	bool isFirstWordLetter(size_t indexOfLetter, std::string text);
};

