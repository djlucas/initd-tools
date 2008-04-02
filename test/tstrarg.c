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
	strarg_t *test = malloc(sizeof(strarg_t));
	if (!test)
		error(2, errno, "malloc");

	test->str = strarg_new(&test->nstr);

	test->str = strarg_add(test->str, &test->nstr, "hello");
	test->str = strarg_add(test->str, &test->nstr, "world");
	test->str = strarg_add(test->str, &test->nstr, "it's");
	test->str = strarg_add(test->str, &test->nstr, "me");

	printf("Contents of test strarg\n");
	for (n = 0; n < strarg_get_num(test); n++)
		printf("Arg %d: %s\n", n, strarg_get_str(test, n));

	if (strarg_exists(test->str, test->nstr, "world"))
		printf("Found \"world\" in test\n");
	else
		printf("Didn't find \"world\" in test\n");

	if (strarg_exists(test->str, test->nstr, "barf"))
		printf("Found \"barf\" in test\n");
	else
		printf("Didn't find \"barf\" in test\n");

	printf("Removing last two elements\n");
	test->str = strarg_pop(test->str, &test->nstr);
	test->str = strarg_pop(test->str, &test->nstr);

	printf("Contents of test strarg\n");
	for (n = 0; n < strarg_get_num(test); n++)
		printf("Arg %d: %s\n", n, strarg_get_str(test, n));

	strarg_free(test->str, test->nstr);
	free(test);
	test = NULL;

	return 0;
}
