#include <stdio.h>
#include "dep.h"

static void print_dep(dep_t *dp);
static void print_list(dep_list_t *dl);

int main(int argc, char *argv[])
{
	dep_t *a, *b;
	dep_list_t *l;

	/* test single deps */
	a = dep_new("test");
	printf("Created dep with name '%s'\n", a->name);

	dep_add(a, "first");
	dep_add(a, "second");

	print_dep(a);

	/* copy dep */
	b = dep_copy(a);
	printf("Contents of copied dep:\n");
	print_dep(b);

	dep_free(a);
	dep_free(b);

	/* test dep list */
	a = dep_new("a");
	dep_add(a, "a1");
	dep_add(a, "a2");
	b = dep_new("b");
	dep_add(b, "b1");
	dep_add(b, "b2");
	dep_add(b, "b3");

	l = dep_list_new();
	dep_list_add(l, a);
	dep_list_add(l, b);

	print_list(l);
	dep_list_free(l);

	return 0;
}

static void print_dep(dep_t *dp)
{
	int n;
	if (!dp) {
		printf("No dep_t provided");
		return;
	}
	printf("Dep \"%s\" has %d dependencies:\n", dp->name,
		dp->ndeps);
	for (n = 0; n < dp->ndeps; n++)
		printf(" %d: %s\n", n, dp->deps[n]);
}

static void print_list(dep_list_t *dl)
{
	dep_t *cur;
	if (!dl) {
		printf("No dep_list_t provided");
		return;
	}
	printf("Dep list contents:\n");
	for (cur = dl->first; cur; cur = cur->next)
		print_dep(cur);
}
