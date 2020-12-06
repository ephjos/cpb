#include <stdio.h>
#include "cfx.h"

int main()
{
	int ysize = 600;
	int xsize = 600;

	char c;
	int x,y;

	// Open a new window for drawing.
	cfx_open(xsize,ysize);


	int i = 0;
	while(1) {
		cfx_clear();

		// Draw a triangle on the screen.
		cfx_color(0,200,100);
		cfx_draw_line(100,100,200,100);
		cfx_draw_line(200,100,150,150);
		cfx_draw_line(150,150,100,100);

		// Draw rectangles
		cfx_color(200,100,50);
		cfx_draw_rectangle(i,20,50,25,0);
		cfx_draw_rectangle(i,50,50,25,1);

		// Draw text
		cfx_color(0,255,0);
		cfx_draw_text(300, 300, "Hello, cfx!");

		// Update screen
		cfx_flush();

		// Increment i (cyclic)
		i+=10; i %= xsize-50;

		if (cfx_event_waiting()) {
			// Wait for the user to press a character.
			c = cfx_wait(&x,&y);
			printf("c=%c x=%d y=%d\n", c, x, y);

			// Quit if it is the letter q.
			if(c=='q') break;
		}

		// Sleep for enough time to update window at 60 fps
		cfx_anim(60);
	}

	// Cleanup
	cfx_free();

	return 0;
}
