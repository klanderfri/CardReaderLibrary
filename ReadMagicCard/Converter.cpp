#include "stdafx.h"
#include "Converter.h"
#include "boost\algorithm\string.hpp"
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <Lmcons.h>
#include <string>
#include <sstream>

using namespace std;

Converter::Converter()
{
}

Converter::~Converter()
{
}

bool Converter::ToBool(const wstring& stringToConvert) {

	return ToBool(ToString(stringToConvert));
}

bool Converter::ToBool(const string& stringToConvert) {

	string strCopy = stringToConvert;
	boost::algorithm::to_lower(strCopy);

	if (strCopy == "true") { return true; }
	if (strCopy == "false") { return false; }
	if (strCopy == "1") { return true; }
	if (strCopy == "0") { return false; }

	throw ParameterException("The provided string cannot be converted to a boolean!", "stringToConvert");
}

string Converter::ToString(const wstring &wstringToConvert)
{
	//Implemented as suggested at:
	//https://stackoverflow.com/a/3999597/1997617

	if (wstringToConvert.empty()) { return string(); }

	size_t originalWstringSizeInCharacters = wstringToConvert.size();
	int requiredWstringBufferSizeInBytes
		= WideCharToMultiByte(CP_ACP, 0,
			&wstringToConvert[0], (int)originalWstringSizeInCharacters,
			NULL, 0,
			NULL, NULL);

	string convertedString(requiredWstringBufferSizeInBytes, 0);
	int bytesWrittenToWstringBuffer
		= WideCharToMultiByte(CP_ACP, 0,
			&wstringToConvert[0], (int)originalWstringSizeInCharacters,
			&convertedString[0], requiredWstringBufferSizeInBytes,
			NULL, NULL);

	if (bytesWrittenToWstringBuffer != originalWstringSizeInCharacters) {
		string message = "ERROR: Wrong number of characters converted!";
		throw OperationException(message);
	}

	if (bytesWrittenToWstringBuffer == 0) {
		string message = "ERROR: " + GetLastError();
		throw OperationException(message);
	}

	return convertedString;
}

wstring Converter::ToWString(const string& stringToConvert) {

	//Implemented as suggested at:
	//https://stackoverflow.com/a/3999597/1997617

	if (stringToConvert.empty()) { return wstring(); }

	size_t originalStringSizeInCharacters = stringToConvert.size();
	int requiredStringBufferSizeInBytes
		= MultiByteToWideChar(CP_ACP, 0,
			&stringToConvert[0], (int)originalStringSizeInCharacters,
			NULL, 0);

	wstring convertedWstring(requiredStringBufferSizeInBytes, 0);
	int bytesWrittenToStringBuffer
		= MultiByteToWideChar(CP_ACP, 0,
			&stringToConvert[0], (int)originalStringSizeInCharacters,
			&convertedWstring[0], requiredStringBufferSizeInBytes);

	if (bytesWrittenToStringBuffer != originalStringSizeInCharacters) {
		string message = "ERROR: Wrong number of characters converted!";
		throw OperationException(message);
	}

	if (bytesWrittenToStringBuffer == 0) {
		string message = "ERROR: " + GetLastError();
		throw OperationException(message);
	}

	return convertedWstring;
}

wstring Converter::ToWString(const float& floatToConvert, int numberOfDecimals) {

	//Implemented as suggested at
	//https://stackoverflow.com/a/29200671/1997617

	wstringstream stream;
	stream << fixed << setprecision(numberOfDecimals) << floatToConvert;
	wstring convertedString = stream.str();

	return convertedString;
}
