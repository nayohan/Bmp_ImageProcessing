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
int n_width;
int size;
unsigned char** input;
unsigned char** temp;
unsigned char** output;

void read_image() {
	FILE* fi = fopen("input.bmp", "rb");

	//BMP헤더 불러오기
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fi);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fi);

	//헤더정보를 변수에 저장
	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	padding = (4 - (width * 3 % 4)) % 4;
	n_width = width * 3;
	r_width = n_width + padding;
	size = r_width * height;

	//BMP저장할 공간할당
	input = (unsigned char**)malloc(sizeof(unsigned char*) * height );
	for (int i = 0; i < height; i++) {
		input[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
	}
	temp = (unsigned char**)malloc(sizeof(unsigned char) * height * r_width);
	for (int i = 0; i < height; i++) {
		temp[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
	}

	//BMP read
	for (int i = 0; i < height; i++)
	{
		fread(input[i], r_width, 1, fi);
	}
	fclose(fi);
	printf("Read From input.bmp Successfully\n");
	//HEX출력
	/*
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < r_width; j++)	{
			printf("%x ", input[i][j]);
		}
		printf("\n");
	}
	printf("\n");*/
}
void write_image() {
	FILE* fo = fopen("output.bmp", "wb");

	//이미지 쓰기
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fo);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fo);
	for (int i = 0; i < height; i++)
	{
		fwrite(temp[i], r_width, 1, fo);
	}
	fclose(fo);
	for (int i = 0; i < height; i++) {
		free(temp[i]);
	}
	printf("Write To output.bmp Successfully\n");
}

void swap(int i, int j, int k, int l) {
	unsigned char tmp = temp[i][j];
	temp[i][j] = temp[k][l];
	temp[k][l] = tmp;
}
void cpy_input_To_temp() {
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < r_width; j++)
		{
			temp[i][j] = input[i][j];
			//printf("%x ", temp[i][j]);
		}
		//printf("\n");
	}
	//printf("\n");
	printf("Copy To Temp Succesful\n");
}

void mirror() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < n_width/2; j = j + 3) {
			swap(i, j, i, n_width - j - 3);
			swap(i, j + 1, i, n_width - j - 2);
			swap(i, j + 2, i, n_width - j - 1);
			//printf("%x %x %x ", temp[i][j], temp[i][j + 1], temp[i][j + 2]);
		}
		for (int k = 0; k < padding; k++) {
			temp[i][n_width + k] = 0;
		}
		//printf("\n");
	}
	//printf("\n");
}
void flip() {
	for (int i = 0; i < height/2; i++) {
		for (int j = 0; j < r_width; j++)	{
			swap(i, j, height - i - 1, j);
			//printf("%x ", temp[i][j]);
		}
		//printf("\n");
	}
	//printf("\n");
}
void crop(int x1, int x2, int y1, int y2) {
	int tmp_h = abs(y2 - y1);
	int tmp_w = abs(x2 - x1);
	int tmp_p = (4 - (tmp_w * 3 % 4)) % 4;
	int tmp_r_w = (tmp_w * 3) + tmp_p;
	height = tmp_h;
	r_width = tmp_r_w;
	size = tmp_r_w * tmp_h;
	infoHeader.biWidth = tmp_w;
	infoHeader.biHeight = tmp_h;
	fileHeader.bfSize = size;

	/* 상식적으로 해야될거 같은데 하면 free오류에다가 재할당 할필요없이 알아서 조정되는듯
	for (int i = 0; i < height; i++) {
		free(temp[i]);
	}
	free(temp);

	temp = (unsigned char**)malloc(sizeof(unsigned char) * height);
	for (int i = 0; i < height; i++) {
		temp[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
	}*/

	int k = 0, l = 0;
	if (y2 > y1) {
		if (x2 > x1) {
			for (int i = y1; i < y2; i++) {							//행
				for (int j = x1 * 3; j < x2 * 3; j++) {				//열
					temp[k][l] = temp[i][j];
					//printf("%x ", temp[k][l]);
					l++;
				}
				for (int j = 0; j < tmp_p; j++) {				//패딩
					temp[k][l] = 0;
					//printf("%x ", temp[k][l]);
					l++;
				}
				k++;
				l = 0;
				//printf("\n");
			}
		}
	}
	else {

	}
}
void rotate() {

	for (int i = 0; i < n_width; i++)
	{
		for (int j = 0; j < height; j++) {

		}
	}
	for (int i = 0; i < height; i++)
	{

		for (int j = 0; j < r_width; j++)
		{
			printf("%x ", input[i * r_width + j]);
		}
		printf("\n");
	}
	printf("---------------------------------------------\n");
}