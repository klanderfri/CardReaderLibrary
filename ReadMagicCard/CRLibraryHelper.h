#pragma once
class CRLibraryHelper
{
public:
	CRLibraryHelper();
	~CRLibraryHelper();


	std::string ReadCardTitles(std::string input);

	bool CompareCardNames(std::string cardName1, std::string cardName2);

	int GetMaxCardAmount();

	std::string GetResultExample();


	const int MAX_RESULT_LENGTH = 5000;
};

