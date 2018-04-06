#pragma once
#include "SystemMethods.h"
class WindowsMethods :
	public SystemMethods
{
public:
	WindowsMethods();
	~WindowsMethods();

	std::string ToString(const std::wstring& wstringToConvert);
	std::wstring ToWString(const std::string& stringToConvert);
	std::wstring ToWString(const float& floatToConvert, int numberOfDecimals);
	std::wstring GetCurrentUserName();
	std::wstring GetUserPicturesFileDirectory();
	std::wstring AddToEndOfFilename(std::wstring originalFilename, std::wstring addition);
	void SetConsoleWidthInCharacters(int amountOfCharacters);
	std::wstring GetPathToExeParentDirectory();
	std::wstring GetPathToExeFile();
	std::wstring GetExeFileName();
	void SetCommandLineTextColour(Colour colour);
	void ResetCommandLineTextColour();

private:

	std::wstring cache_currentUserName;
};
