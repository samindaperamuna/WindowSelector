#include "stdafx.h"
#include "WindowSelector.h"
#include "Convertions.h"
#include "Util.h"

wchar_t* timestamp() {
	time_t lt = time(NULL);
	struct tm tm;
	localtime_s(&tm, &lt);

	char* buff = new char[MAX_LOADSTRING];
	// asctime_s(buff, MAX_LOADSTRING, &tm);

	strftime(buff, 26, "%Y-%m-%d %H:%M:%S", &tm);

	// Strip off the new line.
	char *next_token;
	char* token = strtok_s(buff, "\n", &next_token);

	return getWChar(token);
}