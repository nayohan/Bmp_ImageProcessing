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
int n_width;
int size;
unsigned char** input;
unsigned char** temp;
unsigned char** output;

void read_image() {
	FILE* fi = fopen("input.bmp", "rb");

	//BMP��� �ҷ�����
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fi);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fi);

	//��������� ������ ����
	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	padding = (4 - (width * 3 % 4)) % 4;
	n_width = width * 3;
	r_width = n_width + padding;
	size = r_width * height;

	//BMP������ �����Ҵ�
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
	//HEX���
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

	//�̹��� ����
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

	/* ��������� �ؾߵɰ� ������ �ϸ� free�������ٰ� ���Ҵ� ���ʿ���� �˾Ƽ� �����Ǵµ�
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
			for (int i = y1; i < y2; i++) {							//��
				for (int j = x1 * 3; j < x2 * 3; j++) {				//��
					temp[k][l] = temp[i][j];
					//printf("%x ", temp[k][l]);
					l++;
				}
				for (int j = 0; j < tmp_p; j++) {				//�е�
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