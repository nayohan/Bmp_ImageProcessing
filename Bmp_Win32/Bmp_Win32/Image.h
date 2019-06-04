#ifndef __LOAD_IMAGE_H__
#define __LOAD_IMAGE_H__
struct filter_pos;

void read_image();
void write_image();
void free_mem();
void reset();

void mirror();
void flip();
void crop(int x1, int x2, int y1, int y2);
void rotate();
void brightness(int v);
void contrast(int v);

void grayscale_fillter(float gray_pos);
void blur_fillter(int blur_pos);
void sharpen_fillter(int sharpen_pos);

#endif