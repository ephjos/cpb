#ifndef __CLIST_H__
#define __CLIST_H__

//
// A basic general ArrayList/Vector/DynamicList for C
//
// ephjos - 12/01/2020
//
// Visit github.com/ephjos/cut/clist to learn more
//

#include <stdlib.h>
#include <stdio.h>

void clist_error(char* s);

inline void clist_error(char* s)
{
	fprintf(stderr, "Error: %s \n", s);
	exit(1);
}

// Call with a type, a function to print an instance
// of that type, and the zero value for that type and
// generate basic dynamic list that contains that type
#define DefineListFor(type, print_type, zero_val) \
typedef struct type##_list_t { \
	type* data; \
	int size; \
	int cap; \
} type##_list; \
type##_list* new_##type##_list(int cap) \
{ \
	type##_list* dlist = (type##_list*)calloc(1,sizeof(type##_list)); \
	if (dlist == NULL) { return NULL; } \
	dlist->data = (int*)calloc(1,sizeof(int)*cap); \
	if (dlist->data == NULL) { return NULL; } \
	dlist->size = 0; \
	dlist->cap = cap; \
	return dlist; \
} \
void append_##type##_list(type##_list* dlist, type x) \
{ \
	if (dlist->size >= dlist->cap) { \
		int oc = dlist->cap; \
		dlist->cap *= 2; \
		dlist->data = (type*)realloc( \
				dlist->data, sizeof(type)*dlist->cap); \
		if (dlist->data == NULL) { return; } \
		for (int j = oc; j < dlist->cap; j++) { \
			dlist->data[j] = zero_val; \
		} \
	} \
	dlist->data[dlist->size] = x; \
	dlist->size++; \
} \
void set_##type##_list(type##_list* dlist, int i, type x) \
{ \
	if (i < 0) { return; } \
	if (i >= dlist->cap) { \
		int oc = dlist->cap; \
		dlist->cap = i*2; \
		dlist->data = (type*)realloc( \
				dlist->data, sizeof(type)*dlist->cap); \
		if (dlist->data == NULL) { return; } \
		for (int j = oc; j < dlist->cap; j++) { \
			dlist->data[j] = zero_val; \
		} \
	} \
	int ig = i > dlist->size; \
	dlist->size = (ig*i) + (ig*dlist->size) + 1; \
	dlist->data[i] = x; \
} \
type get_##type##_list(type##_list* dlist, int i) \
{ \
	if (i >= 0 && i < dlist->size) { \
		return dlist->data[i]; \
	} \
 \
	char msg[256] = {0}; \
	sprintf( \
			msg, \
			"Access to index %d is out of bounds for list of size %d", \
			i, \
			dlist->size); \
	clist_error(msg); \
	return -1; \
} \
void free_##type##_list(type##_list* dlist) \
{ \
	free(dlist->data); \
	free(dlist); \
} \
void print_##type##_list(type##_list* dlist) \
{ \
	if (dlist->size == 0) { return; } \
 \
	int i; \
	printf("["); \
	for (i = 0; i<dlist->size-1; i++) { \
		print_type(dlist->data[i]); \
		printf(" "); \
	} \
	print_type(dlist->data[i]); \
	printf("]\n"); \
}

// Default Types

// int_list
void print_int(int x)
{
	printf("%d", x);
}
DefineListFor(int, print_int, 0);

// float_list
void print_float(float x)
{
	printf("%.4f", x);
}
DefineListFor(float, print_float, 0.0f);

// float_list
void print_double(double x)
{
	printf("%.4f", x);
}
DefineListFor(double, print_double, 0.0f);

#endif
