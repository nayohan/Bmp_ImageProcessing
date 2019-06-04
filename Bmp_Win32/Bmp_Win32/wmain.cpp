#include <windows.h>
#include <stdlib.h>
#include "Image.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildLeftProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildRight1Proc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildRight2Proc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildRight3Proc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildRight4Proc(HWND, UINT, WPARAM, LPARAM);
HWND hScroll1, hScroll2, hCrop; //스크롤 핸들, 크롭버튼 핸들
HINSTANCE g_hInst;
HWND hWndMain, hc, hc1, hc2, hc3, hc4; //레이아웃 핸들
LPCTSTR lpszClass = TEXT("Image Editor");

//이미지용
PAINTSTRUCT ps; 
HDC hdc, backMemDC, MemDC; //더블 버퍼링
HBITMAP backBitmap = NULL;
RECT window_rt;
BOOL bNow = FALSE;


#define MIRROR 100
#define FLIP 101
#define CROP 102
#define ROTATE 103
#define BRIGHTNESS 104
#define CONTRAST 105
#define GRAYFILTER 106
#define BLURFILTER 107
#define SHARPENFILTER 108
#define RESET 109
#define EXIT 110
#define BOX 100
extern int width;
extern int height;
int sc1_pos = 100, sc2_pos = 100;
int xy_flag = 0, x1=0, x2=0, y1=0, y2=0; //자르기함수용
int p_x1 = 650, p_x2 = 650, p_y1 = 500, p_y2 = 500; //페인트용


HWND gray, blur, sharpen;
int gray_pos = 0, blur_pos = 0, sharpen_pos = 0;


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
	WndClass.lpszClassName = TEXT("ChildLeft");
	RegisterClass(&WndClass);

	//오른쪽 차일드
	WndClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	WndClass.lpfnWndProc = ChildRight1Proc;
	WndClass.lpszClassName = TEXT("ChildRight1");
	RegisterClass(&WndClass);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	WndClass.lpfnWndProc = ChildRight2Proc;
	WndClass.lpszClassName = TEXT("ChildRight2");
	RegisterClass(&WndClass);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	WndClass.lpfnWndProc = ChildRight3Proc;
	WndClass.lpszClassName = TEXT("ChildRight3");
	RegisterClass(&WndClass);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	WndClass.lpfnWndProc = ChildRight4Proc;
	WndClass.lpszClassName = TEXT("ChildRight4");
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
		hc = CreateWindow(TEXT("ChildLeft"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc1 = CreateWindow(TEXT("ChildRight1"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc2 = CreateWindow(TEXT("ChildRight2"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc3 = CreateWindow(TEXT("ChildRight3"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc4 = CreateWindow(TEXT("ChildRight4"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		return 0;
	case WM_SIZE:
		GetClientRect(hWnd, &crt);
		MoveWindow(hc, 0, 0, crt.right-400, crt.bottom, TRUE);
		MoveWindow(hc1, crt.right-400, 0, 400, 330, TRUE);
		MoveWindow(hc2, crt.right-400, 330, 400, 185, TRUE);
		MoveWindow(hc3, crt.right-400, 515, 400, crt.bottom-615, TRUE);
		MoveWindow(hc4, crt.right-400, crt.bottom-100, 400, 100, TRUE);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildLeftProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {

	HANDLE MyBitmap;
	char temp[20] = "Hello World";
	LPCWSTR tmp = (LPCWSTR)& temp;
	int r_x = 650 - (width/2);
	int r_y = 500 - (height/2);

	switch (iMessage) {
	case WM_LBUTTONDOWN:
		bNow = TRUE;
		p_x1 = LOWORD(lParam);
		p_y1 = HIWORD(lParam);
		x1 = p_x1 - r_x;
		y1 = p_y1 - r_y;

		InvalidateRect(hc, NULL, TRUE);
		InvalidateRect(hc1, NULL, TRUE);
		return 0;
	case WM_MOUSEMOVE: {
		if (bNow == TRUE) {
			p_x2 = LOWORD(lParam);
			p_y2 = HIWORD(lParam);
			x2 = p_x2 - r_x;
			y2 = p_y2 - r_y;
			InvalidateRect(hc, NULL, FALSE);
			InvalidateRect(hc1, NULL, TRUE);
		}
		return 0;
	}
	case WM_LBUTTONUP:
		p_x2 = LOWORD(lParam);
		p_y2 = HIWORD(lParam);
		x2 = p_x2 - r_x;
		y2 = p_y2 - r_y;
		InvalidateRect(hc, NULL, FALSE);
		InvalidateRect(hc1, NULL, TRUE);
		bNow = FALSE;
		return 0;
	case WM_PAINT:
		//버퍼링
		hdc = BeginPaint(hWnd, &ps);

		MemDC = CreateCompatibleDC(hdc);
		MyBitmap = LoadImage(NULL, TEXT("output.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SelectObject(MemDC, MyBitmap);
		StretchBlt(hdc, r_x, r_y, width, height, MemDC, 0, 0, width, height, SRCCOPY);
		
		Rectangle(hdc, p_x1, p_y1, p_x2, p_y2);

		DeleteObject(MyBitmap);
		DeleteDC(MemDC);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		return 0;

	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildRight1Proc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HWND lastwin;
	char str[128];
	int p_width=0, p_height=0;

	switch (iMessage) {

	case WM_CREATE:
		CenterWindow(hWnd);
		lastwin = CreateWindow(TEXT("button"), TEXT("좌우반전"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 20, 175, 90, hWnd, (HMENU)MIRROR, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("상하반전"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_GROUP, 205, 20, 175, 90, hWnd, (HMENU)FLIP, g_hInst, NULL);

		CreateWindow(TEXT("button"), TEXT("회전"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_GROUP, 20, 120, 360, 90, hWnd, (HMENU)ROTATE, g_hInst, NULL);
		hCrop = CreateWindow(TEXT("button"), TEXT("자르기"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 205, 220, 175, 90, hWnd, (HMENU)CROP, g_hInst, NULL);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//hdc = GetDC(hc1);
		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);
		
		wsprintf(str, TEXT("X1 : %d    Y1 : %d"), x1, y1);
		TextOut(hdc, 30, 230, (LPCSTR)str, strlen(str));
		wsprintf(str, TEXT("X2 : %d    Y2 : %d"), x2, y2);
		TextOut(hdc, 30, 250, (LPCSTR)str, strlen(str));
		
		wsprintf(str, TEXT("크기 : %d x %d "), abs(x2-x1), abs(y2-y1));
		TextOut(hdc, 30, 290, (LPCSTR)str, strlen(str));
		//ReleaseDC(hc1, hdc);
		EndPaint(hWnd, &ps);

		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case MIRROR:
			mirror();
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			break;
		case FLIP:
			flip();
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			break;
		case ROTATE:
			rotate();
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			break;
		case CROP:
			crop(x1, x2, y1, y2);
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			InvalidateRect(hc1, NULL, TRUE);
			break;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildRight2Proc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	int TempPos = 0;

	switch (iMessage) {
		case WM_CREATE:
			CreateWindow(TEXT("button"), TEXT(" > 밝기 <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 20, 360, 20, hWnd, (HMENU)BRIGHTNESS, g_hInst, NULL);
			hScroll1 = CreateWindow(TEXT("scrollbar"), TEXT("밝기"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 20, 45, 360, 40, hWnd, (HMENU)BRIGHTNESS, g_hInst, NULL);
			CreateWindow(TEXT("button"), TEXT(" > 대비 <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 100, 360, 20, hWnd, (HMENU)CONTRAST, g_hInst, NULL);
			hScroll2 = CreateWindow(TEXT("scrollbar"), TEXT("대비"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 20, 125, 360, 40, hWnd, (HMENU)CONTRAST, g_hInst, NULL);

			SetScrollRange(hScroll1, SB_CTL, 0, 200, TRUE);
			SetScrollPos(hScroll1, SB_CTL, sc1_pos, TRUE);
			SetScrollRange(hScroll2, SB_CTL, 0, 200, TRUE);
			SetScrollPos(hScroll2, SB_CTL, sc2_pos, TRUE);
			return 0;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case BRIGHTNESS:
					brightness(20);
					write_image();
					InvalidateRect(hc, NULL, TRUE);
					break;
				case CONTRAST:
					contrast(90);
					write_image();
					InvalidateRect(hc, NULL, TRUE);
					break;
			}
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
				TempPos = max(0, TempPos + 10);
				break;
			case SB_THUMBTRACK:
				TempPos = HIWORD(wParam);
				break;
			}
			reset();
			if ((HWND)lParam == hScroll1) {
				sc1_pos = TempPos;
				brightness(sc1_pos-100);
			}
			if ((HWND)lParam == hScroll2) {
				sc2_pos = TempPos;
				contrast(sc2_pos-100);
			}
			SetScrollPos((HWND)lParam, SB_CTL, TempPos, TRUE);
			write_image();
			InvalidateRect(hc, NULL, FALSE);
			return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildRight3Proc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {

	int TempPos = 0;

	switch (iMessage) {
	case WM_CREATE:
		CreateWindow(TEXT("button"), TEXT(" > 필터 <"), WS_CHILD | WS_VISIBLE |  BS_FLAT, 5, 15, 390, 40, hWnd, (HMENU)0, g_hInst, NULL); //50
		CreateWindow(TEXT("button"), TEXT(" > 흑백 필터 <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 70, 380, 30, hWnd, (HMENU)GRAYFILTER, g_hInst, NULL); //갭20 5
		gray = CreateWindow(TEXT("scrollbar"), TEXT("RR"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 105, 380, 40, hWnd, (HMENU)GRAYFILTER, g_hInst, NULL); //갭 5 10
		CreateWindow(TEXT("button"), TEXT(" > 블러 필터 <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 160, 380, 30, hWnd, (HMENU)BLURFILTER, g_hInst, NULL); //갭 20 5
		blur = CreateWindow(TEXT("scrollbar"), TEXT("GR"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 195, 380, 40, hWnd, (HMENU)BLURFILTER, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT(" > 선명하게 <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 250, 380, 30, hWnd, (HMENU)SHARPENFILTER, g_hInst, NULL);
		sharpen = CreateWindow(TEXT("scrollbar"), TEXT("BR"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 285, 380, 40, hWnd, (HMENU)SHARPENFILTER, g_hInst, NULL);

		SetScrollRange(gray, SB_CTL, 0, 10, TRUE);
		SetScrollPos(gray, SB_CTL, gray_pos, TRUE);
		SetScrollRange(blur, SB_CTL, 0, 10, TRUE);
		SetScrollPos(blur, SB_CTL, blur_pos, TRUE);
		SetScrollRange(sharpen, SB_CTL, 0, 10, TRUE);
		SetScrollPos(sharpen, SB_CTL, sharpen_pos, TRUE);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:
			break;
		case GRAYFILTER:
			grayscale_fillter(gray_pos);
			write_image();
			InvalidateRect(hc, NULL, FALSE);
			break;
		case BLURFILTER:
			blur_fillter(blur_pos);
			write_image();
			InvalidateRect(hc, NULL, FALSE);
			break;
		case SHARPENFILTER:
			sharpen_fillter(sharpen_pos);
			write_image();
			InvalidateRect(hc, NULL, FALSE);
			break;
		}
		return 0;

	case WM_HSCROLL:
		if ((HWND)lParam == gray) TempPos = gray_pos;
		if ((HWND)lParam == blur) TempPos = blur_pos;
		if ((HWND)lParam == sharpen) TempPos = sharpen_pos;
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
		if ((HWND)lParam == gray) {
			gray_pos = TempPos;
			grayscale_fillter(gray_pos);
		}
		if ((HWND)lParam == blur) {	
			blur_pos = TempPos;
			blur_fillter(blur_pos);
		}
		if ((HWND)lParam == sharpen) {
			sharpen_pos = TempPos;
			sharpen_fillter(sharpen_pos);
		}
		write_image();
		InvalidateRect(hc, NULL, FALSE);
		SetScrollPos((HWND)lParam, SB_CTL, TempPos, TRUE);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildRight4Proc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_CREATE:

		CreateWindow(TEXT("button"), TEXT("리셋"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 5, 190, 90, hWnd, (HMENU)RESET, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("종료"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 5, 195, 90, hWnd, (HMENU)EXIT, g_hInst, NULL);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case RESET:
			reset();
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			break;
		case EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
