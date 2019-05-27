#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include"resource.h"

#include "Image.h"
#include "Console.h"


int main(void) {
	// jpgToBmp();

	read_image();
	flip();
	write_image();

	Render(10, 10);
	return 0;
}