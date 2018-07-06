#include "stdafx.h"
#include "BrowserHandler.h"
#include "WindowSelector.h"
#include "Convertions.h"

// Global variables.
HWINEVENTHOOK LHook = 0;
BOOL printAllEvents = FALSE;

// Prints browser data on window selection.
void GetWindowData() {
	DWORD threadId;
	HANDLE hStop = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE hThread = CreateThread(NULL, 0, Thread, (LPVOID)hStop, 0, &threadId);

	Sleep(5000);

	SetEvent(hStop);
	WaitForSingleObject(hThread, 5000);
	CloseHandle(hThread);
	CloseHandle(hStop);
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

		// If window is chrome.
		if (wcscmp(className, getWChar(CHROME)) == 0 && (printAllEvents || wcscmp(bstrName, getWChar(CHROME_ADDRESS_BAR))) == 0) {
			WriteWindowInfo(hWnd, L"Chrome", bstrValue);
		}
		else if (wcscmp(className, getWChar(FIREFOX)) == 0 && (printAllEvents || wcscmp(bstrName, getWChar(FIREFOX_ADDRESS_BAR))) == 0) {
			WriteWindowInfo(hWnd, L"Firefox", bstrValue);
		}
		else if (wcscmp(className, getWChar(IE)) == 0) {
			WriteWindowInfo(hWnd, L"IE", bstrValue);
		}
		else {
			WriteWindowInfo(hWnd, bstrName, bstrValue);
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
			std::cout << "Something went wrong with the event object." << std::endl;
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
		fprintf(fp, "Window: %ls, Title: %ls, Url: %ls\n", app, title, value);
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