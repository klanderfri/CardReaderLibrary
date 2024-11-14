#include "stdafx.h"
#include "SystemMethods.h"
#include "boost\lexical_cast.hpp"
#include "boost\algorithm\string.hpp"
#include <Windows.h>

using namespace std;
using boost::lexical_cast;

wstring SystemMethods::UTF8ToWstring(const char *utf8ToConvert)
{
	//ChatGPT upgraded this method from C++14 to C++17...

	if (utf8ToConvert == nullptr) return L"";

	// Get the length of the wide character string.
	int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, utf8ToConvert, -1, nullptr, 0);

	// Handle conversion error.
	if (wideCharLength == 0) { return L""; }

	// Create a wstring to hold the converted characters.
	std::wstring wideString(wideCharLength, 0);

	// Perform the actual conversion.
	MultiByteToWideChar(CP_UTF8, 0, utf8ToConvert, -1, &wideString[0], wideCharLength);

	// Remove the null terminator added by MultiByteToWideChar.
	wideString.pop_back();

	return wideString;
}

wstring SystemMethods::ToWString(long double doubleToConvert, int numberOfDecimals) {

	//Rounding implemented as suggested at:
	//https://stackoverflow.com/a/1344261/1997617

	double roundFactor = pow(10, numberOfDecimals);
	double nearest = round(doubleToConvert * roundFactor) / roundFactor;

	wstring numberStr = boost::lexical_cast<wstring>(nearest);
	numberStr = numberStr.substr(0, numberStr.find(L'.') + numberOfDecimals + 1);
	return numberStr;
}

bool SystemMethods::ToBool(const string& stringToConvert) {

	string strCopy = stringToConvert;
	boost::algorithm::to_lower(strCopy);

	if (strCopy == "true") { return true; }
	if (strCopy == "false") { return false; }
	if (strCopy == "1") { return true; }
	if (strCopy == "0") { return false; }

	throw ParameterException("The provided string cannot be converted to a boolean!", "stringToConvert");
}
