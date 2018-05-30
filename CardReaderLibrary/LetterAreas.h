#pragma once
#include "LetterArea.h"
//Typedef for the vector type the LetterAreas are using as base.
typedef std::vector<LetterArea> LetterAreasBase;
//Class for a set of objects holding information about the area of a letter.
class LetterAreas :
	public LetterAreasBase
{
public:
	LetterAreas();
	LetterAreas(Contours contours);
	LetterAreas(const_iterator first, const_iterator last);
	~LetterAreas();

	//Checks if a letter is a child to any other letter.
	static bool HasParentLetter(LetterArea letterToCheck, LetterAreas possibleParents);

private:

	//Checks if one letter is child to another letter.
	static bool hasChildParentRelation(LetterArea child, LetterArea parent);
};
