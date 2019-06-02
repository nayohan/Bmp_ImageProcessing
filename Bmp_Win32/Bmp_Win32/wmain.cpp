#include <windows.h>
#include "Image.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildLeftProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildRightProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND hWndMain, hc1, hc2;
LPCTSTR lpszClass = TEXT("Image Editor");
HWND hScroll1, hScroll2;
HDC hdc, MemDC;

#define MIRROR 100
#define FLIP 101
#define CROP 102
#define ROTATE 103
#define BRIGHTNESS 104
#define CONTRAST 105
#define GRAYFILTER 106
#define SEPIAFILTER 107
#define RESET 108
#define EXIT 109
#define BOX 100
extern int width;
extern int height;
int sc1_pos = 0, sc2_pos = 0;
int x1, x2, y1, y2;

void CenterWindow(HWND hwnd) {
	RECT rc;
	GetWindowRect(hwnd, &rc);
	SetWindowPos(hwnd, 0,
		(GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2,
		0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	read_image();
	reset();
	write_image();

	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	
	//메인 윈도우
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	//왼쪽 차일드
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpfnWndProc = ChildLeftProc;
	WndClass.lpszClassName = L"ChildLeft";
	RegisterClass(&WndClass);

	//오른쪽 차일드
	WndClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	WndClass.lpfnWndProc = ChildRightProc;
	WndClass.lpszClassName = L"ChildRight";
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0, 1700, 1000, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	RECT crt;
	switch (iMessage) {
	case WM_CREATE:
		hc1 = CreateWindow(L"ChildLeft", NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc2 = CreateWindow(L"ChildRight", NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		return 0;
	case WM_SIZE:
		GetClientRect(hWnd, &crt);
		MoveWindow(hc1, 0, 0, crt.right-400, crt.bottom, TRUE);
		MoveWindow(hc2, crt.right-400, 0, 400, crt.bottom, TRUE);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildLeftProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	
	PAINTSTRUCT ps;
	HBITMAP OldBitmap;
	HANDLE MyBitmap;
	char temp[20] = "Hello World";
	LPCWSTR tmp = (LPCWSTR)& temp;
	
	char str[128];
	switch (iMessage) {
	case WM_LBUTTONDOWN:
		x1 = LOWORD(lParam);
		y1 = HIWORD(lParam);
		hdc = GetDC(hWnd);
		wsprintf((LPWSTR)str, L"x1_Pos: %d, y1_Pos: %d", x1, y1);
		TextOut(hdc, 100, 50, (LPWSTR)str, 25);
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		MemDC = CreateCompatibleDC(hdc);
		MyBitmap = LoadImageW(NULL, L"output.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		OldBitmap = (HBITMAP)SelectObject(MemDC, MyBitmap);
		//BitBlt(hdc, 0, 0, 1000, 1000, MemDC, 0, 0, SRCCOPY);
		//StretchBlt(hdc,0,0,246,320,MemDC,0,0,123,160,SRCCOPY);
		StretchBlt(hdc, 650 - (height / 2), 500 - (width / 2), width, height, MemDC, 0, 0, width, height, SRCCOPY);
		if(height)
		wsprintf((LPWSTR)str, L"Sc1_pos is %d", sc1_pos);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 100, 100, (LPWSTR)str, 15);

		wsprintf((LPWSTR)str, L"Sc2_pos is %d", sc2_pos);
		
		TextOut(hdc, 100, 150, (LPWSTR)str, 15);




		SelectObject(MemDC, OldBitmap);
		DeleteObject(MyBitmap);
		DeleteDC(MemDC);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY:
		free_mem();
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildRightProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HWND lastwin;
	
	char str[20];

	int TempPos = 0;
	switch (iMessage) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case MIRROR:
			mirror();
			write_image();
			InvalidateRect(hc1, NULL, TRUE);
			break;
		case FLIP:
			flip();
			write_image();
			InvalidateRect(hc1, NULL, TRUE);
			break;
		case ROTATE:
			rotate();
			write_image();
			InvalidateRect(hc1, NULL, TRUE);
			break;
		case CROP:
			crop(100, 200, 100, 200);
			write_image();
			InvalidateRect(hc1, NULL, TRUE);
			break;
		case BRIGHTNESS:
			brightness(20);
			write_image();
			InvalidateRect(hc1, NULL, TRUE);
			break;
		case CONTRAST:
			contrast(90);
			write_image();
			InvalidateRect(hc1, NULL, TRUE);
			break;
		case GRAYFILTER:
			grayscale_fillter();
			write_image();
			InvalidateRect(hc1, NULL, TRUE);
			break;
		case SEPIAFILTER:
			//_filter();
			write_image();
			InvalidateRect(hc1, NULL, TRUE);
			break;
		case RESET:
			reset();
			write_image();
			InvalidateRect(hc1, NULL, TRUE);
			break;
		case EXIT:
			break;
		}
		return 0;
	case WM_CREATE:
		CenterWindow(hWnd);
		lastwin = CreateWindow(L"button", L"좌우반전", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 50, 140, 100, hWnd, (HMENU)MIRROR, g_hInst, NULL);
		CreateWindow(L"button", L"상하반전", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_GROUP, 210, 50, 140, 100, hWnd, (HMENU)FLIP, g_hInst, NULL);

		CreateWindow(L"button", L"회전", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_GROUP, 50, 170, 300, 100, hWnd, (HMENU)ROTATE, g_hInst, NULL);
		CreateWindow(L"button", L"자르기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 290, 300, 100, hWnd, (HMENU)CROP, g_hInst, NULL);
	
		CreateWindow(L"button", L"밝기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 410, 300, 80, hWnd, (HMENU)BRIGHTNESS, g_hInst, NULL);
		hScroll1 = CreateWindow(L"scrollbar", L"밝기", WS_CHILD | WS_VISIBLE | SBS_HORZ, 50, 490, 300, 20, hWnd, (HMENU)BRIGHTNESS, g_hInst, NULL);
		CreateWindow(L"button", L"대비", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 530, 300, 80, hWnd, (HMENU)CONTRAST, g_hInst, NULL);
		hScroll2 = CreateWindow(L"scrollbar", L"대비", WS_CHILD | WS_VISIBLE | SBS_HORZ, 50, 610, 300, 20, hWnd, (HMENU)CONTRAST, g_hInst, NULL);
		
		
		
		CreateWindow(L"button", L"회색필터", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 650, 300, 100, hWnd, (HMENU)GRAYFILTER, g_hInst, NULL);
		CreateWindow(L"button", L"세피아필터", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 770, 300, 100, hWnd, (HMENU)SEPIAFILTER, g_hInst, NULL);
		CreateWindow(L"button", L"리셋", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 890, 300, 100, hWnd, (HMENU)RESET, g_hInst, NULL);
		//BOOL  SetScrollRange(HWND hWnd, int nBar, int nMinPos, int nMaxPos, BOOL bRedraw);
		//int SetScrollPos(HWND hWnd, int nBar, int nPos, BOOL bRedraw);
		SetScrollRange(hScroll1, SB_CTL, -100, 100, TRUE);
		SetScrollPos(hScroll1, SB_CTL, 0, TRUE);
		SetScrollRange(hScroll2, SB_CTL, -100, 100, TRUE);
		SetScrollPos(hScroll2, SB_CTL, 0, TRUE);
		return 0;
	case WM_HSCROLL:
		if ((HWND)lParam == hScroll1) TempPos = sc1_pos;
		if ((HWND)lParam == hScroll2) TempPos = sc2_pos;
		switch (LOWORD(wParam)) {
		case SB_LINELEFT:
			TempPos = max(0, TempPos - 1);
			break;
		case SB_LINERIGHT:
			TempPos = max(0, TempPos + 1);
			break;
		case SB_PAGELEFT:
			TempPos = max(0, TempPos - 10);
			break;
		case SB_PAGERIGHT:
			TempPos = max(0, TempPos + 1);
			break;
		case SB_THUMBTRACK:
			TempPos = HIWORD(wParam);
			break;
		}
		reset();
		if ((HWND)lParam == hScroll1) {
			sc1_pos = TempPos;
			brightness(sc1_pos);
		}
		if ((HWND)lParam == hScroll2) {
			sc2_pos = TempPos;
			contrast(sc2_pos);
		}
		SetScrollPos((HWND)lParam, SB_CTL, TempPos, TRUE);
		write_image();
		InvalidateRect(hc1, NULL, TRUE);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}