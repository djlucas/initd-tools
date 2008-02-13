#include <stdio.h>
#include "deps.h"

int main(int argc, char *argv[])
{
	int n;
	dep_t *d = dep_new("test");
	printf("Created dep with name '%s'\n", d->name);

	dep_add(d, "first");
	dep_add(d, "second");

	for (n = 0; n < d->ndeps; n++)
		printf("Arg %d: %s\n", n, d->deps[n]);

	dep_free(d);

	return 0;
}
