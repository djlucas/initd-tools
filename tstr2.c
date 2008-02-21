#include <stdio.h>
#include <string.h>
#include "str.h"

static void print_strarg(strarg_t *sa);

int main(int argc, char *argv[])
{
	strarg_t *foo = strarg_new();

	strarg_add(foo, "this");
	strarg_add(foo, "is");
	strarg_add(foo, "a");
	strarg_add(foo, "test");

	print_strarg(foo);

	strarg_free(foo);

	return 0;
}

void print_strarg(strarg_t *sa)
{
	int n;

	if (!sa)
		return;

	for (n = 0; n < sa->nstr; n++)
		printf("String %d: %s\n", n, sa->str[n]);
}
