#include <stdlib.h>
#include <stdio.h>
#include "str.h"

int main(int argc, char *argv[])
{
	int n;
	unsigned int num;
	char **test;

	test = strarg_new(&num);

	test = strarg_add(test, &num, "hello");
	test = strarg_add(test, &num, "world");
	test = strarg_add(test, &num, "it's");
	test = strarg_add(test, &num, "me");

	printf("Contents of test strarg\n");
	for (n = 0; n < num; n++)
		printf("Arg %d: %s\n", n, test[n]);

	strarg_free(test, num);

	return 0;
}
