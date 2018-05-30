#include "stdafx.h"
#include "WindowsMethods.h"
#include <iomanip>
#include <windows.h>
#include <Lmcons.h>
#include "boost\filesystem\path.hpp"

using namespace std;

WindowsMethods::WindowsMethods()
{
}

WindowsMethods::~WindowsMethods()
{
}

string WindowsMethods::ToString(const wstring &wstringToConvert)
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

wstring WindowsMethods::ToWString(const string& stringToConvert) {

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

wstring WindowsMethods::ToWString(const float& floatToConvert, int numberOfDecimals) {

	//Implemented as suggested at
	//https://stackoverflow.com/a/29200671/1997617

	wstringstream stream;
	stream << fixed << setprecision(numberOfDecimals) << floatToConvert;
	wstring convertedString = stream.str();

	return convertedString;
}

wstring WindowsMethods::AddToEndOfFilename(wstring originalFilename, wstring addition) {

	if (addition.empty()) { return originalFilename; }

	size_t pointPosition = originalFilename.rfind('.');
	wstring filename = originalFilename.substr(0, pointPosition);
	wstring extension = originalFilename.substr(pointPosition);
	wstring newFilename = filename + addition + extension;

	return newFilename;
}

wstring WindowsMethods::GetPathToExeParentDirectory() {

	//Implemented as suggested at:
	//https://gist.github.com/yoggy/1496617
	
	boost::filesystem::path path(GetPathToExeFile());
	wstring tmp = path.parent_path().wstring();
	return path.parent_path().wstring() + L"\\";
}

wstring WindowsMethods::GetPathToExeFile() {

	//Implemented as suggested at:
	//https://gist.github.com/yoggy/1496617

	wchar_t path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	return wstring(path);
}

wstring WindowsMethods::GetExeFileName()
{
	//Implemented as suggested at:
	//https://gist.github.com/yoggy/1496617

	boost::filesystem::path path(GetPathToExeFile());
	wstring tmp = path.filename().wstring();
	return path.filename().wstring();
}
