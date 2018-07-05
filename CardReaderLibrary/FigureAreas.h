#pragma once
#include "FigureArea.h"
//Typedef for the vector type the FigureAreas are using as base.
typedef std::vector<FigureArea> FigureAreasBase;
//Class for a set of objects holding information about the area of a figure.
class FigureAreas :
	public FigureAreasBase
{
public:
	FigureAreas();
	FigureAreas(Contours contours);
	FigureAreas(const_iterator first, const_iterator last);
	~FigureAreas();

	//Checks if a letter is a child to any other letter.
	static bool HasParentFigure(FigureArea figureToCheck, FigureAreas possibleParents);

private:

	//Checks if one letter is child to another letter.
	static bool hasChildParentRelation(FigureArea child, FigureArea parent);
};
