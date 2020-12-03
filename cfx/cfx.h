#ifndef __CFX_H__
#define __CFX_H__

//
// My fork of gfx (https://www3.nd.edu/~dthain/courses/cse20211/fall2013/gfx/)
// Make sure to link X with `-lX11` when compiling.
//   `gcc -lX11 main.c`
//
// ephjos - 12/02/2020
//
// Visit github.com/ephjos/cut/cfx to learn more
//

#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static Display *cfx_display=0;
static Window  cfx_window;
static GC      cfx_gc;
static Colormap cfx_colormap;
static int      cfx_fast_color_mode = 0;

typedef struct vec2i_t {
	int x, y;
} vec2i;

vec2i s_event = {
	.x = 0,
	.y = 0
};

void cfx_open(int width, int height, const char *title)
{
	cfx_display = XOpenDisplay(0);
	if(!cfx_display) {
		fprintf(stderr,"cfx_open: unable to open the graphics window.\n");
		exit(1);
	}

	Visual *visual = DefaultVisual(cfx_display,0);
	if(visual && visual->class==TrueColor) {
		cfx_fast_color_mode = 1;
	} else {
		cfx_fast_color_mode = 0;
	}

	int blackColor = BlackPixel(cfx_display, DefaultScreen(cfx_display));
	int whiteColor = WhitePixel(cfx_display, DefaultScreen(cfx_display));

	cfx_window = XCreateSimpleWindow(cfx_display, DefaultRootWindow(cfx_display), 0, 0, width, height, 0, blackColor, blackColor);

	XSetWindowAttributes attr;
	attr.backing_store = Always;

	XChangeWindowAttributes(cfx_display,cfx_window,CWBackingStore,&attr);

	XStoreName(cfx_display,cfx_window,title);

	XSelectInput(cfx_display, cfx_window, StructureNotifyMask|KeyPressMask|ButtonPressMask);

	XMapWindow(cfx_display,cfx_window);

	cfx_gc = XCreateGC(cfx_display, cfx_window, 0, 0);

	cfx_colormap = DefaultColormap(cfx_display,0);

	XSetForeground(cfx_display, cfx_gc, whiteColor);

	// Wait for the MapNotify event

	for(;;) {
		XEvent e;
		XNextEvent(cfx_display, &e);
		if (e.type == MapNotify)
			break;
	}
}

void cfx_flush()
{
	XFlush(cfx_display);
}

void cfx_point(int x, int y)
{
	XDrawPoint(cfx_display,cfx_window,cfx_gc,x,y);
}

void cfx_line(int x1, int y1, int x2, int y2)
{
	XDrawLine(cfx_display,cfx_window,cfx_gc,x1,y1,x2,y2);
}

void cfx_color(int r, int g, int b)
{
	XColor color;

	if(cfx_fast_color_mode) {
		/* If this is a truecolor display, we can just pick the color directly. */
		color.pixel = ((b&0xff)| ((g&0xff)<<8)| ((r&0xff)<<16));
	} else {
		/* Otherwise, we have to allocate it from the colormap of the display. */
		color.pixel = 0;
		color.red = r<<8;
		color.green = g<<8;
		color.blue = b<<8;
		XAllocColor(cfx_display,cfx_colormap,&color);
	}

	XSetForeground(cfx_display, cfx_gc, color.pixel);
}

void cfx_clear()
{
	XClearWindow(cfx_display,cfx_window);
}

void cfx_clear_color(int r, int g, int b)
{
	XColor color;
	color.pixel = 0;
	color.red = r<<8;
	color.green = g<<8;
	color.blue = b<<8;
	XAllocColor(cfx_display,cfx_colormap,&color);

	XSetWindowAttributes attr;
	attr.background_pixel = color.pixel;
	XChangeWindowAttributes(cfx_display,cfx_window,CWBackPixel,&attr);
}

int cfx_event_waiting()
{
	XEvent event;

	cfx_flush();

	while (1) {
		if(XCheckMaskEvent(cfx_display,-1,&event)) {
			if(event.type==KeyPress) {
				XPutBackEvent(cfx_display,&event);
				return 1;
			} else if (event.type==ButtonPress) {
				XPutBackEvent(cfx_display,&event);
				return 1;
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}
}

char cfx_wait()
{
	XEvent event;

	cfx_flush();

	while(1) {
		XNextEvent(cfx_display,&event);

		if(event.type==KeyPress) {
			s_event.x = event.xkey.x;
			s_event.y = event.xkey.y;

			return XLookupKeysym(&event.xkey,0);
		} else if(event.type==ButtonPress) {
			s_event.x = event.xkey.x;
			s_event.y = event.xkey.y;
			return event.xbutton.button;
		}
	}
}

vec2i cfx_event_pos()
{
	return s_event;
}

#endif
