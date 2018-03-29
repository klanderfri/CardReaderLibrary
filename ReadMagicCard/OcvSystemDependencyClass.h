#pragma once
#include<string>
//Class holding methods whose implementation depends on the operating system.
class OcvSystemDependencyClass
{
public:
	//Convert a wide Unicode string to an UTF8 string.
	virtual std::string ToString(const std::wstring& wstringToConvert) = 0;
	//Convert an UTF8 string to a wide Unicode string.
	virtual std::wstring ToWString(const std::string& stringToConvert) = 0;
	//Converts a float to an UTF8 string.
	virtual std::string ToString(const float& floatToConvert, int precision = 5) = 0;
	//Gets the user name of the current user.
	virtual std::wstring GetCurrentUserName() = 0;
	//Gets the picture folder of the current user.
	virtual std::wstring GetUserPicturesFileDirectory() = 0;
	//Adds a string to the end of the filename, before the extension.
	virtual std::wstring AddToEndOfFilename(std::wstring originalFilename, std::wstring addition) = 0;
	//Sets the width of the console window to accommodate a specific amount of characters.
	virtual void SetConsoleWidthInCharacters(int amountOfCharacters) = 0;
};
