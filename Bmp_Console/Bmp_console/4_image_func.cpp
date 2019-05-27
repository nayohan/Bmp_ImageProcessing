#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//이미지 변수 및 동적할당
BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;
int width;
int height;
int padding;
int r_width;
int size;
unsigned char* input;
unsigned char* temp;
unsigned char* output;

void read_image() {
	FILE* fi = fopen("input.bmp", "rb");

	//BMP헤더 불러오기
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fi);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fi);

	//헤더정보를 변수에 저장
	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	padding = (4 - (width * 3 % 4)) % 4;
	r_width = (width * 3) + padding;
	size = r_width * height;

	//BMP저장할 공간할당
	input = (unsigned char*)malloc(sizeof(unsigned char) * size);
	temp = (unsigned char*)malloc(sizeof(unsigned char) * size);
	output = (unsigned char*)malloc(sizeof(unsigned char) * size);

	//BMP read
	fread(input, size, 1, fi);
	fclose(fi);

	//input대신 편집할 temp에 저장
	for (int i = 0; i < size; i++) {
		temp[i] = input[i];
	}
}

void write_image() {
	FILE* fo = fopen("output.bmp", "wb");

	//이미지 쓰기
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fo);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fo);
	fwrite(temp, size, 1, fo);
	fclose(fo);
}

void edit_image() {
	//이미지 편집

}

void mirror() {
	for (int y = 0; y < size; y++) {
		temp[y] = input[size - y];
	}
}
void flip() {

}
void crop() {

}
void rotate() {

}
void contrast() {

}