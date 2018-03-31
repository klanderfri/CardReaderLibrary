#pragma once
class CardNameInfo
{
public:
	CardNameInfo();
	~CardNameInfo();

	//Tells if the algorithm extracting the card name was successful.
	bool Success;
	//The name of the file containing the card.
	std::wstring FileName;
	//The extracted card name.
	std::wstring CardName;
};

