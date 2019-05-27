#include<windows.h>
#include<conio.h>
#include"resource.h"

HWND hWnd = GetConsoleWindow();
HINSTANCE hInst = GetModuleHandle(NULL);

void Render(int x, int y)
{
	// DC�� �ڵ鰪�� ���� ������ �����Ѵ�.(hDC : ����ȭ��DC, hMemDC : �̹����� ���� DC)
	// Device Context�� �׷��ȿ� �ʿ��� ��� �ɼ��� �־�� ����ü��� �� �� �ִ�.
	// �׸��׸��� �׸��� �׸� ȭ���̶� ����ȴ�.
	HDC hDC, hMemDC;
	// �ĸ�����̴�. static ������ ���������μ�, �޸��� Data������ ����Ǵ� �Լ��� ������ ������� �ʰ� �޸𸮿� �����ִ� �����̴�.
	static HDC hBackDC;
	// �̹��� ��Ʈ���� �ڵ鰪�� ������ �����̴�.
	HBITMAP hBitmap, hOldBitmap, hBackBitmap;
	// �ڵ鰪���κ��� ���� ���� ��Ʈ�� ����ü.
	BITMAP Bitmap;
	// ���� �������� Rect��(ũ��)�� ���´�. Rect�� ������, ������ �Ʒ��� ���� ������ ���簢���� ��Ÿ���� ����ü�̴�.
	RECT WindowRect;
	GetWindowRect(hWnd, &WindowRect);

	// ���� �������� DC �ڵ鰪�� ���´�. GetWindowDC(hWnd)�� �����ϴ�.
	hDC = GetDC(hWnd);
	// hDC�� ȣȯ�Ǵ� DC�� �޸𸮿� ����� �ڵ鰪�� ��ȯ�Ѵ�.
	hBackDC = CreateCompatibleDC(hDC);
	hMemDC = CreateCompatibleDC(hDC);

	// ��Ʈ�� �޸𸮸� �Ҵ��ϰ� �ڵ��� ��ȯ�Ѵ�.
	hBackBitmap = CreateCompatibleBitmap(hDC, WindowRect.right, WindowRect.bottom);
	// �׸� ��ȭ���� �غ��Ѵ�.
	hOldBitmap = (HBITMAP)SelectObject(hBackDC, hBackBitmap);
	// ��Ʈ���� �ε��Ͽ� �ڵ��� ��ȯ�Ѵ�. resource.h�� ������ define�Ǿ��ִ� �ҷ��� ���ҽ��� �ε��Ѵ�.
	hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	// ũ�⸦ �ޱ����� ��Ʈ�� ����ü�� ��Ʈ�� �ڵ�κ��� �����Ѵ�.
	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	// �о�� ��Ʈ�� ���ҽ��� �޸� DC�� �����Ѵ�.
	SelectObject(hMemDC, hBitmap);

	// hMemDC�� �̹����� hBackDC�� ���ϴ� ��ġ�� ��Ӻ����Ų��.(����Ϸ��� �̹����� �ĸ���ۿ� �����Ų��.)
	BitBlt(hBackDC, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY);
	// hBackDC(�ĸ� ����)�� �ϼ��� �׸��� ȭ������ ��Ӻ����Ų��.
	BitBlt(hDC, x, y, x + Bitmap.bmWidth, y + Bitmap.bmHeight, hBackDC, 0, 0, SRCCOPY);

	// �޸𸮿� ������Ʈ�� �����Ѵ�.
	DeleteObject(SelectObject(hBackDC, hBackBitmap));
	DeleteObject(hBitmap);
	DeleteDC(hBackDC);
	DeleteDC(hMemDC);

	ReleaseDC(hWnd, hDC);
}
