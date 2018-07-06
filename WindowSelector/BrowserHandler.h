/*
* File:   BrowserHandler.h
* Author: Saminda
*
* Created on 03 July 2018, 17:15
*/

#include "stdafx.h"

#ifndef BROWSER_CLASS_H
#define BROWSER_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_URL_LEN 2000;
#define LOG_FILE "data.log"
#define CHROME "Chrome_WidgetWin_1"
#define IE "IEFrame"
#define FIREFOX "MozillaWindowClass"

#define CHROME_ADDRESS_BAR "Address and search bar"
#define FIREFOX_ADDRESS_BAR "Search with Google or enter address"
#define IE_ADDRESS_BAR "Address and search using Bing"

	void GetWindowData();
	void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);
	void Hook();
	void UnHook();
	DWORD WINAPI Thread(LPVOID pVoid);
	BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);
	BOOL WriteWindowInfo(HWND, const wchar_t*, wchar_t*);

#ifdef __cplusplus
}
#endif

#endif /* BROWSER_CLASS_H */

