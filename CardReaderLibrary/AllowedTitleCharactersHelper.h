#pragma once
//Class holding information about which characters are allowed in a card name.
class AllowedTitleCharactersHelper
{
public:
	AllowedTitleCharactersHelper();
	~AllowedTitleCharactersHelper();

	//Gets the characters that are allowed in a card name.
	std::vector<wchar_t> GetAll();
	//Gets the non-letter characters that are allowed in a card name.
	std::vector<wchar_t> GetNonLetter();
	//Gets the characters that are ignored when sorting/comparing card names.
	std::vector<wchar_t> GetNonSortingRelevant();
};

