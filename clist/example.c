#include <stdlib.h>
#include <stdio.h>

#include "clist.h"

int main(int argc, char *argv[])
{
	int_list* il = new_int_list(1);

	for (int i=0; i<100; i++) {
		append_int_list(il, i);
	}

	print_int_list(il);
	free_int_list(il);

	float_list* fl = new_float_list(1);

	for (float i=0; i<100; i++) {
		append_float_list(fl, i);
	}

	print_float_list(fl);
	free_float_list(fl);

	double_list* dl = new_double_list(1);

	for (double i=0; i<100; i++) {
		append_double_list(dl, i);
	}

	print_double_list(dl);
	free_double_list(dl);
	return 0;
}
