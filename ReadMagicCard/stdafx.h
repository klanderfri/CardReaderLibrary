// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here

#include "OcvException.h"
#include "OcvImageHelper.h"
#include <opencv2\core.hpp>



// Ugly hacks here! Keep them in one place!

#pragma comment(lib, "ws2_32.lib") //Seems like Tesseract needs this. (https://github.com/Microsoft/vcpkg/issues/2435)
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE 1 //Vcpkg apparantly doesn't have the latest version of boost so suppress the version warnings for now.


//Read the ReadMe.txt file before coding!
