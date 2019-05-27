#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define PIXEL_SIZE 3

//typedef struct tagBITMAPFILEHEADER { // 16
//	WORD bfType;		//	BMP ���� ���� �ѹ�. ��Ʈ�� ������ �´��� Ȯ���ϴµ� ����ϸ� ASCII �ڵ�� -> 0x42(B)0x4D(M)
//	DWORD bfSize;		// ���� ũ��(����Ʈ)
//	WORD  bfReserved1;	// ����� ������� ������ �̷��� ���� ����� ����
//	WORD bfReserved2;	// ����� ������� ������ �̷��� ���� ����� ����
//	DWORD bfOFFBits;	// ��Ʈ�ʵ������� ������ġ -> 54
//}BITMAPFILEHEADER;
//
//typedef struct tagBITMAPINFOHEADER {
//	DWORD biSize;			//���� ��Ʈ�� ���� ����� ũ�� ->40
//	LONG  biWidth;			//��Ʈ�� �̹����� ����ũ��(�ȼ�)
//	LONG  biHeight;			//��Ʈ�� �̹����� ����ũ��(�ȼ�)
//	WORD  biPlanes;			//����ϴ� �������� �� -> 1
//	WORD  biBitCount;		//�ȼ� �ϳ��� ǥ���ϴ� ��Ʈ�� -> ���� 24
//	DWORD biCompression;	//������ -> ������ ����X -> 0
//	DWORD biSizeImage;		//��Ʈ�� �̹����� �ȼ������� ũ�� -> 0
//	LONG  biXPelsPerMeter;	//�׸��� ���� �ػ�(���ʹ� �ȼ�)
//	LONG  biYPelsPerMeter;	//�׸��� ���� �ػ�(���ʹ� �ȼ�)
//	DWORD biClrUsed;		//���� ���̺��� ���� ���Ǵ� ���� ��
//	DWORD biClrImportant;	//��Ʈ���� ǥ���ϱ� ���� �ʿ��� ���� �ε��� ��
//}BITMAPINFOHEADER;
//
//typedef struct tagRGBTRIPLE {
//	BYTE rgbtBlue;			//B
//	BYTE rgbtGreen;			//G
//	BYTE rgbtRed;			//R
//} RGBTRIPLE;


int jpgToBmp(cv::Mat image) {
	if (image.empty()) {
		printf("error");
		return -1;
	}
	imwrite("input.bmp", image);
}


int main() {
	cv::Mat image = cv::imread("image.png");
	//jpgToBmp(image);

	FILE* fi = fopen("input.bmp", "rb");
	FILE* fo = fopen("output.bmp", "wb");
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fi);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fi);
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fo);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fo);

	int width = infoHeader.biWidth;
	int height = infoHeader.biHeight;
	int padding = (4 - (width * 3 % 4)) % 4;				//���ο��� ���� ����(���ӵ�����)
	int r_width = (width * 3) + padding;
	int size = r_width * height;

	unsigned char* input = (unsigned char*)malloc(size);
	fread(input, size, 1, fi);
	fclose(fi);

	unsigned char* temp = (unsigned char*)malloc(size);
	j = i * j;
	for (int i = 0; i < height; i++) {
		for (int j = i*j; j < r_width; j++) {
			temp[j] = input[j];
		}
	}

	

	unsigned char* output = (unsigned char*)malloc(size);
	fwrite(temp, size, 1, fo);

	fclose(fo);
}