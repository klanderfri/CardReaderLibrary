#include "stdafx.h"
#include "OcvWindows.h"
#include <iomanip>
#include <windows.h>
#include <Lmcons.h>

using namespace std;

OcvWindows::OcvWindows()
{
	//Make sure the cache for the current user name is cleare.
	cache_currentUserName = L"";
}

OcvWindows::~OcvWindows()
{
}

string OcvWindows::ToString(const wstring &wstringToConvert)
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
		throw OcvOperationException(message);
	}

	if (bytesWrittenToWstringBuffer == 0) {
		string message = "ERROR: " + GetLastError();
		throw OcvOperationException(message);
	}

	return convertedString;
}

wstring OcvWindows::ToWString(const string& stringToConvert) {

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
		throw OcvOperationException(message);
	}

	if (bytesWrittenToStringBuffer == 0) {
		string message = "ERROR: " + GetLastError();
		throw OcvOperationException(message);
	}

	return convertedWstring;
}

string OcvWindows::ToString(const float& floatToConvert, int precision) {

	//Implemented as suggested at
	//https://stackoverflow.com/a/29200671/1997617

	stringstream stream;
	stream << fixed << setprecision(precision) << floatToConvert;
	string convertedString = stream.str();

	return convertedString;
}

wstring OcvWindows::GetCurrentUserName() {

	if (cache_currentUserName.empty())
	{
		//Get username.
		wchar_t username[UNLEN + 1];
		DWORD username_len = UNLEN + 1;
		GetUserNameW(username, &username_len);

		cache_currentUserName = wstring(username);
	}

	return cache_currentUserName;
}

wstring OcvWindows::GetUserPicturesFileDirectory() {

	return L"C:\\Users\\" + GetCurrentUserName() + L"\\Pictures\\";
}

wstring OcvWindows::AddToEndOfFilename(wstring originalFilename, wstring addition) {

	size_t pointPosition = originalFilename.rfind('.');
	wstring filename = originalFilename.substr(0, pointPosition);
	wstring extension = originalFilename.substr(pointPosition);
	wstring newFilename = filename + addition + extension;

	return newFilename;
}

void OcvWindows::SetConsoleWidthInCharacters(int amountOfCharacters) {

	//Implemented as suggested at
	//https://stackoverflow.com/a/29197930/1997617
	HANDLE Hout = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX Font = { sizeof(Font) };
	GetCurrentConsoleFontEx(Hout, FALSE, &Font);

	//Implemented as suggested at
	//https://stackoverflow.com/a/21238849/1997617
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //Stores the console's current dimensions.
	double phi = (1 + sqrt(5)) / 2;
	int width = amountOfCharacters * Font.dwFontSize.X + 33; //Lets hope the user doesn't change the font size!
	int height = (int)round(width / phi); //Use the golden ratio! :-D
	MoveWindow(console, r.left, r.top, width, height, TRUE);
}
