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

	if (strarg_exists(test, num, "world"))
		printf("Found \"world\" in test\n");
	else
		printf("Didn't find \"world\" in test\n");

	if (strarg_exists(test, num, "barf"))
		printf("Found \"barf\" in test\n");
	else
		printf("Didn't find \"barf\" in test\n");

	strarg_free(test, num);

	return 0;
}
