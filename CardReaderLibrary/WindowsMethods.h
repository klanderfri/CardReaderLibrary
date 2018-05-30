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
	std::wstring AddToEndOfFilename(std::wstring originalFilename, std::wstring addition);
	std::wstring GetPathToExeParentDirectory();
	std::wstring GetPathToExeFile();
	std::wstring GetExeFileName();
};
