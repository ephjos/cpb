#ifndef __CGE_H__
#define __CGE_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Macros
 */

#define bool int
#define TRUE 1
#define FALSE 1

#define DEBUG 1
#define DDUMP( fmt, ... ) \
	do { \
		if (DEBUG) \
			fprintf(stderr, "%s:%d:%s(): " fmt "\n", __FILE__, \
					__LINE__, __func__, ##__VA_ARGS__); \
	} while (0)

#define DUMP( fmt, val ) \
	printf("%s = " fmt "\n", #val, val);

#define ERR( fmt, ... ) \
	do { \
		fprintf(stderr, fmt, ##__VA_ARGS__); \
		exit(1); \
	} while (0)

#ifndef MAX
#define MAX( a, b ) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN( a, b ) ((a) > (b) ? (b) : (a))
#endif

#define IS_BETWEEN( x, a, b ) \
	((unsigned char)((x) >= (a) && (x) <= (b)))

#define XIS_BETWEEN( x, a, b ) \
	((unsigned char)((x) > (a) && (x) < (b)))

/*
 * Functions
 */

// File contents

#define DEFAULT_BSIZE 8192
char** load_file(char* fn, int bsize, int* return_n);
inline
char**
load_file(char* fn, int bsize, int* return_n)
{
	// Treat 0 as default
	if (bsize == 0) bsize = DEFAULT_BSIZE;

	// Open file
	FILE* fp = fopen(fn, "r");
	if (fp == NULL) ERR("Could not open file %s\n", fn);

	// Init struct
	char ** data = (char**)malloc(sizeof(char*));
	int n = 0;

	// Loop inits
	char* buf = (char*)malloc(bsize);
	int w = 0;
	int l = 0;
	int i;
	for (i = 0; fgets(buf, bsize, fp); i++) {
		// Remove trailing newline
		l = strnlen(buf, bsize);
		buf[l-1] = '\0';

		// Resize, alloc, and copy
		data = (char**)realloc(data,sizeof(char*)*(i+1));
		data[i] = (char*)calloc(1,sizeof(char)*bsize);
		strncpy(data[i], buf, bsize);
		w = MAX(w, l); // Track max length line
	}
	free(buf);

	// If there was a line of max size, there might be issues
	if (w == bsize) {
		fclose(fp);
		ERR("Error while loading file, line width >= buf\
size %d; try increasing the buf size\n", bsize);
	}

	// Shrink all lines to match the max width
	n = i;
	for (i = 0; i < n; i++) {
		data[i] = (char*)realloc(data[i], sizeof(char)*w);
	}

	*return_n = n;
	fclose(fp);
	return data;
}

void free2d(void** f, int n);
inline void
free2d(void** f, int n)
{
	for (int i = 0; i < n; i++) {
		free(f[i]);
	}
	free(f);
}

char** split(char* s, char* delim, int* return_ntoks);
inline char**
split(char* s, char* delim, int* return_ntoks)
{
	char** toks = (char**)malloc(sizeof(char*));
	char* p = strtok(s, delim);
	int i = 0;

	while (p != NULL) {
		toks = (char**)realloc(toks, sizeof(char**)*(i+1));
		toks[i] = (char*)calloc(1, sizeof(char*)*strlen(p)+1);
		toks[i] = strncpy(toks[i], p, strlen(p));
		p = strtok(NULL, delim);
		i++;
	}

	*return_ntoks = i;
	free(p);
	return toks;
}

int* sstoi(char** ss, int n);
inline int*
sstoi(char** ss, int n)
{
	int* is = (int*)malloc(sizeof(int)*n);
	for (int i = 0; i < n; i++) {
		is[i] = atoi(ss[i]);
	}

	return is;
}

long* sstol(char** ss, int n);
inline long*
sstol(char** ss, int n)
{
	long* is = (long*)malloc(sizeof(long)*n);
	for (int i = 0; i < n; i++) {
		is[i] = atol(ss[i]);
	}

	return is;
}

long long* sstoll(char** ss, int n);
inline long long*
sstoll(char** ss, int n)
{
	long long* is = (long long*)malloc(sizeof(long long)*n);
	for (int i = 0; i < n; i++) {
		is[i] = atoll(ss[i]);
	}

	return is;
}

#endif
