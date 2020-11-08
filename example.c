#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cpb.h"

int main(int argc, char *argv[])
{
	long int n = 1000000;
	pb_t* pb = new_pb(n);

	long int i;
	printf("A really long loop\n");
	for (i = 0; i < n; i++) {
		update_pb(pb, i);
	}
	free_pb(pb);

	return 0;
}
