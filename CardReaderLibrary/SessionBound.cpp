#include "stdafx.h"
#include "SessionBound.h"

using namespace std;

SessionBound::SessionBound(Session* session)
{
	this->session = session;
}

SessionBound::~SessionBound()
{
}
