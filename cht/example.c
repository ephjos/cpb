#include <stdlib.h>
#include <stdio.h>

#include "cht.h"

int main(int argc, char *argv[])
{
	char t[128] = "This is a test";
	printf("%s => %lu\n", t, hash_cht(t));

	cht* d = NEW_CHT();

	INS_CHT(d, "key1", 15.5);
	printf("%f\n", GET_CHT(d, "key1"));

	INS_CHT_I(d, 25, 123);

	for (int i = 0; i < 256; i++) {
		INS_CHT_I(d, i, (i<<3)%37);
	}

	print_cht(d);

	printf("%f\n", GET_CHT(d, "242"));
	printf("%f\n", GET_CHT_I(d, 242));

	free_cht(d);
	return 0;
}
