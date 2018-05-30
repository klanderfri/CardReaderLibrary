#include "stdafx.h"
#include "SystemMethods.h"
#include <codecvt>
#include "boost\lexical_cast.hpp"
#include "boost\algorithm\string.hpp"

using namespace std;
using boost::lexical_cast;

wstring SystemMethods::UTF8ToWstring(const char *utf8ToConvert)
{
	//Implemented as suggested at:
	//https://stackoverflow.com/a/17688508/1997617

	wstring_convert<codecvt_utf8<wchar_t>> wconv;
	wstring wstr = wconv.from_bytes(utf8ToConvert);

	return wstr;
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
