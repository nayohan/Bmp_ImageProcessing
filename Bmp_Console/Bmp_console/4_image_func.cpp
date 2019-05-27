#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//�̹��� ���� �� �����Ҵ�
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

	//BMP��� �ҷ�����
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fi);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fi);

	//��������� ������ ����
	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	padding = (4 - (width * 3 % 4)) % 4;
	r_width = (width * 3) + padding;
	size = r_width * height;

	//BMP������ �����Ҵ�
	input = (unsigned char*)malloc(sizeof(unsigned char) * size);
	temp = (unsigned char*)malloc(sizeof(unsigned char) * size);
	output = (unsigned char*)malloc(sizeof(unsigned char) * size);

	//BMP read
	fread(input, size, 1, fi);
	fclose(fi);

	//input��� ������ temp�� ����
	for (int i = 0; i < size; i++) {
		temp[i] = input[i];
	}
}

void write_image() {
	FILE* fo = fopen("output.bmp", "wb");

	//�̹��� ����
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fo);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fo);
	fwrite(temp, size, 1, fo);
	fclose(fo);
}

void edit_image() {
	//�̹��� ����

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