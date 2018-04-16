#include "stdafx.h"
#include "SystemMethods.h"
#include <codecvt>

using namespace std;

wstring SystemMethods::UTF8ToWstring(const char *utf8ToConvert)
{
	//Implemented as suggested at:
	//https://stackoverflow.com/a/17688508/1997617

	wstring_convert<codecvt_utf8<wchar_t>> wconv;
	wstring wstr = wconv.from_bytes(utf8ToConvert);

	return wstr;
}
