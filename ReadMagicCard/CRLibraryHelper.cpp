#include "stdafx.h"
#include "CRLibraryHelper.h"
#include "CardReaderLibrary.h"

using namespace CRLibrary;
using namespace std;;

CRLibraryHelper::CRLibraryHelper()
{
}

CRLibraryHelper::~CRLibraryHelper()
{
}

string CRLibraryHelper::ReadCardTitles(string input) {

	char* output = new char[MAX_RESULT_LENGTH];
	CRLibrary::ReadCardTitles(input.c_str(), output, MAX_RESULT_LENGTH);
	string result(output);

	return result;
}

bool CRLibraryHelper::CompareCardNames(string cardName1, string cardName2) {

	return CRLibrary::CompareCardNames(cardName1.c_str(), cardName2.c_str());
}

int CRLibraryHelper::GetMaxCardAmount() {

	return CRLibrary::GetMaxCardAmount();
}

string CRLibraryHelper::GetResultExample() {

	char* output = new char[MAX_RESULT_LENGTH];
	CRLibrary::GetResultExample(output, MAX_RESULT_LENGTH);
	string result(output);

	return result;
}
