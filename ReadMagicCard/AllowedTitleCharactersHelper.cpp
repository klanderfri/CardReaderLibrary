#include "stdafx.h"
#include "AllowedTitleCharactersHelper.h"
#include "AlgorithmHelper.h"

using namespace std;

AllowedTitleCharactersHelper::AllowedTitleCharactersHelper()
{
}

AllowedTitleCharactersHelper::~AllowedTitleCharactersHelper()
{
}

vector<wchar_t> AllowedTitleCharactersHelper::GetAll() {

	vector<wchar_t> allowedCharacters
	{
		L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i',
		L'j', L'k', L'l', L'm', L'n', L'o', L'p', L'q', L'r',
		L's', L't', L'u', L'v', L'w', L'x', L'y', L'z', L'ö'

		//Damn those Scandinavians whith their fancy Ö!
		//At least Wizards stoped using Æ. ;-)
		//http://markrosewater.tumblr.com/post/144471532728/this-is-a-bit-weird-but-i-notice-its-the-aether
	};

	allowedCharacters = AlgorithmHelper::JoinVectors(GetNonLetter(), allowedCharacters);

	return allowedCharacters;
}

vector<wchar_t> AllowedTitleCharactersHelper::GetNonLetter() {

	return AlgorithmHelper::JoinVectors(vector<wchar_t> { L' ' }, GetNonSortingRelevant());
}

vector<wchar_t> AllowedTitleCharactersHelper::GetNonSortingRelevant() {

	return vector<wchar_t> { L'-', L'\'', L',', L'/' };
}
