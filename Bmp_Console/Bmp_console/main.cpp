#include<windows.h>
#include<stdio.h>
#include<conio.h>

#include "Console.h"
#include"resource.h"
#include "Image.h"


int main(void) {
	//jpgToBmp();

	read_image();
	cpy_input_To_temp();
	mirror();
	flip();	
	//crop(200, 300, 200, 300);
	write_image();
	
	Render(10, 10);
}
