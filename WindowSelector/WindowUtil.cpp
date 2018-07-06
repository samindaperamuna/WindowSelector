#include "stdafx.h"
#include "WindowUtil.h"

/*
 * Center a window on its parent. Only works for child windows.
 */
BOOL CenterChild(HWND hWndChild)
{
	HWND hwndParent;
	RECT rectWindow, rectParent;

	// make the window relative to its parent
	if ((hwndParent = GetParent(hWndChild)) != NULL)
	{
		GetWindowRect(hWndChild, &rectWindow);
		GetWindowRect(hwndParent, &rectParent);

		int nWidth = rectWindow.right - rectWindow.left;
		int nHeight = rectWindow.bottom - rectWindow.top;

		int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
		int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;

		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		// make sure that the dialog box never moves outside of the screen
		if (nX < 0) nX = 0;
		if (nY < 0) nY = 0;
		if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
		if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

		MoveWindow(hWndChild, nX, nY, nWidth, nHeight, FALSE);

		return TRUE;
	}

	return FALSE;
}

BOOL CenterWindow(HWND hWndWindow) {
	RECT rectWindow;

	GetWindowRect(hWndWindow, &rectWindow);

	int nWidth = rectWindow.right - rectWindow.left;
	int nHeight = rectWindow.bottom - rectWindow.top;

	// Get screen dimentions.
	int scWidth = GetSystemMetrics(SM_CXSCREEN);
	int scHeight = GetSystemMetrics(SM_CYSCREEN);

	int startX = (scWidth / 2) - (nWidth / 2);
	int startY = (scHeight / 2) - (nHeight / 2);

	MoveWindow(hWndWindow, startX, startY, nWidth, nHeight, FALSE);

	return FALSE;
}