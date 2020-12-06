#include <stdlib.h>
#include <stdio.h>

#include "cge.h"

int main(int argc, char *argv[])
{
	DDUMP("Debugging dump!");
	DDUMP("Formatted dump = %d", 123);

	DUMP("%d",1+2+3+4);
	int x = 100;
	DUMP("%d", x);
	DUMP("%d", x/2);

	printf("%d %d\n", MAX(20,10), MAX(20,10));
	printf("%d %d\n", MIN(20,10), MIN(20,10));
	printf("%d %d\n", IS_BETWEEN(5,0,10), IS_BETWEEN(10,0,5));
	printf("%d %d\n", XIS_BETWEEN(5,0,10), XIS_BETWEEN(10,0,5));

	int n = 0;
	char** cs = load_file("./test", 0, &n);
	for (int i = 0; i<n; i++) {
		printf("%s\n", cs[i]);
	}
	free2d((void**)cs, n);

	char str[128] = "These are the tokens of my string";
	cs = split(str, " ", &n);
	for (int i = 0; i<n; i++) {
		printf("'%s'\n", cs[i]);
	}
	free2d((void**)cs, n);

	strcpy(str, "12 34 56 78");
	cs = split(str, " ", &n);
	for (int i = 0; i<n; i++) {
		printf("'%s' ", cs[i]);
	}
	printf("\n");
	int* is = sstoi(cs, n);
	int sum = 0;
	for (int i = 0; i<n; i++) {
		sum += is[i];
	}
	DUMP("%d", sum);
	free2d((void**)cs, n);

	strcpy(str, "922337203685477580 223372036854775808");
	cs = split(str, " ", &n);
	for (int i = 0; i<n; i++) {
		printf("'%s' ", cs[i]);
	}
	printf("\n");
	long long* lls = sstoll(cs, n);
	long long lsum = 0;
	for (int i = 0; i<n; i++) {
		lsum += lls[i];
	}
	DUMP("%lld", lsum);

	free2d((void**)cs, n);
	free(is);
	free(lls);

	return 0;
}
