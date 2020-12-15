//
//
//
#ifndef __CHT_H__
#define __CHT_H__

/*
 * Imports
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*
 * Macros
 */
#define ABS( x ) (((x) < 0) ? (-x) : (x))

#define NEW_CHT()  new_cht()

#define INS_CHT( d, key, x )   insert_cht(d, key, x)
#define INS_CHT_I( d, key, x ) insert_cht_i(d, key, x)

#define GET_CHT_ITEM( type, item ) \
	((type == DOUBLE) ? ((void*)&item->value.d) : \
	 (type == LONG)   ? ((void*)&item->value.l) : \
	 (type == ULONG)  ? ((void*)&item->value.ul) : \
											((void*)&item->value.b))

#define GET_CHT( d, key )    get_cht(d, key)
#define GET_CHT_I( d, key )  get_cht_i(d, key)

/*
 * Constants
 */
#define BUCKETS  8192
#define BUF_SIZE  128

/*
 * Types
 */

// A single table item, may be numbers or bool
typedef struct __cht_item cht_item;
typedef struct __cht_item {
	double value;
	char* key;
	cht_item* next;
} cht_item;

// C Hash Table struct
typedef struct __cht cht;
typedef struct __cht {
	long key_count;
	char** keys;
	cht_item** items;
} cht;

/*
 * Functions
 */
cht_item* new_cht_item();
cht* new_cht();
void free_cht(cht* d);
ulong hash_cht(char* key);
void insert_cht_core(cht* d, char* key, double value);
void insert_cht_i(cht* d, ulong h, double value);
double get_cht_core(cht* d, char* key);
double get_cht(cht* d, char* key);
double get_cht_i(cht* d, ulong h);
void print_cht(cht* d);

inline cht_item* new_cht_item()
{
	cht_item* i = (cht_item*)calloc(1,sizeof(cht_item));
	i->value = 0;
	i->key = NULL;
	i->next = NULL;
	return i;
}

inline cht* new_cht()
{
	cht* d = (cht*)malloc(sizeof(cht));
	d->key_count = 0;
	d->keys = (char**)malloc(sizeof(char*));
	d->items = (cht_item**)calloc(1,sizeof(cht_item*)*BUCKETS);
	return d;
}

inline void free_cht(cht* d)
{
	cht_item* curr, *tmp;
	for (int i = 0; i < BUCKETS; i++) {
		if ((curr = d->items[i]) != NULL) {
			while (curr) {
				tmp = curr;
				curr = curr->next;
				free(tmp);
			}
		}
	}
	for (int i = 0; i < d->key_count; i++) {
		free(d->keys[i]);
	}
	free(d->items);
	free(d->keys);
	free(d);
}

// sfold
// https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/HashFuncExamp.html#string-folding
inline ulong hash_cht(char* key)
{
	ulong sum = 0, mul = 1;
	bool m4;
	int l = strlen(key);
	for (int i = 0; i < l; i++) {
		m4 = i % 4 == 0;
		mul = m4 + (!m4*256);
		sum += key[i] * mul;
	}
	return ABS(sum) % BUCKETS;
}

inline void insert_cht_core(cht* d, char* key, double value)
{
	ulong h = hash_cht(key);

	// Add key
	d->key_count++;
	d->keys = (char**)realloc(d->keys, sizeof(char*)*d->key_count);
	d->keys[d->key_count-1] = key;

	// Make new item
	cht_item* n = new_cht_item();
	n->key = key; n->value = value;

	if (d->items[h]) { // collision, append to linked list
		cht_item* curr = d->items[h];
		while (curr->next != NULL) {
			curr = curr->next;
		}
		curr->next = n;
		return;
	}

	// No collision, create bucket
	d->items[h] = n;
	return;
}

void insert_cht(cht* d, char* key, double value);
inline void insert_cht(cht* d, char* key, double value)
{
	char* buf = (char*)malloc(sizeof(char)*BUF_SIZE);
	strcpy(buf, key);
	insert_cht_core(d, buf, value);
}

inline void insert_cht_i(cht* d, ulong h, double value)
{
	char* buf = (char*)malloc(sizeof(char)*BUF_SIZE);
	sprintf(buf, "%lu", h);
	insert_cht_core(d, buf, value);
}

inline double get_cht_core(cht* d, char* key) {
	ulong h = hash_cht(key);

	// Check if bucket exists
	if (!d->items[h]) return 0;
	cht_item* curr = d->items[h];
	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			return curr->value;
		}
		curr = curr->next;
	}

	// Couldn't find key
	return 0;
}

inline double get_cht(cht* d, char* key)
{
	return get_cht_core(d, key);
}

inline double get_cht_i(cht* d, ulong h)
{
	char* buf = (char*)malloc(sizeof(char)*BUF_SIZE);
	sprintf(buf, "%lu", h);
	double res =  get_cht_core(d, buf);
	free(buf);
	return res;
}

inline void print_cht(cht* d)
{
	if (d->key_count == 0) {
		printf("{}\n");
		return;
	}

	printf("{\n  ");
	for (int i = 0; i < d->key_count; i++) {
		printf("(%s %.2f) ", d->keys[i], get_cht(d, d->keys[i]));
		if ((i+1) % 8 == 0 && i != d->key_count-1) printf("\n  ");
	}
	printf("\n}\n");
}

#endif
