#include "stdafx.h"
#include "BrowserHandler.h"
#include "WindowSelector.h"
#include "Convertions.h"
#include "Util.h"

// Global variables.
HWINEVENTHOOK LHook = 0;
BOOL printAllEvents = FALSE;
HANDLE hStop;
HANDLE hThread;

// Prints browser data on window selection.
void GetWindowData() {
	DWORD threadId;
	hStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	hThread = CreateThread(NULL, 0, Thread, (LPVOID)hStop, 0, &threadId);
}

// Stop the theads and close handles.
void Release() {
	if (hStop && hThread) {
		SetEvent(hStop);
		WaitForSingleObject(hThread, 5000);
		CloseHandle(hThread);
		CloseHandle(hStop);
	}
}

/* Function to handle events*/
void CALLBACK WinEventProc(
	HWINEVENTHOOK hWinEventHook,
	DWORD event,
	HWND hWnd,
	LONG idObject,
	LONG idChild,
	DWORD dwEventThread,
	DWORD dwmsEventTime) {
	IAccessible* pAcc = NULL;
	VARIANT varChild;
	HRESULT hr = AccessibleObjectFromEvent(hWnd, idObject, idChild, &pAcc, &varChild);
	if ((hr == S_OK) && pAcc != NULL) {
		BSTR bstrName, bstrValue;
		pAcc->get_accValue(varChild, &bstrValue);
		pAcc->get_accName(varChild, &bstrName);

		wchar_t className[80];
		GetClassName(hWnd, className, 80);

		// If bstrName is null, return.
		if (bstrName == NULL) {
			pAcc->Release();
			return;
		}

		// If window is chrome.
		if (wcscmp(className, getWChar(CHROME)) == 0 && (printAllEvents || wcscmp(bstrName, getWChar(CHROME_ADDRESS_BAR))) == 0) {
			WriteWindowInfo(hWnd, L"Chrome", bstrValue);
		}
		else if (wcscmp(className, getWChar(FIREFOX)) == 0 && (printAllEvents || wcscmp(bstrName, getWChar(FIREFOX_ADDRESS_BAR))) == 0) {
			WriteWindowInfo(hWnd, L"Firefox", bstrValue);
		}
		else if (wcscmp(className, getWChar(IE)) == 0) {
			HandleIE(hWnd);
		}
		else if (wcscmp(className, getWChar(ADOBE_READER)) == 0) {
			WriteWindowInfo(hWnd, L"Adobe Reader", NULL);
		}
		pAcc->Release();
	}
}

void Hook() {
	if (LHook != 0) return;

	CoInitialize(NULL);
	LHook = SetWinEventHook(EVENT_OBJECT_FOCUS, EVENT_OBJECT_VALUECHANGE,
		0, WinEventProc, 0, 0, WINEVENT_SKIPOWNPROCESS);

}

void UnHook() {
	if (LHook == 0) return;

	UnhookWinEvent(LHook);
	CoUninitialize();
	LHook = 0;
}

DWORD WINAPI Thread(LPVOID pVoid) {
	HANDLE waitHandles[] = { (HANDLE)pVoid };
	BOOL leave = FALSE;

	MSG msg;
	Hook();

	do {
		DWORD waitRes = MsgWaitForMultipleObjects(1, waitHandles, FALSE, INFINITE, QS_ALLEVENTS);

		switch (waitRes) {
		case WAIT_ABANDONED:
		case WAIT_FAILED:
			OutputDebugString(L"Something went wrong with the event object.");
			break;
		case WAIT_OBJECT_0:
			leave = TRUE;
			break;
		case WAIT_OBJECT_0 + 1:
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			break;
		}
	} while (!leave);

	UnHook();

	return 0;
}

// Deprecated.
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
	wchar_t* className = new wchar_t[MAX_LOADSTRING];
	wchar_t* title = new wchar_t[MAX_LOADSTRING];
	GetClassName(hWnd, className, MAX_LOADSTRING);
	GetWindowText(hWnd, title, MAX_LOADSTRING);

	wchar_t* msgBuff = new wchar_t[MAX_LOADSTRING];

	if (wcscmp(className, getWChar(CHROME)) == 0 && wcscmp(title, L"") != 0) {
		swprintf(msgBuff, MAX_LOADSTRING, L"Chrome found. Title is : %ls\n", title);
		OutputDebugString(msgBuff);
	}
	else if (wcscmp(className, getWChar(IE)) == 0 && wcscmp(title, L"") != 0) {
		swprintf(msgBuff, MAX_LOADSTRING, L"Internet explorer found. Title is : %ls\n", title);
		OutputDebugString(msgBuff);
	}
	else if (wcscmp(className, getWChar(FIREFOX)) == 0 && wcscmp(title, L"") != 0) {
		swprintf(msgBuff, MAX_LOADSTRING, L"Firefox found. Title is : %ls\n", title);
		OutputDebugString(msgBuff);
	}
	//else {
	//	swprintf(msgBuff, MAX_LOADSTRING, L"Title : %ls\n", title);
	//	OutputDebugString(msgBuff);

	//	delete[] msgBuff;
	//	msgBuff = new wchar_t[MAX_LOADSTRING];

	//	swprintf(msgBuff, MAX_LOADSTRING, L"Class : %ls\n", className);
	//}

	// OutputDebugString(msgBuff);

	delete[] msgBuff;
	delete[] className;
	delete[] title;

	return TRUE;
}

BOOL WriteWindowInfo(HWND hWndWindow, const wchar_t* app, wchar_t* value) {
	wchar_t* title = new wchar_t[MAX_LOADSTRING];
	wchar_t* msgBuff = new wchar_t[MAX_LOADSTRING];

	try {
		// Get window title.
		GetWindowText(hWndWindow, title, MAX_LOADSTRING);

		// Write to file in appnd mode.
		FILE* fp;
		_wfopen_s(&fp, getWChar(LOG_FILE), L"a");

		if (value == NULL)
			fprintf(fp, "%ls: Window: %ls, Title: %ls\n", timestamp(), app, title);
		else
			fprintf(fp, "%ls: Window: %ls, Title: %ls, Url: %ls\n", timestamp(), app, title, value);

		fclose(fp);
	}
	catch (std::exception e) {
		swprintf(msgBuff, MAX_LOADSTRING, L"Couldn't write to file. Error is : %ls\n", getWChar(e.what()));
		OutputDebugString(msgBuff);

		delete[] title;
		delete[] msgBuff;

		return FALSE;
	}

	delete[] title;
	delete[] msgBuff;

	return FALSE;
}

void HandleIE(HWND hWnd) {
	SHDocVw::IShellWindowsPtr spSHWinds;
	IDispatchPtr spDisp;

	//Find all explorer (Windows and Internet) and list them
	if (spSHWinds.CreateInstance(__uuidof(SHDocVw::ShellWindows)) == S_OK) {
		long nCount = spSHWinds->GetCount();
		for (long i = 0; i < nCount; i++) {
			_variant_t va(i, VT_I4);
			spDisp = spSHWinds->Item(va);
			SHDocVw::IWebBrowser2Ptr spBrowser(spDisp);
			if (spBrowser != NULL) {
				IWebBrowser2 *pBrowser = (IWebBrowser2 *)spBrowser.GetInterfacePtr();

				BSTR bstrUrl = NULL, bstrTitle;
				//Get the window title
				pBrowser->get_LocationName(&bstrTitle);

				//Detect if this is Windows Explorer (My Computer) or Internet Explorer (the internet)
				IDispatchPtr spDisp;
				if (pBrowser->get_Document(&spDisp) == S_OK && spDisp != NULL) {
					MSHTML::IHTMLDocument2Ptr spHtmlDocument(spDisp);
					if (spHtmlDocument != NULL) {
						MSHTML::IHTMLElementPtr spHtmlElement;
						spHtmlDocument->get_body(&spHtmlElement);
						if (spHtmlElement != NULL) {
							// Internet explorer.
							pBrowser->get_LocationURL(&bstrUrl);

							// Get window title.
							wchar_t* title = new wchar_t[MAX_LOADSTRING];
							GetWindowText(hWnd, title, MAX_LOADSTRING);

							// Truncate the title to remove extra string.
							std::wstring ws(title);
							std::string titleStr(ws.begin(), ws.end());
							size_t len = strlen(IE_TITLE_TRUNC);
							titleStr.erase(strlen(titleStr.c_str()) - len, len);

							if (wcscmp(getWChar(titleStr.c_str()), bstrTitle) == 0)
								WriteWindowInfo(hWnd, L"Internet Explorer", bstrUrl);
						}
					}
					spDisp.Release();
				}

				SysFreeString(bstrTitle);

				if (bstrUrl != NULL)
					SysFreeString(bstrUrl);

				spBrowser.Release();
			}
		}
	}
	else {
		OutputDebugString(L"Shell windows failed to initialise");
	}
}