#pragma once
class Converter
{
public:
	Converter();
	~Converter();

	//Converts a string to a bool.
	bool ToBool(const std::wstring& stringToConvert);
	//Converts a string to a bool.
	bool ToBool(const std::string& stringToConvert);

	std::string ToString(const std::wstring& wstringToConvert);

	std::wstring ToWString(const std::string& stringToConvert);

	std::wstring ToWString(const float& floatToConvert, int numberOfDecimals = 5);
};

