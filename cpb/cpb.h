#ifndef __CPB_H__
#define __CPB_H__

//
// A simple, header only progress bar for C
//
// ephjos - 11/08/2020
//
// Visit github.com/ephjos/cut/cpb to learn more
//

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#define PB_WIDTH 80
#define PB_STREAM stderr
#define C_CURR '#'
#define C_EMPTY '.'
#define HIDE_CURSOR() fputs("\e[?25l", PB_STREAM);
#define SHOW_CURSOR() fputs("\e[?25h", PB_STREAM);

void pb_sigint_handler(int s);
typedef struct pb_struct pb_t;
pb_t* new_pb(const long int tot, char* label);
void print_pb(pb_t* pb);
void update_pb(pb_t* pb, const long int ncurr);
void inc_pb(pb_t* pb);
void free_pb(pb_t* pb);

inline void pb_sigint_handler(int s)
{
	SHOW_CURSOR();
	fputs("\n", PB_STREAM);
	fflush(PB_STREAM);
	exit(1);
}

typedef struct pb_struct {
	long int tot;
	long int curr;
	char* label;
	int w;
	int mw;
	int done;
} pb_t;

inline pb_t* new_pb(const long int tot, char* label)
{
  signal(SIGINT, pb_sigint_handler);
	pb_t* pb = (pb_t*)malloc(sizeof(pb_t));

	int mw = PB_WIDTH;

#ifdef TIOCGWINSZ
	struct winsize win;
	ioctl(0, TIOCGWINSZ, &win);

	if (win.ws_col) {
		mw = win.ws_col;
	}
#endif

	pb->tot = tot;
	pb->curr = 0;
	pb->mw = mw;
	pb->w = mw - 1;
	pb->label = "";
	if (label) {
		int sl = strlen(label);
		if (sl < mw - 40) {
			pb->w = mw - sl - 2;
			pb->label = label;
		} else {
			fprintf(PB_STREAM, "%s\n", label);
		}
	}
	pb->done = 0;

	return pb;
}

inline void print_pb(pb_t* pb)
{
	HIDE_CURSOR()
	fflush(PB_STREAM);
	if (pb->done) return;

	double pc = (double)(pb->curr+1)/(double)pb->tot;
	int size = pb->w-14;
	int curr_size = (int)(pc * size+1);
	int i;
	char* out = (char*)calloc(1, size*sizeof(char)+2);
	//printf("\n%ld, %ld, %f %d %d\n", pb->curr, pb->tot, pc,size,curr_size);

	pc *= 100;

	for (i = 0; i < curr_size; i++) {
		out[i] = C_CURR;
	}

	if (pb->curr >= pb->tot-1) {
		for (; i<size; i++) {
			out[i] = C_CURR;
		}
		fprintf(PB_STREAM, "\r %s [%s] %.2f%% \n", pb->label, out, 100.0);
		pb->done = 1;
		free(out);
		return;
	}

	for (; i<size+1; i++) {
		out[i] = C_EMPTY;
	}

	fprintf(PB_STREAM, "\r %s [%s] %.2f%% ", pb->label, out, pc);
	free(out);
}

inline void update_pb(pb_t* pb, const long int ncurr)
{
	pb->curr = ncurr;
	print_pb(pb);
}

inline void inc_pb(pb_t* pb)
{
	pb->curr++;
	print_pb(pb);
}

inline void free_pb(pb_t* pb)
{
	SHOW_CURSOR();
	free(pb);
}

#endif
