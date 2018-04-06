#pragma once
#include <string>
#include "Colour.h"
//Class holding methods whose implementation depends on the operating system.
class SystemMethods
{
public:
	//Convert a wide Unicode string to an UTF8 string.
	virtual std::string ToString(const std::wstring& wstringToConvert) = 0;
	//Convert an UTF8 string to a wide Unicode string.
	virtual std::wstring ToWString(const std::string& stringToConvert) = 0;
	//Converts a float to an UTF8 string.
	virtual std::wstring ToWString(const float& floatToConvert, int precision = 5) = 0;
	//Gets the user name of the current user.
	virtual std::wstring GetCurrentUserName() = 0;
	//Gets the picture folder of the current user.
	virtual std::wstring GetUserPicturesFileDirectory() = 0;
	//Adds a string to the end of the filename, before the extension.
	virtual std::wstring AddToEndOfFilename(std::wstring originalFilename, std::wstring addition) = 0;
	//Sets the width of the console window to accommodate a specific amount of characters.
	virtual void SetConsoleWidthInCharacters(int amountOfCharacters) = 0;
	//Gets the full path to the parent folder of the current running executable file (.exe in Windows).
	virtual std::wstring GetPathToExeParentDirectory() = 0;
	//Gets the full path to the current running executable file (.exe in Windows).
	virtual std::wstring GetPathToExeFile() = 0;
	//Gets the fiulkname of the current running executable file (.exe in Windows).
	virtual std::wstring GetExeFileName() = 0;
	//Sets the command line text colour to the specified color.
	virtual void SetCommandLineTextColour(Colour colour) = 0;
	//Resets the command line text colour to the OS default one.
	virtual void ResetCommandLineTextColour() = 0;
};
