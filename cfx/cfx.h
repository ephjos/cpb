#ifndef __CFX_H__
#define __CFX_H__

//
// My fork of gfx (https://www3.nd.edu/~dthain/courses/cse20211/fall2013/gfx/)
//
// ephjos
//
// Visit github.com/ephjos/cut/cfx to learn more
//

#include <X11/Xlib.h>             // -lX11
#include <X11/Xutil.h>

#include <libavcodec/avcodec.h>   // -lavcodec
#include <libavutil/opt.h>        // -lavutil
#include <libavutil/imgutils.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>   //-lswscale

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PLANE_MASK (1<<30)-1
#define MIN( a, b ) ((a) > (b) ? (b) : (a))

static Display*               cfx_display = 0;
static Window                 cfx_window;
static GC                     cfx_gc;
static XFontStruct*           cfx_font;
static Colormap               cfx_colormap;
static int                    cfx_width = 0;
static int                    cfx_height = 0;
static int                    cfx_fast_color_mode = 0;
static int                    cfx_ex = 0;
static int                    cfx_ey = 0;
static double                 cfx_fps = 30;
static AVCodec*               cfx_av_codec = NULL;
static AVCodecContext*        cfx_av_codec_context = NULL;
static FILE*                  cfx_av_fp = NULL;
static AVFrame*               cfx_av_frame = NULL;
static AVPacket*              cfx_av_pkt = NULL;
static uint8_t                cfx_av_endcode[] = {0,0,1,0xb7};
static int                    cfx_av_ret = 0;
static int                    cfx_av_fcnt = 0;
static struct SwsContext*     cfx_sws_context = NULL;

static void
cfx_encode(
		AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile)
{
	int ret;

	ret = avcodec_send_frame(enc_ctx, frame);
	if (ret < 0) {
		fprintf(stderr, "Error sending a frame for encoding\n");
		exit(1);
	}

	while (ret >= 0) {
		ret = avcodec_receive_packet(enc_ctx, pkt);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;
		else if (ret < 0) {
			fprintf(stderr, "Error during encoding\n");
			exit(1);
		}

		fwrite(pkt->data, 1, pkt->size, outfile);
		av_packet_unref(pkt);
	}
}

void
cfx_open(int width, int height, int fps, char* filename)
{
	cfx_width = width; cfx_height = height; cfx_fps = fps;

	// Initialize display
	cfx_display = XOpenDisplay(NULL);
	if(!cfx_display) {
		fprintf(stderr,"cfx_open: unable to open the graphics window.\n");
		exit(1);
	}

	if (filename) { // Initialize for recording
		// Init codec
		cfx_av_codec = avcodec_find_encoder_by_name("mpeg4");
		//cfx_av_codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		assert(cfx_av_codec);

		// Init context
		cfx_av_codec_context = avcodec_alloc_context3(cfx_av_codec);
		assert(cfx_av_codec_context);

		// Init packet
		cfx_av_pkt = av_packet_alloc();
		assert(cfx_av_pkt);

		// Set context params
		cfx_av_codec_context->bit_rate = 20000000;
		cfx_av_codec_context->width = width;
		cfx_av_codec_context->height = height;
		cfx_av_codec_context->time_base = (AVRational){1, MIN(120, fps)};
		cfx_av_codec_context->framerate = (AVRational){MIN(120, fps), 1};
		cfx_av_codec_context->gop_size = 10;
		cfx_av_codec_context->max_b_frames = 1;
		cfx_av_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

		if (cfx_av_codec->id == AV_CODEC_ID_H264) {
			av_opt_set(cfx_av_codec_context->priv_data, "preset", "slow", 0);
		}

		cfx_av_ret = avcodec_open2(cfx_av_codec_context, cfx_av_codec, NULL);
		assert(cfx_av_ret == 0);

		cfx_av_fp = fopen(filename, "wb");
		assert(cfx_av_fp);

		cfx_av_frame = av_frame_alloc();
		assert(cfx_av_frame);
		cfx_av_frame->format = cfx_av_codec_context->pix_fmt;
		cfx_av_frame->width = cfx_av_codec_context->width;
		cfx_av_frame->height = cfx_av_codec_context->height;

		cfx_av_ret = av_frame_get_buffer(cfx_av_frame, 0);
		assert(cfx_av_ret == 0);

		cfx_sws_context = sws_getContext(
				width, height, AV_PIX_FMT_RGB32,
				width, height, AV_PIX_FMT_YUV420P,
				SWS_BICUBIC, 0, 0, 0);
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
	XStoreName(cfx_display,cfx_window,"cfx");

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

// Draws a circle centered at x,y with radius r
void
cfx_draw_circle(int x, int y, int r)
{
	int n = 12;
	for (int i = 0; i < 2*n; i++) {
		cfx_draw_line(
				x-(r*cos(i*(M_PI/n))), y-(r*sin(i*(M_PI/n))),
				x-(r*cos((i+1%n)*(M_PI/n))), y-(r*sin((i+1%n)*(M_PI/n))));
	}
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
			cfx_ex = event.xkey.x;
			cfx_ey = event.xkey.y;
			*x = cfx_ex;
			*y = cfx_ey;
			return XLookupKeysym(&event.xkey,0);
		} else if(event.type==ButtonPress) {
			cfx_ex = event.xkey.x;
			cfx_ey = event.xkey.y;
			*x = cfx_ex;
			*y = cfx_ey;
			return event.xbutton.button;
		}
	}
}

// Get the (x,y) position of the last event
void
cfx_get_event_pos(int* x, int* y)
{
	*x = cfx_ex;
	*y = cfx_ey;
}

// Call in loop after handling input to run at fps
void
cfx_wait_frame() {
	usleep(1000000.0 / cfx_fps);

	if (!cfx_av_fp) return;

	fflush(stdout);
	cfx_av_ret = av_frame_make_writable(cfx_av_frame);
	assert(cfx_av_ret == 0);

	XImage* img = XGetImage(cfx_display, cfx_window,
			0,0,cfx_width,cfx_height,PLANE_MASK,ZPixmap);

	const uint8_t* data[1] = {(uint8_t*)(img->data)};
	int data_linesize[1] = { 4*cfx_width };
	sws_scale(cfx_sws_context, data, data_linesize, 0, cfx_height,
			cfx_av_frame->data, cfx_av_frame->linesize);

	cfx_av_frame->pts = cfx_av_fcnt; cfx_av_fcnt++;
	cfx_encode(cfx_av_codec_context,cfx_av_frame,cfx_av_pkt,cfx_av_fp);
	XDestroyImage(img);
}

void
cfx_free()
{
	if (cfx_av_fp) {
		cfx_encode(cfx_av_codec_context,NULL,cfx_av_pkt,cfx_av_fp);

		if (cfx_av_codec->id == AV_CODEC_ID_MPEG1VIDEO ||
				cfx_av_codec->id == AV_CODEC_ID_MPEG2VIDEO) {
			fwrite(cfx_av_endcode, 1, sizeof(cfx_av_endcode), cfx_av_fp);
		}
		fclose(cfx_av_fp);

		sws_freeContext(cfx_sws_context);

		avcodec_free_context(&cfx_av_codec_context);
		av_frame_free(&cfx_av_frame);
		av_packet_free(&cfx_av_pkt);
	}

	XFreeFont(cfx_display, cfx_font);
	XFreeColormap(cfx_display, cfx_colormap);
	XFreeGC(cfx_display, cfx_gc);

	XDestroyWindow(cfx_display, cfx_window);
	XSync(cfx_display, 0);
	XCloseDisplay(cfx_display);
}

#endif
