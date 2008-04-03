#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include "str.h"

int main(int argc, char *argv[])
{
	int n;
	strarg_t *test = strarg_new();

	strarg_add(test, "hello");
	strarg_add(test, "world");
	strarg_add(test, "it's");
	strarg_add(test, "me");

	printf("Contents of test strarg\n");
	for (n = 0; n < strarg_get_num(test); n++)
		printf("Arg %d: %s\n", n, strarg_get_str(test, n));

	if (strarg_exists(test, "world"))
		printf("Found \"world\" in test\n");
	else
		printf("Didn't find \"world\" in test\n");

	if (strarg_exists(test, "barf"))
		printf("Found \"barf\" in test\n");
	else
		printf("Didn't find \"barf\" in test\n");

	printf("Removing last two elements\n");
	strarg_pop(test);
	strarg_pop(test);

	printf("Contents of test strarg\n");
	for (n = 0; n < strarg_get_num(test); n++)
		printf("Arg %d: %s\n", n, strarg_get_str(test, n));

	strarg_free(test);

	return 0;
}
