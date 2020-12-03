#include <stdio.h>
#include "cfx.h"

int main()
{
	int ysize = 300;
	int xsize = 300;

	char c;
	vec2i v;

	// Open a new window for drawing.
	cfx_open(xsize,ysize,"Example Graphics Program");

	// Set the current drawing color to green.
	cfx_color(0,200,100);

	// Draw a triangle on the screen.
	cfx_line(100,100,200,100);
	cfx_line(200,100,150,150);
	cfx_line(150,150,100,100);

	while(1) {
		// Wait for the user to press a character.
		c = cfx_wait();
		v = cfx_event_pos();
		printf("x=%d y=%d\n", v.x, v.y);

		// Quit if it is the letter q.
		if(c=='q') break;
	}

	return 0;
}
