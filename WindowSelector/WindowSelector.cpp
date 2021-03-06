// WindowSelector.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "WindowSelector.h"
#include "BrowserHandler.h"
#include "WindowUtil.h"

// Global Variables:
HINSTANCE hInst;                                // current instance
HFONT hFont;										// Font handle for the window.
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
BOOL isActive = FALSE;
HWND hWndBtnStart;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Set locale.
	setlocale(LC_ALL, "en-GB");

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWSELECTOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		std::cout << "Cannot initialize application." << std::endl;
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSELECTOR));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSELECTOR));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(DC_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSELECTOR);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		0, 0, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		OutputDebugString(L"Couldn't initialize window.");
		return FALSE;
	}

	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE)&~WS_SIZEBOX&~WS_MAXIMIZEBOX);
	CenterWindow(hWnd);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// Create font.
		const WCHAR* fontName = L"Tahoma";
		const long nFontSize = 10;

		HDC hdc = GetDC(hWnd);

		LOGFONT logFont = { 0 };
		logFont.lfHeight = -MulDiv(nFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		logFont.lfWeight = FW_REGULAR;
		wcscpy_s(logFont.lfFaceName, 7, fontName);

		hFont = CreateFontIndirect(&logFont);

		ReleaseDC(hWnd, hdc);

		// Create button.
		WCHAR btnText[MAX_LOADSTRING];
		LoadStringW(hInst, IDS_BTN_START_TEXT_ACTIVE, btnText, MAX_LOADSTRING);
		hWndBtnStart = CreateWindow(L"button",
			btnText,
			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			0, 0, 200, 50,
			hWnd, (HMENU)ID_BTN_START, GetModuleHandle(NULL), NULL);

		// Set button font.
		SendMessage(hWndBtnStart, WM_SETFONT, (WPARAM)hFont, (LPARAM)MAKELONG(TRUE, 0));

		// Center button on the window.
		CenterChild(hWndBtnStart);

		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_BTN_START: {
			WCHAR btnText[MAX_LOADSTRING];

			// If the event is not registered.
			if (!isActive) {
				int result = MessageBox(hWnd,
					L"Please select the window for more information.",
					L"Select Window",
					MB_OKCANCEL | MB_DEFBUTTON1 | MB_APPLMODAL | MB_ICONINFORMATION);

				if (result == IDOK) {
					GetWindowData();
					LoadStringW(hInst, IDS_BTN_START_TEXT_INACTIVE, btnText, MAX_LOADSTRING);
					isActive = TRUE;
					OutputDebugString(L"Event handler listener attached.\n");
				}
				else {
					return 0;
				}
			}
			else {
				Release();
				LoadStringW(hInst, IDS_BTN_START_TEXT_ACTIVE, btnText, MAX_LOADSTRING);
				isActive = FALSE;
				OutputDebugString(L"Event handler listener detached.\n");
			}

			// Change button text.
			if (hWndBtnStart)
				SendMessage(hWndBtnStart, WM_SETTEXT, 0, (LPARAM)(btnText));

			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		SetDCBrushColor(hdc, RGB(255, 200, 200));
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
