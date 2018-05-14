#pragma once
#include "Session.h"
class SessionBound
{
public:
	SessionBound(Session* session);
	~SessionBound();

protected:

	//The current session.
	Session* session;
};

