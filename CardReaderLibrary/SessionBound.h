#pragma once
#include "Session.h"
//Base class for classes needing a connection to the current card reading session.
class SessionBound
{
public:
	SessionBound(Session* session);
	~SessionBound();

protected:

	//The current session.
	Session* session;
};

