#include "stdafx.h"
#include "LetterArea.h"

LetterArea::LetterArea()
{
}

LetterArea::~LetterArea()
{
}

bool LetterArea::CompareLetterAreaByXAscending(LetterArea area1, LetterArea area2)
{
	return (area1.box.center.x < area2.box.center.x);
}
