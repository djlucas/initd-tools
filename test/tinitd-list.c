#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "initd.h"

static void print_initd_list(initd_list_t *ilp);

int main(int argc, char *argv[])
{
	initd_t *i1, *i2, *i3, *i4;
	initd_list_t *il1, *il2;
	char *miss;

	/* test dep list */
	i1 = initd_new("i1");
	initd_add(i1, "i2");
	i2 = initd_new("i2");
	initd_add(i2, "i3");
	i3 = initd_new("i3");
	i4 = initd_new("i4");
	initd_add(i4, "barf");

	il1 = initd_list_new();
	initd_list_add(il1, i1);
	initd_list_add(il1, i2);
	initd_list_add(il1, i3);

	/* another list, but with the bogus i4 element */
	il2 = initd_list_copy(il1);
	initd_list_add(il2, i4);

	printf("il1 ");
	print_initd_list(il1);
	printf("il2 ");
	print_initd_list(il2);

	miss = initd_list_verify_all(il1);
	if (miss)
		printf("dep %s was not found in the il1 list\n", miss);
	else
		printf("Found all deps in the il1 list\n");
	free(miss);

	miss = initd_list_verify_all(il2);
	if (miss)
		printf("dep %s was not found in the il2 list\n", miss);
	else
		printf("Found all deps in the il2 list\n");
	free(miss);

	initd_list_free(il1);
	initd_list_free(il2);

	return 0;
}

static void print_initd_list(initd_list_t *ilp)
{
	initd_t *cur;
	if (!ilp) {
		printf("No initd list provided");
		return;
	}
	printf("initd list contents:\n");
	for (cur = ilp->first; cur; cur = cur->next)
		printf(" %s\n", cur->name);
}
