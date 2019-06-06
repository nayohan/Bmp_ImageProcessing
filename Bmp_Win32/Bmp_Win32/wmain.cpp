#include <windows.h>
#include <stdlib.h>
#include "ImageFunction.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildLeftProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildLeft1Proc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildRight1Proc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildRight2Proc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildRight3Proc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildRight4Proc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
HWND hScroll1, hScroll2, hCrop; //��ũ�� �ڵ�, ũ�ӹ�ư �ڵ�
HWND hWndMain, hc, hc0, hc1, hc2, hc3, hc4;  //���̾ƿ� �ڵ�
HWND gray, blur, sepia; //��ũ�� �ڵ�(����)
LPCTSTR lpszClass = TEXT("Image Editor");
RECT crt;

//�̹�����
PAINTSTRUCT ps;
HDC hdc, MemDC;
RECT window_rt;
BOOL bNow = FALSE;

#define MIRROR 100
#define FLIP 101
#define CROP 102
#define ROTATE 103
#define BRIGHTNESS 104
#define CONTRAST 105
#define GRAYFILTER 106
#define BLURFILTER 108
#define SEPIAFILTER 107
#define RESET 109
#define EXIT 110
#define BOX 100

extern int width;
extern int height;
extern int byte_size;


//��ũ�ѹ� �����ǰ���
int sc1_pos = 10, sc2_pos = 10; //���,���
int xy_flag = 0, x1 = 0, x2 = 0, y1 = 0, y2 = 0; //�ڸ����Լ���
int p_x1 = 650, p_x2 = 650, p_y1 = 500, p_y2 = 500; //����Ʈ��
int gray_pos = 0, blur_pos = 0, sepia_pos = 0; //���, ���Ǿ� ��
int now_w = 0, now_h = 0; // ���� ���� �̹��� ��ġ��

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

	//���� ������
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	//���� ���ϵ�
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpfnWndProc = ChildLeftProc;
	WndClass.lpszClassName = TEXT("ChildLeft");
	RegisterClass(&WndClass);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpfnWndProc = ChildLeft1Proc;
	WndClass.lpszClassName = TEXT("ChildLeft1");
	RegisterClass(&WndClass);

	//������ ���ϵ�
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

	//������ ����
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0, 1700, 1000, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	//�޽��� ����
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) { //���� ���ν���

	switch (iMessage) { //���̾ƿ� �� 6��  ���� 2�� ���� 4��
	case WM_CREATE:
		hc = CreateWindow(TEXT("ChildLeft"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc0 = CreateWindow(TEXT("ChildLeft1"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc1 = CreateWindow(TEXT("ChildRight1"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc2 = CreateWindow(TEXT("ChildRight2"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc3 = CreateWindow(TEXT("ChildRight3"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		hc4 = CreateWindow(TEXT("ChildRight4"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 0, 0, 0, 0, hWnd, (HMENU)0, g_hInst, NULL);
		return 0;
	case WM_SIZE:
		GetClientRect(hWnd, &crt); //ȭ�鰪 �ҷ��ͼ� �б�
		MoveWindow(hc, 0, 0, crt.right - 400, crt.bottom - 50, TRUE);
		MoveWindow(hc0, 0, crt.bottom - 50, crt.right - 400, 50, TRUE);
		MoveWindow(hc1, crt.right - 400, 0, 400, 330, TRUE);
		MoveWindow(hc2, crt.right - 400, 330, 400, 185, TRUE);
		MoveWindow(hc3, crt.right - 400, 515, 400, crt.bottom - 615, TRUE);
		MoveWindow(hc4, crt.right - 400, crt.bottom - 100, 400, 100, TRUE);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

LRESULT CALLBACK ChildLeftProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) { //�׸��� ������ ���ν���

	HANDLE MyBitmap;
	int r_x = 650 - (width / 2); //�̹��� ����ִ� ���ʻ�� ��ǥ��
	int r_y = 500 - (height / 2);
	
	float f_width, f_height;
	float beyul = 0;

	switch (iMessage) {

	case WM_LBUTTONDOWN: //��ư �������� �����ǰ� �ް�
		bNow = TRUE;
		p_x1 = LOWORD(lParam);
		p_y1 = HIWORD(lParam);
		x1 = p_x1 - r_x;
		y1 = p_y1 - r_y;
		InvalidateRect(hc1, NULL, TRUE);
		return 0;

	case WM_MOUSEMOVE: { //�����µ��ȸ� detecting
		if (bNow == TRUE) {
			p_x2 = LOWORD(lParam);
			p_y2 = HIWORD(lParam);
			x2 = p_x2 - r_x;
			y2 = p_y2 - r_y;
			InvalidateRect(hc1, NULL, TRUE);
		}
		return 0;
	}
	case WM_LBUTTONUP: //��ư�� ���� x2,y2�� ����
		p_x2 = LOWORD(lParam);
		p_y2 = HIWORD(lParam);
		x2 = p_x2 - r_x;
		y2 = p_y2 - r_y;
		InvalidateRect(hc1, NULL, TRUE);
		bNow = FALSE;
		return 0;

	case WM_PAINT: //�̹��� �׸���
		//���۸�
		hdc = BeginPaint(hWnd, &ps);

		MemDC = CreateCompatibleDC(hdc);
		MyBitmap = LoadImage(NULL, TEXT("output.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SelectObject(MemDC, MyBitmap);

		now_w = width;
		now_h = height;

		f_width = width;
		f_height = height;
		beyul = f_width / f_height;
		r_x = 650 - (width / 2);
		r_y = 500 - (height / 2);
	
		//�̹��� ����� ���� ����
		GetClientRect(hWnd, &crt);
		if (beyul > 1) { //���ΰ� ���.
			if (crt.right < width) {
				now_w = crt.right - 25;
				now_h = (crt.right - 25) / beyul;
				r_x = 650 - (now_w / 2);
				r_y = 500 - (now_h / 2);
			}
		}
		else { //���ΰ� ���.
			if (crt.bottom - 50 < height) {
				now_w = (crt.bottom - 50) * beyul;
				now_h = crt.bottom - 50;
				r_x = 650 - (now_w / 2);
				r_y = 500 - (now_h / 2);
			}
		}
		SetStretchBltMode(hdc, COLORONCOLOR); //�÷����� ���� ->ȭ��
		StretchBlt(hdc, r_x, r_y, now_w, now_h, MemDC, 0, 0, width, height, SRCCOPY);

		DeleteObject(MyBitmap);
		DeleteDC(MemDC);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildLeft1Proc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	char str[120];

	switch (iMessage) {

	case WM_PAINT: //��� �ϴܿ� �̹��� ������ ���
		hdc = BeginPaint(hWnd, &ps);
		SetTextColor(hdc, RGB(255, 255, 255)); //���ڻ� : ���
		SetBkMode(hdc, TRANSPARENT);

		wsprintf(str, TEXT(" ������ : %d  X %d     �̹��� ũ�� : %d Byte"), width, height, byte_size / 1024);
		TextOut(hdc, 950, 20, (LPCSTR)str, strlen(str));

		wsprintf(str, TEXT("ũ�� : %d x %d "), abs(x2 - x1), abs(y2 - y1));
		TextOut(hdc, 30, 290, (LPCSTR)str, strlen(str));

		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

LRESULT CALLBACK ChildRight1Proc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) { //��ư 4��
	char str[128];
	int p_width = 0, p_height = 0;

	switch (iMessage) {

	case WM_CREATE:
		CreateWindow(TEXT("button"), TEXT("�¿����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 20, 175, 90, hWnd, (HMENU)MIRROR, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("���Ϲ���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_GROUP, 205, 20, 175, 90, hWnd, (HMENU)FLIP, g_hInst, NULL);

		CreateWindow(TEXT("button"), TEXT("ȸ��"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_GROUP, 20, 120, 360, 90, hWnd, (HMENU)ROTATE, g_hInst, NULL);
		hCrop = CreateWindow(TEXT("button"), TEXT("�ڸ���"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 205, 220, 175, 90, hWnd, (HMENU)CROP, g_hInst, NULL);
		return 0;
	case WM_PAINT: //���콺 ��ǥ�� ���
		hdc = BeginPaint(hWnd, &ps);

		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkMode(hdc, TRANSPARENT);

		wsprintf(str, TEXT("X1 : %d    Y1 : %d"), x1, y1);
		TextOut(hdc, 30, 230, (LPCSTR)str, strlen(str));
		wsprintf(str, TEXT("X2 : %d    Y2 : %d"), x2, y2);
		TextOut(hdc, 30, 250, (LPCSTR)str, strlen(str));

		wsprintf(str, TEXT("ũ�� : %d x %d "), abs(x2 - x1), abs(y2 - y1));
		TextOut(hdc, 30, 290, (LPCSTR)str, strlen(str));

		EndPaint(hWnd, &ps);

		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case MIRROR:
			mirror();
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			InvalidateRect(hc0, NULL, TRUE);
			break;
		case FLIP:
			flip();
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			InvalidateRect(hc0, NULL, TRUE);
			break;
		case ROTATE:
			rotate();
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			InvalidateRect(hc0, NULL, TRUE);
			break;
		case CROP:
			if (x1 == 0 && x2 == 0) {

			}
			crop(x1, x2, y1, y2);
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			InvalidateRect(hc0, NULL, TRUE);
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
LRESULT CALLBACK ChildRight2Proc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) { //��ũ�ѹ� 2�� 
	int TempPos = 0;

	switch (iMessage) {
	case WM_CREATE:
		CreateWindow(TEXT("button"), TEXT(" > ��� <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 20, 360, 20, hWnd, (HMENU)BRIGHTNESS, g_hInst, NULL);
		hScroll1 = CreateWindow(TEXT("scrollbar"), TEXT("���"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 20, 45, 360, 40, hWnd, (HMENU)BRIGHTNESS, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT(" > ��� <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 100, 360, 20, hWnd, (HMENU)CONTRAST, g_hInst, NULL);
		hScroll2 = CreateWindow(TEXT("scrollbar"), TEXT("���"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 20, 125, 360, 40, hWnd, (HMENU)CONTRAST, g_hInst, NULL);

		SetScrollRange(hScroll1, SB_CTL, 0, 20, TRUE);
		SetScrollPos(hScroll1, SB_CTL, sc1_pos, TRUE);
		SetScrollRange(hScroll2, SB_CTL, 0, 20, TRUE);
		SetScrollPos(hScroll2, SB_CTL, sc2_pos, TRUE);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case BRIGHTNESS:
			brightness(2);
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			break;
		case CONTRAST:
			contrast(2);
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
		prevset();
		if ((HWND)lParam == hScroll1) {
			sc1_pos = TempPos;
			brightness(sc1_pos - 10);
		}
		if ((HWND)lParam == hScroll2) {
			sc2_pos = TempPos;
			contrast(sc2_pos - 10);
		}
		SetScrollPos((HWND)lParam, SB_CTL, TempPos, TRUE);
		write_image();
		InvalidateRect(hc, NULL, TRUE);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
LRESULT CALLBACK ChildRight3Proc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) { //��ư 3��  ��ũ�ѹ� 3��

	int TempPos = 0;

	switch (iMessage) {
	case WM_CREATE:
		CreateWindow(TEXT("button"), TEXT(" > ���� <"), WS_CHILD | WS_VISIBLE | BS_FLAT, 5, 15, 390, 40, hWnd, (HMENU)0, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT(" > ��� ���� <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 70, 380, 30, hWnd, (HMENU)GRAYFILTER, g_hInst, NULL);
		gray = CreateWindow(TEXT("scrollbar"), TEXT("RR"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 105, 380, 40, hWnd, (HMENU)GRAYFILTER, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT(" > ���Ǿ� ���� <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 160, 380, 30, hWnd, (HMENU)SEPIAFILTER, g_hInst, NULL);
		sepia = CreateWindow(TEXT("scrollbar"), TEXT("GR"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 195, 380, 40, hWnd, (HMENU)SEPIAFILTER, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT(" > �� ���� <"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 250, 380, 30, hWnd, (HMENU)BLURFILTER, g_hInst, NULL);
		blur = CreateWindow(TEXT("scrollbar"), TEXT("BR"), WS_CHILD | WS_VISIBLE | SBS_HORZ, 10, 285, 380, 40, hWnd, (HMENU)BLURFILTER, g_hInst, NULL);

		SetScrollRange(gray, SB_CTL, 0, 10, TRUE);
		SetScrollPos(gray, SB_CTL, gray_pos, TRUE);
		SetScrollRange(sepia, SB_CTL, 0, 20, TRUE);
		SetScrollPos(sepia, SB_CTL, sepia_pos, TRUE);
		SetScrollRange(blur, SB_CTL, 0, 3, TRUE);
		SetScrollPos(blur, SB_CTL, blur_pos, TRUE);
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
		case SEPIAFILTER:
			sepia_filter(3);
			write_image();
			InvalidateRect(hc, NULL, FALSE);
			break;
		case BLURFILTER:
			blur_fillter(3);
			write_image();
			InvalidateRect(hc, NULL, FALSE);
			break;
		}
		return 0;

	case WM_HSCROLL:
		if ((HWND)lParam == gray) TempPos = gray_pos;
		if ((HWND)lParam == sepia) TempPos = sepia_pos;
		if ((HWND)lParam == blur) TempPos = blur_pos;
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
			prevset();
			gray_pos = TempPos;
			grayscale_fillter(gray_pos);
		}
		if ((HWND)lParam == sepia) {
			prevset();
			sepia_pos = TempPos;
			sepia_filter(sepia_pos);
		}
		if ((HWND)lParam == blur) {

			blur_pos = TempPos;
			blur_fillter(3);
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

		CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 5, 5, 190, 90, hWnd, (HMENU)RESET, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("����"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, 5, 195, 90, hWnd, (HMENU)EXIT, g_hInst, NULL);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case RESET:
			reset();
			write_image();
			InvalidateRect(hc, NULL, TRUE);
			InvalidateRect(hc0, NULL, TRUE);
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
