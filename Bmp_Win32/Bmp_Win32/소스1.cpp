#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Image Editor");

void CenterWindow(HWND);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0, 1700, 1000, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

int temps = 50;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, MemDC;
	PAINTSTRUCT ps;
	HBITMAP OldBitmap;
	HANDLE MyBitmap;
	char temp[20] = "Hello World";
	LPCWSTR tmp = (LPCWSTR)& temp;
	HWND lastwin;
	HWND g_hScroll;
	
	switch (iMessage) {
	case WM_LBUTTONDOWN:
		hdc = GetDC(hWnd);
		TextOut(hdc, 100, 100, tmp, 15);
		ReleaseDC(hWnd, hdc);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		MemDC = CreateCompatibleDC(hdc);
		MyBitmap =LoadImageW(NULL, L"C:\\Users\\N\\Links\\input.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		OldBitmap = (HBITMAP)SelectObject(MemDC, MyBitmap);
		//BitBlt(hdc, 0, 0, 1000, 1000, MemDC, 0, 0, SRCCOPY);
		//StretchBlt(hdc,0,0,246,320,MemDC,0,0,123,160,SRCCOPY);
		StretchBlt(hdc, 50, temps, 900, 900, MemDC, 0, 0, 900, 900, SRCCOPY);

		SelectObject(MemDC, OldBitmap);
		DeleteObject(MyBitmap);
		DeleteDC(MemDC);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:
			MessageBox(hWnd, L"Ok Button Clickd", L"Button", MB_OK);

			break;
		case 1:
			temps = 300;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		return 0;
	case WM_CREATE:
			CenterWindow(hWnd);
			lastwin = CreateWindow(L"button", L"ÁÂ¿ì¹ÝÀü", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1350, 50, 300, 100, hWnd, (HMENU)1, g_hInst, NULL);
			CenterWindow(lastwin);
			CreateWindow(L"button", L"»óÇÏ¹ÝÀü", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_GROUP, 1350, 200, 300, 100, hWnd, (HMENU)0, g_hInst, NULL);
			CreateWindow(L"button", L"È¸Àü", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1350, 350, 300, 100, hWnd, (HMENU)0, g_hInst, NULL);
			CreateWindow(L"button", L"¹à±â",  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 1350, 500, 300, 100, hWnd, (HMENU)0, g_hInst, NULL);

			g_hScroll = CreateWindow(L"scrollbar", L"¾È³ç", WS_CHILD | WS_VISIBLE | SBS_HORZ, 1350, 600, 300, 20, hWnd, (HMENU)1, g_hInst, NULL);
			BOOL  SetScrollRange(HWND hWnd, int nBar, int nMinPos, int nMaxPos, BOOL bRedraw);
			int SetScrollPos(HWND hWnd, int nBar, int nPos, BOOL bRedraw);
			SetScrollRange(g_hScroll, SB_CTL, 0, 255, TRUE);
			SetScrollPos(g_hScroll, SB_CTL, 0, TRUE);
			
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void CenterWindow(HWND hwnd) {
	RECT rc;
	GetWindowRect(hwnd, &rc);
	SetWindowPos(hwnd, 0,
		(GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2,
		0, 0, SWP_NOZORDER | SWP_NOSIZE);
}


