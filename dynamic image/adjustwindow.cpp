#include <windows.h>
#include <commctrl.h>
#include <string>
#include "com.h"

RECT rcClient;
extern HINSTANCE hInst;
VOID CreateTextBox(HWND hParent, TCHAR* caption,POINT ptPos,POINT ptSize,UINT ID)
{
	CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING,
		WC_STATIC,caption,WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER,
		ptPos.x,ptPos.y,
		ptSize.x/2, 25,
		hParent, NULL, hInst, NULL);
	CreateWindowEx(WS_EX_LEFT | WS_EX_CLIENTEDGE | WS_EX_CONTEXTHELP,    //Extended window styles.
		WC_EDIT,
		NULL,
		WS_CHILDWINDOW | WS_VISIBLE //| WS_BORDER    // Window styles.
		| ES_NUMBER | ES_CENTER | WS_DISABLED,                     // Edit control styles.
		ptPos.x+ptSize.x / 2, ptPos.y,
		ptSize.x/2, 25,
		hParent, (HMENU)ID, hInst, NULL);
}
VOID CreateGroupBox(HWND hwndParent, TCHAR* caption, POINT ptPos,POINT ptSize)
{
	CreateWindowEx(WS_EX_LEFT | WS_EX_CONTROLPARENT | WS_EX_LTRREADING,
		WC_BUTTON,caption,WS_CHILDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE | WS_GROUP | BS_GROUPBOX,
		ptPos.x, ptPos.y,
		ptSize.x, ptSize.y,
		hwndParent, NULL, hInst, NULL);
}
HWND WINAPI CreateTrackbar(HWND hwndDlg, UINT iMin, UINT iMax, POINT ptPos, POINT ptSize, TCHAR* szName, UINT ID)
{
	CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING,
		WC_STATIC, szName, WS_CHILDWINDOW | WS_VISIBLE | SS_CENTER,
		ptPos.x, ptPos.y,
		ptSize.x / 2, 25,
		hwndDlg, NULL, hInst, NULL);
	CreateWindowEx(WS_EX_LEFT | WS_EX_CLIENTEDGE | WS_EX_CONTEXTHELP,    //Extended window styles.
		WC_EDIT,
		NULL,
		WS_CHILDWINDOW | WS_VISIBLE //| WS_BORDER    // Window styles.
		| ES_NUMBER | ES_CENTER | WS_DISABLED,                     // Edit control styles.
		ptPos.x + ptSize.x / 2, ptPos.y,
		ptSize.x / 2, 25,
		hwndDlg, (HMENU)(ID * 20), hInst, NULL);

	HWND hwndTrack = CreateWindowEx(
		0,                               // no extended styles 
		TRACKBAR_CLASS,                  // class name 
		TEXT("Trackbar Control"),             // title (caption) 
		WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
		ptPos.x, ptPos.y + 35, ptSize.x, ptSize.y,
		hwndDlg, (HMENU)ID, hInst, NULL);

	SendMessage(hwndTrack, TBM_SETRANGE,
		(WPARAM)TRUE,                   // redraw flag 
		(LPARAM)MAKELONG(iMin, iMax));  // min. & max. positions

	SendMessage(hwndTrack, TBM_SETPAGESIZE,
		0, (LPARAM)1);                  // new page size 

	SendMessage(hwndTrack, TBM_SETPOS,
		(WPARAM)TRUE,                   // redraw flag 
		(LPARAM)0);
	SetFocus(hwndTrack);

	return hwndTrack;
}
BOOL CALLBACK EnumChildProc(HWND hWnd,LPARAM lParam)
{
	if (IsWindow(hWnd))
	{
		RECT rect;
		GetWindowRect(hWnd, &rect);
		MapWindowPoints(NULL, GetParent(hWnd), (LPPOINT)&rect, 2);
		return MoveWindow(hWnd, (INT)lParam, rect.top, 
			(rect.right - rect.left)*((INT)lParam)/rect.left, rect.bottom - rect.top, TRUE);
		//SetWindowPos(hWnd, GetParent(hWnd), (UINT)lParam, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
	} 
	return TRUE;
	//return SetWindowPos(hWnd, NULL, (UINT)lParam, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
}
