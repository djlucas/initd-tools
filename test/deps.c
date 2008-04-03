#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include "initd.h"
#include "types.h"
#include "rdep.h"

static void print_sk_list(const dep_t *list, initd_sk_t sk);

int main(int argc, char *argv[])
{
	initd_list_t *all;
	initd_t *a, *b, *c, *d;
	dep_t *startlist, *stoplist, *need;

	a = initd_new("a");
	initd_add_prov(a, "a");
	initd_add_rstart(a, "b");
	initd_add_rstop(a, "b");
	initd_add_rstart(a, "d");
	initd_add_rstop(a, "d");

	b = initd_new("b");
	initd_add_prov(b, "b");
	initd_add_rstart(b, "c");
	initd_add_rstop(b, "c");

	c = initd_new("c");
	initd_add_prov(c, "c");

	d = initd_new("d");
	initd_add_prov(d, "d");
	initd_add_rstart(d, "c");
	initd_add_rstop(d, "c");

	all = initd_list_new();
	initd_list_add(all, a);
	initd_list_add(all, b);
	initd_list_add(all, c);
	initd_list_add(all, d);

	need = dep_new();
	dep_add(need, "a");

	startlist = initd_recurse_deps(all, RC_START, need);
	print_sk_list(startlist, RC_START);

	/* FIXME: initd_recurse_deps can only be called once
	stoplist = initd_recurse_deps(all, RC_STOP, need);
	print_sk_list(stoplist, RC_STOP);
	*/

	return 0;
}

static void print_sk_list(const dep_t *list, initd_sk_t sk)
{
	char *startstop;
	int n;

	switch (sk) {
	case RC_START:
		startstop = "start";
		break;
	case RC_STOP:
		startstop = "stop";
		break;
	default:
		startstop = "";
	}

	if (list) {
		printf("%d deps for %s\n", dep_get_num(list), startstop);
		printf("Ordered:");
		for (n = 0; n < dep_get_num(list); n++)
			printf(" %s", dep_get_dep(list, n));
		printf("\n");
	} else {
		printf("%s list is empty\n", startstop);
	}

}
