#include <stdio.h>
#include <string.h>
#include "initd.h"

static void print_initd(initd_t *ip);

int main(int argc, char *argv[])
{
	initd_t *a, *b;

	/* test single initds */
	a = initd_new("test");
	printf("Created initd with name '%s'\n", a->name);

	initd_add(a, "first");
	initd_add(a, "second");

	print_initd(a);

	/* copy initd */
	b = initd_copy(a);
	printf("Contents of copied initd:\n");
	print_initd(b);

	if (initd_exists_name(b, "first") == 0)
		printf("Found dep \"first\" in b\n");
	else
		printf("Fail finding dep \"first\" in b\n");

	initd_free(a);
	initd_free(b);

	return 0;
}

static void print_initd(initd_t *ip)
{
	int n;
	if (!ip) {
		printf("No initd provided");
		return;
	}
	printf("initd \"%s\" has %d dependencies:\n", ip->name,
		ip->deps->ndep);
	for (n = 0; n < ip->deps->ndep; n++)
		printf(" %d: %s\n", n, ip->deps->dep[n]);
}
