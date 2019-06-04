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
	temp = (unsigned char**)malloc(sizeof(unsigned char) * height * r_width);
	output = (unsigned char**)malloc(sizeof(unsigned char) * height * r_width);

	for (int i = 0; i < height; i++) {
		input[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
		temp[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
		output[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
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
		fwrite(output[i], r_width, 1, fo);
	}
	fclose(fo);
}
void free_mem() {
	for (int i = 0; i < height; i++) {
		free(input[i]);
		free(temp[i]);
		free(output[i]);
	}
	free(input);
	free(temp);
	free(output);
}
void swap_int(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*a = temp;
}
void swap_output(int i, int j, int k, int l) {
	unsigned char tmp = output[i][j];
	output[i][j] = output[k][l];
	output[k][l] = tmp;
}
void reset() {
	FILE* fi = fopen("input.bmp", "rb");
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fi);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fi);
	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	padding = (4 - (width * 3 % 4)) % 4;
	n_width = width * 3;
	r_width = n_width + padding;
	size = r_width * height;
	fclose(fi);

	for (int i = 0; i < height; i++) {
		temp[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
		output[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < r_width; j++)
		{
			temp[i][j] = input[i][j];
			output[i][j] = input[i][j];
			//printf("%x ", output[i][j]);
		}
		//printf("\n");
	}
	//printf("\n");
	printf("Copy To Temp Succesful\n");
}
void mirror() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < n_width/2; j = j + 3) {
			swap_output(i, j, i, n_width - j - 3);
			swap_output(i, j + 1, i, n_width - j - 2);
			swap_output(i, j + 2, i, n_width - j - 1);
			//printf("%x %x %x ", output[i][j], output[i][j + 1], output[i][j + 2]);
		}
		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
		//printf("\n");
	}
	//printf("\n");
}
void flip() {
	for (int i = 0; i < height/2; i++) {
		for (int j = 0; j < r_width; j++)	{
			swap_output(i, j, height - i - 1, j);
			//printf("%x ", output[i][j]);
		}
		//printf("\n");
	}
	//printf("\n");
}
void crop(int x1, int x2, int y1, int y2) {
	if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
		return;
	}
	if (x1 > width || x2 > width || y1 > height || y2 > height) {
		return;
	}
	int tmp_h = abs(y2 - y1);
	int tmp_w = abs(x2 - x1);
	int tmp_p = (4 - (tmp_w * 3 % 4)) % 4;
	int tmp_r_w = (tmp_w * 3) + tmp_p;
	height = tmp_h;
	width = tmp_w;
	padding = tmp_p;
	r_width = tmp_r_w;
	size = tmp_r_w * tmp_h;
	infoHeader.biWidth = tmp_w;
	infoHeader.biHeight = tmp_h;
	fileHeader.bfSize = size;

	int k = 0, l = 0;
	int swap_temp;
	if (y2 < y1) {
		swap_temp = y2;
		y2 = y1;
		y1 = swap_temp;
	}
	if (x2 < x1) {
		swap_temp = x2;
		x2 = x1;
		x1 = swap_temp;
	}
	if (y1 < y2) {
		if (x1 < x2) {
			for (int i = y1; i < y2; i++) {
				for (int j = x1 * 3; j < x2 * 3; j++) {
					output[k][l] = output[i][j];
					l++;
				}
				for (int j = 0; j < tmp_p; j++) {	
					output[k][l] = 0;
					l++;
				}
				k++;
				l = 0;
			}
		}
	}
}
void rotate() {
	for (int i = 0; i < height; i++) {
		temp[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < r_width; j++)
		{
			temp[i][j] = output[i][j];
		}
	}
	int tmp = height;
	height = width;
	width = tmp;
	padding = (4 - (width * 3 % 4)) % 4;
	n_width = width * 3;
	r_width = n_width + padding;

	infoHeader.biWidth = width;
	infoHeader.biHeight = height;
	for (int i = 0; i < height; i++) {
		output[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
	}

	int k = 0, l = 0;
	for (int i = height-1; i >= 0; i--) {
		for (int j = 0; j  < n_width; j = j + 3){
			output[i][j] = temp[k][l];
			output[i][j+1] = temp[k][l+1];
			output[i][j+2] = temp[k][l+2];
			k++;
		}
		
		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
		k = 0;
		l = l + 3;
	}
}
void brightness(int v) {
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < r_width; j++)
		{
			if (output[i][j] + v > 255) {
				output[i][j] = 255;
			}
			else if (output[i][j] + v < 0) {
				output[i][j] = 0;
			}
			else {
				output[i][j] = output[i][j] + v;
			}
		}
	}
}
int contrast_factor(int _c, int _i, int _j) {
	int retVal;
	float F = (259 * (_c + 255)) / (255 * (259 - _c));
	retVal = F * (output[_i][_j] - 128) + 128;
	return retVal;
}
void contrast(int _c) {
	for (int i = 0; i < height; i++) 	{
		for (int j = 0; j < n_width; j++)
		{
			if (contrast_factor(_c, i, j) > 255) {
				output[i][j] = 255;
			}
			else if (contrast_factor(_c, i, j) < 0) {
				output[i][j] = 0;
			}
			else {
				output[i][j] = contrast_factor(_c, i, j);
			}
		}
	}
}


float returnYval(float x1, float y1, float x2, float y2, float x_val) { //두점의좌표로 X에 대한 Y값 계산하는 함수
	//ax+b;
	float a = (y2 - y1) / (x2 - x1);
	float b =  y1 - a * x1;
	return a * x_val + b;
}
void grayscale_fillter(float gray_pos) {
	float pos = gray_pos / 10;
	float r = 0.2126;
	float g = 0.7152;
	float b = 0.0722;

	//그레이  //(0,C) (1,1) 함수 0에가까울수록 1을, 1에 가까울수록  C에 가까워진다.
	float g_r = returnYval(0, 1, 1, r, pos);
	float g_g = returnYval(0, 1, 1, g, pos);
	float g_b = returnYval(0, 1, 1, b, pos);

	//컬러 상관 //(0,C), (1,0) 함수 0에가까울수록 0을, 1에 가까울수록  C에 가까워진다.
	float c_r = returnYval(0, 0, 1, r, pos);
	float c_g = returnYval(0, 0, 1, g, pos);
	float c_b = returnYval(0, 0, 1, b, pos);
	
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < n_width; j = j + 3) {
			int R = input[i][j];
			int G = input[i][j+1];
			int B = input[i][j+2];
			output[i][j] = (g_r * R + c_g * G + c_b*  B);
			output[i][j + 1] = (c_r * R + g_g * G + c_b * B);
			output[i][j + 2] = (c_r * R + c_g * G + g_b * B);
		}
		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
	}
}
void sepiaFilter() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < n_width; j = j + 3) {
			int r = output[i][j];
			int g = output[i][j + 1];
			int b = output[i][j + 2];
			output[i][j] = r * 0.3588 + g * 0.7044 + b * 0.1368;
			output[i][j + 1] = r * 0.2990 + g * 0.5870 + b * 0.1140;
			output[i][j + 2] = r * 0.2392 + g * 0.4696 + b * 0.0912;
		}
		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
	}
}

int gaussian_distribution(int _blur_sz, int i, int j) {
	int blur_val = 0;


	return blur_val;
}
void blur_fillter(int blur_sz) {
	int** gaussian = (int**)malloc(sizeof(int*) * blur_sz);
	for (int i = 0; i < blur_sz; i++) 	{
		gaussian[i] = (int*)malloc(sizeof(int) * blur_sz);
	}
	for (int i = 0; i < blur_sz; i++) 	{
		for (int j = 0; j < blur_sz; j++) 	{
			//gaussian[i][j]=
		}
	}
	for (int i = 0; i < height; i= i + 3) {
		for (int j = 0; j < n_width; j = j + 3) {
			for (int l = 0; l < 3; l++) {
				for (int k = 0; k < 9; k++)
				{
					//output[i+l][j+k]= 
				}
			}
		}
		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
	}
}
void sharpen_fillter(int shapen_pos) {

}