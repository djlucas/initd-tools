#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include "dep.h"

static void print_dep(dep_t *sa);

int main(int argc, char *argv[])
{
	dep_t *foo, *bar;
	printf("Creating new dep foo\n");
	foo = dep_new();

	dep_add(foo, "this");
	dep_add(foo, "is");
	dep_add(foo, "a");
	dep_add(foo, "test");

	printf("Contents of foo dep:\n");
	print_dep(foo);

	bar = dep_copy(foo);
	dep_add(bar, "again");

	print_dep(bar);

	dep_free(foo);
	dep_free(bar);

	return 0;
}

void print_dep(dep_t *dp)
{
	int n;

	if (!dp)
		return;

	for (n = 0; n < dp->ndep; n++)
		printf("Dep %d: %s\n", n, dp->dep[n]);
}
