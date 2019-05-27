#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define PIXEL_SIZE 3

//typedef struct tagBITMAPFILEHEADER { // 16
//	WORD bfType;		//	BMP 파일 매직 넘버. 비트맵 파일이 맞는지 확인하는데 사용하며 ASCII 코드로 -> 0x42(B)0x4D(M)
//	DWORD bfSize;		// 파일 크기(바이트)
//	WORD  bfReserved1;	// 현재는 사용하지 않으면 미래를 위해 예약된 공간
//	WORD bfReserved2;	// 현재는 사용하지 않으며 미래를 위해 예약된 공간
//	DWORD bfOFFBits;	// 비트맵데이터의 시작위치 -> 54
//}BITMAPFILEHEADER;
//
//typedef struct tagBITMAPINFOHEADER {
//	DWORD biSize;			//현재 비트맵 정보 헤더의 크기 ->40
//	LONG  biWidth;			//비트맵 이미지의 가로크기(픽셀)
//	LONG  biHeight;			//비트맵 이미지의 세로크기(픽셀)
//	WORD  biPlanes;			//사용하는 색상판의 수 -> 1
//	WORD  biBitCount;		//픽셀 하나를 표현하는 비트수 -> 거의 24
//	DWORD biCompression;	//압축방식 -> 보통은 압축X -> 0
//	DWORD biSizeImage;		//비트맵 이미지의 픽셀데이터 크기 -> 0
//	LONG  biXPelsPerMeter;	//그림의 가로 해상도(미터당 픽셀)
//	LONG  biYPelsPerMeter;	//그림의 세로 해상도(미터당 픽셀)
//	DWORD biClrUsed;		//색상 테이블에서 실제 사용되는 색상 수
//	DWORD biClrImportant;	//비트맵을 표현하기 위해 필요한 색상 인덱스 수
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
	int padding = (4 - (width * 3 % 4)) % 4;				//가로에서 남은 비율(계산속도때문)
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