#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


void load_image(){
	FILE* fi = fopen("input.bmp", "rb");
	FILE* fo = fopen("output.bmp", "wb");


	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fi);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fi);

	int width = infoHeader.biWidth;
	int height = infoHeader.biHeight;
	int padding = (4 - (width * 3 % 4)) % 4;
	int r_width = (width * 3) + padding;
	int size = r_width * height;

	//이미지 변수 및 동적할당
	unsigned char** input = (unsigned char**)malloc(sizeof(unsigned char*) * height);
	unsigned char** temp = (unsigned char**)malloc(sizeof(unsigned char*) * height);
	unsigned char** output = (unsigned char**)malloc(sizeof(unsigned char*) * height);
	for (int i = 0; i < height; i++) {
		input[i] = (unsigned char*)malloc(sizeof(unsigned char)* r_width);
		temp[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
		output[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
	}


	//fread(input, size, 1, fi);
	//이미지 불러오기
	unsigned char* temp_t;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < r_width; j++) {
			temp_t = &input[i][j];
			fread(temp_t, sizeof(unsigned char), 1, fi);
			//printf("%x", input[i][j]);
		}
	}
	fclose(fi);
	printf("%x", input);
	//이미지 편집
	temp = input;

	//이미지 쓰기
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fo);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fo);
	fwrite(input, size, 1, fo);
	fclose(fo);
}