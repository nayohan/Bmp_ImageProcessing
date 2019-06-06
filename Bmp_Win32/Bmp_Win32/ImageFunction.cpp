#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#define PI 3.141592

//이미지 변수 및 동적할당
BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;

int width;
int height;
int padding;
int r_width; //실제 RGB형태일때 길이
int n_width; //r_width에서 패딩 뺀 길이
int size;
int byte_size;

unsigned char** input;
unsigned char** temp;
double** f_temp;
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
	byte_size = fileHeader.bfSize;

	//BMP저장할 공간할당
	input = (unsigned char**)malloc(sizeof(unsigned char*) * height);
	temp = (unsigned char**)malloc(sizeof(unsigned char) * height * r_width);
	f_temp = (double**)malloc(sizeof(double) * height * r_width);
	output = (unsigned char**)malloc(sizeof(unsigned char) * height * r_width);

	for (int i = 0; i < height; i++) {
		input[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
		temp[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
		f_temp[i] = (double*)malloc(sizeof(double) * r_width);
		output[i] = (unsigned char*)malloc(sizeof(unsigned char) * r_width);
	}
	//BMP read
	for (int i = 0; i < height; i++)
	{
		fread(input[i], r_width, 1, fi);
	}
	fclose(fi);
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

void prevset() { //자르기 함수사이즈 기억해서 이전으로 돌리는 함수
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < r_width; j++)
		{
			output[i][j] = temp[i][j];
		}
	}
}
void reset() { //맨처음으로 돌리는 함수
	FILE* fi = fopen("input.bmp", "rb");
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fi);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fi);
	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	padding = (4 - (width * 3 % 4)) % 4;
	n_width = width * 3;
	r_width = n_width + padding;
	size = r_width * height;
	byte_size = fileHeader.bfSize;
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
			f_temp[i][j] = input[i][j];
			output[i][j] = input[i][j];
		}
	}
}
void mirror() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < n_width / 2; j = j + 3) {
			swap_output(i, j, i, n_width - j - 3);
			swap_output(i, j + 1, i, n_width - j - 2);
			swap_output(i, j + 2, i, n_width - j - 1);
		}
		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
	}
}
void flip() {
	for (int i = 0; i < height / 2; i++) {
		for (int j = 0; j < r_width; j++) {
			swap_output(i, j, height - i - 1, j);
		}
	}
}

void crop(int x1, int x2, int y1, int y2) { //파일정보변경후 그만큼 할당
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
	byte_size = fileHeader.bfSize;

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
					temp[k][l] = temp[i][j];
					output[k][l] = output[i][j];
					l++;
				}
				for (int j = 0; j < tmp_p; j++) {
					temp[k][l] = 0;
					output[k][l] = 0;
					l++;
				}
				k++;
				l = 0;
			}
		}
	}
}
void rotate() { //회전인데 temp를 넣어서 사용.
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
	for (int i = height - 1; i >= 0; i--) {
		for (int j = 0; j < n_width; j = j + 3) {
			output[i][j] = temp[k][l];
			output[i][j + 1] = temp[k][l + 1];
			output[i][j + 2] = temp[k][l + 2];
			k++;
		}

		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
		k = 0;
		l = l + 3;
	}
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
}

void brightness(int v) {

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < r_width; j++)
		{
			if (output[i][j] + v * 10 > 255) {
				output[i][j] = 255;
			}
			else if (output[i][j] + v * 10 < 0) {
				output[i][j] = 0;
			}
			else {
				output[i][j] = output[i][j] + v * 10;
			}
		}
	}
}

int contrast_factor(int _c, int _i, int _j) {
	int retVal = output[_i][_j] + ((output[_i][_j] - 128) * _c / 20);
	return retVal;
}
void contrast(int _c) {
	for (int i = 0; i < height; i++) {
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

float return_Yval(float x1, float y1, float x2, float y2, float x_val) { //두점의좌표로 X에 대한 Y값 계산하는 함수
	//ax+b;
	float a = (y2 - y1) / (x2 - x1);
	float b = y1 - a * x1;
	return a * x_val + b;
}
void grayscale_fillter(float gray_pos) {
	float pos = gray_pos / 10;
	float r = 0.2126;
	float g = 0.7152;
	float b = 0.0722;

	//반비례변수
	//그레이  //(0,C) (1,1) 함수 0에가까울수록 1을, 1에 가까울수록  C에 가까워진다.
	float g_r = return_Yval(0, 1, 1, r, pos);
	float g_g = return_Yval(0, 1, 1, g, pos);
	float g_b = return_Yval(0, 1, 1, b, pos);

	//컬러 상관 //(0,C), (1,0) 함수 0에가까울수록 0을, 1에 가까울수록  C에 가까워진다.
	float c_r = return_Yval(0, 0, 1, r, pos);
	float c_g = return_Yval(0, 0, 1, g, pos);
	float c_b = return_Yval(0, 0, 1, b, pos);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < n_width; j = j + 3) {
			int R = temp[i][j];
			int G = temp[i][j + 1];
			int B = temp[i][j + 2];
			output[i][j] = (g_r * R + c_g * G + c_b * B);
			output[i][j + 1] = (c_r * R + g_g * G + c_b * B);
			output[i][j + 2] = (c_r * R + c_g * G + g_b * B);
		}
		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
	}
}

void limit256(int i, int j, float input_val) {
	if (input_val > 255) {
		output[i][j] = 255;
	}
	else if (input_val < 0) {
		output[i][j] = 0;
	}
	else {
		output[i][j] = input_val;
	}
}
void sepia_filter(int sepia_pos) {
	float pos = sepia_pos / 10 + 1;

	float B_r = 0.2392;
	float B_g = 0.4696;
	float B_b = 0.0912;
	float G_r = 0.2990;
	float G_g = 0.5870;
	float G_b = 0.1140;
	float R_r = 0.3588;
	float R_g = 0.7044;
	float R_b = 0.1368;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < n_width; j = j + 3) {
			float R = output[i][j];
			float G = output[i][j + 1];
			float B = output[i][j + 2];

			//Y가 0에 가까울수록 1을, 1에 가까울수록 C에 가까워진다.
			//세피아  //(0,C) (1,1) 함수 0에가까울수록 1을, 1에 가까울수록  C에 가까워진다.
			//0이면 컬러가 되고, 1이면 세피아가 된다.
			float s_B_b = return_Yval(0, 1, 1, B_b, pos); //세피아 필터변수인데, output색상 B에다가 넣을 세피아 b필터변수.
			float s_G_g = return_Yval(0, 1, 1, G_g, pos);
			float s_R_r = return_Yval(0, 1, 1, R_r, pos);

			//컬러 상관 //(0,C), (1,0) 함수 0에가까울수록 0을, 1에 가까울수록  C에 가까워진다.
			//0이면 값이 안들어가고, 1이면 세피아가 된다.
			float c_B_r = return_Yval(0, 0, 1, B_r, pos);
			float c_B_g = return_Yval(0, 0, 1, B_g, pos);
			float c_G_r = return_Yval(0, 0, 1, G_r, pos);
			float c_G_b = return_Yval(0, 0, 1, G_b, pos);
			float c_R_g = return_Yval(0, 0, 1, R_g, pos);
			float c_R_b = return_Yval(0, 0, 1, R_b, pos);

			float input_B = R * c_B_r + G * c_B_g + B * s_B_b; //B
			float input_G = R * c_G_r + G * s_G_g + B * c_G_b; //G
			float input_R = R * s_R_r + G * c_R_g + B * c_R_b;   //R

			limit256(i, j, input_B);
			limit256(i, j + 1, input_G);
			limit256(i, j + 2, input_R);
		}
		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
	}
}

double out_mean_mask_sum(double** mask, int blur_sz, int i, int j) {//평균값을 이용한 블러
	double result = 0;
	int cen = blur_sz / 2;
	if (i < (blur_sz / 2)) {//첫행 ->이미지상 마지막 줄
		if (j < (blur_sz / 2)) {//첫열
			result += output[i][j] + output[i][j + 3];
			result += output[i + 1][j] + output[i + 1][j + 3];
			result /= 4;
			return result;
		}
		else if (j >= n_width - 2 - (blur_sz / 2)) {//마지막열
			result += output[i][j - 3] + output[i][j];
			result += output[i + 1][j - 3] + output[i + 1][j];
			result /= 4;
			return result;
		}
		else {//첫행 나머지
			result += output[i][j - 3] + output[i][j] + output[i][j + 3];
			result += output[i + 1][j - 3] + output[i + 1][j] + output[i + 1][j + 3];
			result /= 6;
			return result;
		}
	}
	if (i >= (height - (blur_sz / 2))) {//마지막행 -> 이미지상 첫줄
		if (j < (blur_sz / 2)) {//첫열
			result += output[i - 1][j] + output[i - 1][j + 3];
			result += output[i][j] + output[i][j + 3];
			result /= 4;
			return result;
		}
		else if (j >= n_width - 1 - (blur_sz / 2)) {//마지막열
			result += output[i - 1][j - 3] + output[i - 1][j];
			result += output[i][j - 3] + output[i][j];
			result /= 4;
			return result;
		}
		else {//마지막열 나머지
			result += output[i - 1][j - 3] + output[i - 1][j] + output[i - 1][j + 3];
			result += output[i][j - 3] + output[i][j] + output[i][j + 3];
			result /= 6;
			return result;
		}
	}
	result += output[i - 1][j - 3] + output[i - 1][j] + output[i - 1][j + 3];
	result += output[i][j - 3] + output[i][j] + output[i][j + 3];
	result += output[i + 1][j - 3] + output[i + 1][j] + output[i + 1][j + 3];
	result /= 9;
	return result;
}

double mean_mask_sum(double** mask, int blur_sz, int i, int j) {//평균값을 이용한 블러
	double result = 0;
	float f_blur_sz = blur_sz;
	float cen = f_blur_sz / 2;

	if (i < (blur_sz / 2)) {//첫행 ->이미지상 마지막 줄
		if (j < (blur_sz / 2) + 2) {//첫열
			result += temp[i][j] + temp[i][j + 3];
			result += temp[i + 1][j] + temp[i + 1][j + 3];
			result /= 4;
			return result;
		}
		else if (j >= n_width - 2 - (blur_sz / 2)) {//마지막열
			result += temp[i][j - 3] + temp[i][j];
			result += temp[i + 1][j - 3] + temp[i + 1][j];
			result /= 4;
			return result;
		}
		else {//첫행 나머지
			result += temp[i][j - 3] + temp[i][j] + temp[i][j + 3];
			result += temp[i + 1][j - 3] + temp[i + 1][j] + temp[i + 1][j + 3];
			result /= 6;
			return result;
		}
	}
	if (i > (height - 1 - (blur_sz / 2))) {//마지막행 -> 이미지상 첫줄
		if (j < (blur_sz / 2) + 2) {//첫열
			result += temp[i - 1][j] + temp[i - 1][j + 3];
			result += temp[i][j] + temp[i][j + 3];
			result /= 4;
			return result;
		}
		else if (j >= n_width - 1 - (blur_sz / 2)) {//마지막열
			result += temp[i - 1][j - 3] + temp[i - 1][j];
			result += temp[i][j - 3] + temp[i][j];
			result /= 4;
			return result;
		}
		else {//마지막열 나머지
			result += temp[i - 1][j - 3] + temp[i - 1][j] + temp[i - 1][j + 3];
			result += temp[i][j - 3] + temp[i][j] + temp[i][j + 3];
			result /= 6;
			return result;
		}
	}
	if (j < (blur_sz / 2) + 2) {//첫열
		result += temp[i - 1][j] + temp[i - 1][j + 3];
		result += temp[i][j] + temp[i][j + 3];
		result += temp[i + 1][j] + temp[i + 1][j + 3];
		result /= 6;
		return result;
	}
	if (j >= n_width - 2 - (blur_sz / 2)) {//마지막열
		result += temp[i - 1][j - 3] + temp[i - 1][j];
		result += temp[i][j - 3] + temp[i][j];
		result += temp[i + 1][j - 3] + temp[i + 1][j];
		result /= 6;
		return result;
	}
	result += temp[i - 1][j - 3] + temp[i - 1][j] + temp[i - 1][j + 3];
	result += temp[i][j - 3] + temp[i][j] + temp[i][j + 3];
	result += temp[i + 1][j - 3] + temp[i + 1][j] + temp[i + 1][j + 3];
	result /= 9;
	return result;
}


double Cumulative_distribution_function(double x) {//누적확률함수
	return  (1 / sqrt((2 * PI))) * exp(-(pow(x, 2) / 2));
}
double Integral(double start, double end, double(*func)(double a)) {//적분
	int NumIntervals = 50000;
	double ans = 0.0;
	double dx = ((end - start) / (NumIntervals + 1));  //4/101
	for (int i = 0; i <= NumIntervals; i++)
	{
		ans += func(start + i * dx);
	}
	ans *= dx;
	return ans;
}
double gaussian_mask_sum(double** mask, int blur_sz, int i, int j) { //가우시안분포를 이용한 블러
	double result = 0;
	float f_blur_sz = blur_sz;
	int cen = f_blur_sz / 2;
	if (i < (blur_sz / 2)) {//첫행
		if (j < (blur_sz / 2) + 2) {//첫열
			result += mask[cen][cen] * temp[i][j] + mask[cen][cen + 1] * temp[i][j + 3];
			result += mask[cen + 1][cen] * temp[i + 1][j] + mask[cen + 1][cen + 1] * temp[i + 1][j + 3];
			result /= 4;
			return result;
		}
		else if (j > n_width - (blur_sz / 2)) {//마지막열
			result += mask[cen][cen - 1] * temp[i][j - 3] + mask[cen][cen] * temp[i][j];
			result += mask[cen + 1][cen - 1] * temp[i + 1][j - 3] + mask[cen + 1][cen] * temp[i + 1][j];
			result /= 4;
			return result;
		}
		else {//첫행 나머지
			result += mask[cen][cen - 1] * temp[i][j - 3] + mask[cen][cen] * temp[i][j] + mask[cen][cen + 1] * temp[i][j + 3];
			result += mask[cen + 1][cen - 1] * temp[i + 1][j - 3] + mask[cen + 1][cen] * temp[i + 1][j] + mask[cen + 1][cen + 1] * temp[i + 1][j + 3];
			result /= 6;
			return result;
		}
	}
	if (i > (height - 1 - (blur_sz / 2))) {//마지막행
		if (j < (blur_sz / 2) + 2) {//첫열
			result += mask[cen - 1][cen] * temp[i - 1][j] + mask[cen - 1][cen + 1] * temp[i - 1][j + 3];
			result += mask[cen][cen] * temp[i][j] + mask[cen][cen + 1] * temp[i][j + 3];
			result /= 4;
			return result;
		}
		else if (j >= n_width - (blur_sz / 2) - 2) {//마지막열
			result += mask[cen - 1][cen - 1] * temp[i - 1][j - 3] + mask[cen - 1][cen] * temp[i - 1][j];
			result += mask[cen][cen - 1] * temp[i][j - 3] + mask[cen][cen] * temp[i][j];
			result /= 4;
			return result;
		}
		else {//마지막열 나머지
			result += mask[cen - 1][cen - 1] * temp[i - 1][j - 3] + mask[cen - 1][cen] * temp[i - 1][j] + mask[cen - 1][cen + 1] * temp[i - 1][j + 3];
			result += mask[cen][cen - 1] * temp[i][j - 3] + mask[cen][cen] * temp[i][j] + mask[cen][cen + 1] * temp[i][j + 3];
			result /= 6;
			return result;
		}
	}
	if (j < (blur_sz / 2) + 2) {//첫열
		result += mask[cen - 1][cen] * temp[i - 1][j] + mask[cen - 1][cen + 1] * temp[i - 1][j + 3];
		result += mask[cen][cen] * temp[i][j] + mask[cen][cen + 1] * temp[i][j + 3];
		result += mask[cen + 1][cen] * temp[i + 1][j] + mask[cen + 1][cen + 1] * temp[i + 1][j + 3];
		result /= 6;
		return result;
	}
	if (j >= n_width - (blur_sz / 2) - 2) {//마지막열
		result += mask[cen - 1][cen - 1] * temp[i - 1][j - 3] + mask[cen - 1][cen] * temp[i - 1][j];
		result += mask[cen][cen - 1] * temp[i][j - 3] + mask[cen][cen] * temp[i][j];
		result += mask[cen + 1][cen - 1] * temp[i + 1][j - 3] + mask[cen + 1][cen] * temp[i + 1][j];
		result /= 6;
		return result;
	}
	result += mask[cen - 1][cen - 1] * temp[i - 1][j - 3] + mask[cen - 1][cen] * temp[i - 1][j] + mask[cen - 1][cen + 1] * temp[i - 1][j + 3];
	result += mask[cen][cen - 1] * temp[i][j - 3] + mask[cen][cen] * temp[i][j] + mask[cen][cen + 1] * temp[i][j + 3];
	result += mask[cen + 1][cen - 1] * temp[i + 1][j - 3] + mask[cen + 1][cen] * temp[i + 1][j] + mask[cen + 1][cen + 1] * temp[i + 1][j + 3];
	result /= 9;
	return result;
}
void blur_fillter(int blur_sz) {
	//마스크 저장 공간
	double** gaussian_mask = (double**)malloc(sizeof(double*) * blur_sz);
	for (int i = 0; i < blur_sz; i++) {
		gaussian_mask[i] = (double*)malloc(sizeof(double) * blur_sz);
	}
	//weight kernel
	double f_blur_sz = blur_sz;
	double gap = 8 / f_blur_sz;
	double x1 = -4;
	double x2;
	double y1 = -4;
	double y2;

	int k = 0, l = 0;
	for (int i = 0; i < blur_sz; i++) {
		y2 = y1 + gap;
		for (int j = 0; j < blur_sz; j++) {

			double v2 = Integral(y1, y2, Cumulative_distribution_function);
			gaussian_mask[i][j] = v2;

			x2 = x1 + gap;
			double v1 = Integral(x1, x2, Cumulative_distribution_function);
			gaussian_mask[i][j] += v1;
			gaussian_mask[i][j] /= 2;
			x1 = x2;
		}
		y1 = y2;
		x1 = -4;
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < r_width; j++)
		{
			temp[i][j] = output[i][j];
		}
	}

	double weight_sum = 0;
	for (int i = 0; i < blur_sz; i++) {
		for (int j = 0; j < blur_sz; j++) {
			weight_sum += gaussian_mask[i][j];
		}
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < n_width; j++) {
			output[i][j] = mean_mask_sum(gaussian_mask, blur_sz, i, j); //옵션 선택가능
			//output[i][j] = gaussian_mask_sum(gaussian_mask, blur_sz, i, j)*weight_sum;
		}
		for (int k = 0; k < padding; k++) {
			output[i][n_width + k] = 0;
		}
	}
}