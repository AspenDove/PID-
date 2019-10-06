// dynamic image.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "dynamic image.h"
#include "com.h"
#include <commctrl.h>

#define MAX_LOADSTRING 100
#define ITB_SPEED 100
#define ITB_KP 101
#define ITB_TI 102
#define ITB_TD 103
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND WINAPI CreateTrackbar(HWND hwndDlg, UINT iMin, UINT iMax, POINT ptPos, POINT ptSize, TCHAR* szName, UINT ID);
VOID CreateGroupBox(HWND hwndParent, TCHAR* caption, POINT ptPos, POINT ptSize);
VOID CreateTextBox(HWND hParent, TCHAR* caption, POINT ptPos, POINT ptSize, UINT ID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DYNAMICIMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if(!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DYNAMICIMAGE));

	MSG msg;

	// 主消息循环: 
	while(GetMessage(&msg, nullptr, 0, 0))
	{
		if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DYNAMICIMAGE));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DYNAMICIMAGE);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if(!hWnd) return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

BOOL CALLBACK EnumChildProc(
	HWND hwnd, // handle to child window 
	LPARAM lParam // application-defined value 
);
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
char COM[20] = "COM4";
#define RANGE 8000.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient;
	static double t = 0.0;
	static bool first = true;
	static HDC hdcMem, hdc;
	static HBITMAP hBitmap;
	static int scale = 2;
	static rec r;
	switch(message)
	{
	case WM_CREATE:
	{
		//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);

		OpenPort(COM);
		SetupDCB(115200, hComm);  //这里后期可以设置串口波特率
		SetupTimeout(0, 0, 0, 0, 0);    //不设置读、写超时
		PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);  //清空串口1缓冲区
		SetTimer(hWnd, 1, 2, NULL);
		return 0;
	}
	case WM_HSCROLL:
	{
		switch(LOWORD(wParam))
		{
		case TB_PAGEUP:
			SendMessage((HWND)lParam, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)((SendMessage((HWND)lParam, TBM_GETPOS, 0, 0)) + 1));
			break;
		case TB_PAGEDOWN:
			SendMessage((HWND)lParam, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)((SendMessage((HWND)lParam, TBM_GETPOS, 0, 0)) - 1));
			break;
		}
		TCHAR s[10];
		char a[20];
		swprintf_s(s, TEXT("%d"), SendMessage((HWND)lParam, TBM_GETPOS, 0, 0));
		SendMessage(GetDlgItem(hWnd, GetDlgCtrlID((HWND)lParam) * 20), WM_SETTEXT, (WPARAM)TRUE, (LPARAM)s);

		for(int i = 1; i != 8; ++i)
		{
			unsigned x = SendMessage(GetDlgItem(hWnd, i), TBM_GETPOS, 0, 0);
			a[(i - 1) * 2] = (x & 0x0000ff00) >> 8;
			a[(i - 1) * 2 + 1] = (x & 0x000000ff);
		}
		WriteHEX(a, 14, hComm);
		break;
	}
	case WM_SIZE:
	{
		if(!first)
		{
			float ratio = (float)LOWORD(lParam)*4.f / 5.f / (float)cxClient;
			EnumChildWindows(hWnd, (WNDENUMPROC)[](HWND hWnd, LPARAM lParam)
			{
				RECT rect;
				GetWindowRect(hWnd, &rect);
				MapWindowPoints(NULL, GetParent(hWnd), (LPPOINT)&rect, 2);
				return MoveWindow(hWnd, rect.left**(float*)lParam, rect.top,
					(rect.right - rect.left)**(float*)lParam, rect.bottom - rect.top, TRUE);
			}, (LPARAM)&ratio);
		}
		StretchBlt(hdc, 0, 0, LOWORD(lParam) * 4 / 5, HIWORD(lParam), hdcMem, 0, 0, cxClient, cyClient, SRCCOPY);
		StretchBlt(hdcMem, 0, 0, LOWORD(lParam) * 4 / 5, HIWORD(lParam), hdcMem, 0, 0, cxClient, cyClient, SRCCOPY);
		cyClient = HIWORD(lParam);
		cxClient = LOWORD(lParam) * 4 / 5;
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_TIMER:
		hdc = GetDC(hWnd);
		if(first)
		{
			CreateGroupBox(hWnd, TEXT("调整"), POINT{ cxClient,0 }, POINT{ cxClient / 4 ,350 + 240 });
			CreateTrackbar(hWnd, 0, 10000, POINT{ cxClient + 10 , 25 }, POINT{ cxClient / 4 - 20,40 }, TEXT("速度"), 1);
			CreateTrackbar(hWnd, 0, 10000, POINT{ cxClient + 10 , 105 }, POINT{ cxClient / 4 - 20,40 }, TEXT("Kp"), 2);
			CreateTrackbar(hWnd, 0, 10000, POINT{ cxClient + 10 , 185 }, POINT{ cxClient / 4 - 20,40 }, TEXT("Ti"), 3);
			CreateTrackbar(hWnd, 0, 10000, POINT{ cxClient + 10 , 265 }, POINT{ cxClient / 4 - 20,40 }, TEXT("Td"), 4);

			CreateTrackbar(hWnd, 0, 10000, POINT{ cxClient + 10 , 345 }, POINT{ cxClient / 4 - 20,40 }, TEXT("Kp"), 5);
			CreateTrackbar(hWnd, 0, 10000, POINT{ cxClient + 10 , 425 }, POINT{ cxClient / 4 - 20,40 }, TEXT("Ti"), 6);
			CreateTrackbar(hWnd, 0, 10000, POINT{ cxClient + 10 , 505 }, POINT{ cxClient / 4 - 20,40 }, TEXT("Td"), 7);

			CreateGroupBox(hWnd, TEXT("参数"), POINT{ cxClient,600 }, POINT{ cxClient / 4 ,150 });
			CreateTextBox(hWnd, TEXT("速度"), POINT{ cxClient + 10 ,625 }, POINT{ cxClient / 4 - 20,20 }, 8);
			CreateTextBox(hWnd, TEXT("电流"), POINT{ cxClient + 10 ,425 + 240 }, POINT{ cxClient / 4 - 20,20 }, 9);
			CreateTextBox(hWnd, TEXT("温度"), POINT{ cxClient + 10 ,465 + 240 }, POINT{ cxClient / 4 - 20,20 }, 10);

			hdcMem = CreateCompatibleDC(hdc);
			hBitmap = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
			SelectObject(hdcMem, hBitmap);
			Rectangle(hdcMem, -1, -1, cxClient + 1, cyClient + 1);
			MoveToEx(hdcMem, cxClient - scale, (int)(cyClient - 0 * cyClient / RANGE), NULL);
			first = false;
		}
		r = ReciveString(hComm);
		t = r.iSpeed;

		SendMessage(GetDlgItem(hWnd, 8), WM_SETTEXT, (WPARAM)TRUE, (LPARAM)(to_wstring(r.iSpeed) + TEXT("RPM")).c_str());
		SendMessage(GetDlgItem(hWnd, 9), WM_SETTEXT, (WPARAM)TRUE, (LPARAM)to_wstring(r.iCurrent).c_str());
		SendMessage(GetDlgItem(hWnd, 10), WM_SETTEXT, (WPARAM)TRUE, (LPARAM)(to_wstring(r.iTemperature) + TEXT("℃")).c_str());

		FillRect(hdcMem, &RECT(RECT{ cxClient - scale , 0, cxClient, cyClient }), (HBRUSH)GetStockObject(WHITE_BRUSH));

		LineTo(hdcMem, cxClient - 1, (int)(cyClient - t*cyClient / RANGE));
		MoveToEx(hdcMem, cxClient - scale - 1, (int)(cyClient - t*cyClient / RANGE), NULL);

		BitBlt(hdc, 0, 0, cxClient, cyClient, hdcMem, 0, 0, SRCCOPY);
		BitBlt(hdcMem, 0, 0, cxClient - scale, cyClient, hdc, scale, 0, SRCCOPY);

		ReleaseDC(hWnd, hdc);
		break;
	case WM_DESTROY:
		DeleteDC(hdcMem);
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch(message)
	{
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hDlg, IDC_EDIT));
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			GetWindowTextA(GetDlgItem(hDlg, IDC_EDIT), COM, 20);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
