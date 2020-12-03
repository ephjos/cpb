#include <stdio.h>
#include "cfx.h"

int main()
{
	int ysize = 600;
	int xsize = 600;

	char c;
	int x,y;

	// Open a new window for drawing.
	cfx_open(xsize,ysize,"Example Graphics Program");

	// Set the current drawing color to green.
	cfx_color(0,200,100);

	// Draw a triangle on the screen.
	cfx_draw_line(100,100,200,100);
	cfx_draw_line(200,100,150,150);
	cfx_draw_line(150,150,100,100);

	cfx_draw_rectangle(0,50,50,25,0);
	cfx_draw_rectangle(0,75,50,25,1);

	cfx_draw_text(300, 300, "Hello, cfx!");

	while(1) {
		// Wait for the user to press a character.
		c = cfx_wait(&x,&y);
		printf("c=%c x=%d y=%d\n", c, x, y);

		// Quit if it is the letter q.
		if(c=='q') break;
	}

	return 0;
}
