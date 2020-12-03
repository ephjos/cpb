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
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Display*        cfx_display = 0;
static Window          cfx_window;
static GC              cfx_gc;
static XFontStruct*    cfx_font;
static Colormap        cfx_colormap;
static int             cfx_fast_color_mode = 0;
static int             event_x = 0;
static int             event_y = 0;


void
cfx_open(int width, int height, const char* title)
{
	// Initialize display
	cfx_display = XOpenDisplay(0);
	if(!cfx_display) {
		// TODO: ERROR FUNC
		fprintf(stderr,"cfx_open: unable to open the graphics window.\n");
		exit(1);
	}

	// Determine color mode
	Visual* visual = DefaultVisual(cfx_display,0);
	if(visual && visual->class==TrueColor) {
		cfx_fast_color_mode = 1;
	} else {
		cfx_fast_color_mode = 0;
	}

	// Initialize base colors
	int blackColor = BlackPixel(cfx_display, DefaultScreen(cfx_display));
	int whiteColor = WhitePixel(cfx_display, DefaultScreen(cfx_display));

	// Initialize window
	cfx_window = XCreateSimpleWindow(
			cfx_display, DefaultRootWindow(cfx_display),
			0, 0, width, height, 0, blackColor, blackColor);

	// Set window attributes
	XSetWindowAttributes attr;
	attr.backing_store = Always;
	XChangeWindowAttributes(
			cfx_display,cfx_window,
			CWBackingStore,&attr);

	// Set name of window
	XStoreName(cfx_display,cfx_window,title);

	// Specify what events to listen to
	XSelectInput(
			cfx_display, cfx_window,
			StructureNotifyMask|KeyPressMask|ButtonPressMask);

	// Send window to be attached to screen
	XMapWindow(cfx_display,cfx_window);

	// Initialize graphics context
	cfx_gc = XCreateGC(cfx_display, cfx_window, 0, 0);
	cfx_colormap = DefaultColormap(cfx_display,0);
	XSetForeground(cfx_display, cfx_gc, whiteColor);

	// Load any mono font
	cfx_font = XLoadQueryFont(cfx_display, "*mono*");
	// TODO: ERROR FUNC
	assert(cfx_font != NULL);

	// Wait for the MapNotify event, fired when window is attached
	for(;;) {
		XEvent e;
		XNextEvent(cfx_display, &e);
		if (e.type == MapNotify)
			break;
	}
}

// Push all changes to display
void
cfx_flush()
{
	XFlush(cfx_display);
}

// Draw a point
void
cfx_draw_point(int x, int y)
{
	XDrawPoint(
			cfx_display, cfx_window, cfx_gc,
			x, y);
}

// Draw a line
void
cfx_draw_line(int x1, int y1, int x2, int y2)
{
	XDrawLine(
			cfx_display,cfx_window,cfx_gc,
			x1, y1, x2, y2);
}

// Draw a rectangle
void
cfx_draw_rectangle(int x, int y, int w, int h, int fill)
{
	if (fill) {
		XFillRectangle(
				cfx_display, cfx_window, cfx_gc,
				x, y, w, h);
	} else {
		XDrawRectangle(
				cfx_display, cfx_window, cfx_gc,
				x, y, w-1, h-1);
	}
}

// TODO
void
cfx_draw_circle()
{
}

// Draw some text
// returns the width of the string in pixels
int
cfx_draw_text(int x, int y, char* s)
{
	int width = XTextWidth(cfx_font, s, strlen(s));
	XDrawString(
			cfx_display, cfx_window, cfx_gc,
			x, y, s, strlen(s));
	return width;
}

// Set the color
void
cfx_color(int r, int g, int b)
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

// Clear the display
void
cfx_clear()
{
	XClearWindow(cfx_display,cfx_window);
}

// Change what the clear color will be (default background)
void
cfx_clear_color(int r, int g, int b)
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

// Returns true if there is an input event available
int
cfx_event_waiting()
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

// Waits for input, returning the character of the
// key that was pressed, or '' if it was a click
//
char
cfx_wait(int* x, int* y)
{
	XEvent event;

	cfx_flush();

	while(1) {
		XNextEvent(cfx_display,&event);

		if(event.type==KeyPress) {
			event_x = event.xkey.x;
			event_y = event.xkey.y;
			*x = event_x;
			*y = event_y;
			return XLookupKeysym(&event.xkey,0);
		} else if(event.type==ButtonPress) {
			event_x = event.xkey.x;
			event_y = event.xkey.y;
			*x = event_x;
			*y = event_y;
			return event.xbutton.button;
		}
	}
}

// Get the (x,y) position of the last event
void
cfx_get_event_pos(int* x, int* y)
{
	*x = event_x;
	*y = event_y;
}

// TODO: free all data, cleanup
void
cfx_free()
{
}

#endif
