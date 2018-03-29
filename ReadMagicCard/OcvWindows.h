#pragma once
#include "OcvSystemDependencyClass.h"
class OcvWindows :
	public OcvSystemDependencyClass
{
public:
	OcvWindows();
	~OcvWindows();

	std::string ToString(const std::wstring& wstringToConvert);
	std::wstring ToWString(const std::string& stringToConvert);
	std::string ToString(const float& floatToConvert, int precision);
	std::wstring GetCurrentUserName();
	std::wstring GetUserPicturesFileDirectory();
	std::wstring AddToEndOfFilename(std::wstring originalFilename, std::wstring addition);
	void SetConsoleWidthInCharacters(int amountOfCharacters);
	std::wstring GetPathToExeParentDirectory();
	std::wstring GetPathToExeFile();
	std::wstring GetExeFileName();

private:

	std::wstring cache_currentUserName;
};
