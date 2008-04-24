#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include "initd.h"
#include "types.h"
#include "rdep.h"

static void print_sk_list(const initd_list_t *list, initd_sk_t sk);

int main(int argc, char *argv[])
{
	initd_list_t *all, *startlist, *stoplist;
	initd_t *a, *b, *c, *d, *r;
	dep_t *need;

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
	initd_add_sstart(b, "q");

	c = initd_new("c");
	initd_add_prov(c, "c");
	initd_add_rstop(b, "$l");

	d = initd_new("d");
	initd_add_prov(d, "d");
	initd_add_rstart(d, "c");
	initd_add_rstop(d, "c");

	r = initd_new("r");
	initd_add_prov(r, "$l");
	initd_add_rstop(r, "d");
	initd_add_sstop(r, "q");

	all = initd_list_new();
	initd_list_add(all, a);
	initd_list_add(all, b);
	initd_list_add(all, c);
	initd_list_add(all, d);
	initd_list_add(all, r);

	need = dep_new();
	dep_add(need, "a");

	initd_recurse_set_verbose(true);
	startlist = initd_recurse_deps(all, RC_START, need);
	print_sk_list(startlist, RC_START);

	stoplist = initd_recurse_deps(all, RC_STOP, need);
	print_sk_list(stoplist, RC_STOP);

	return 0;
}

static void print_sk_list(const initd_list_t *list, initd_sk_t sk)
{
	char *startstop;
	initd_t *ip;

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
		printf("All deps for %s\n", startstop);
		printf("Ordered:");
		for (ip = list->first; ip; ip = ip->next)
			printf(" %s", ip->name);
		printf("\n");
	} else {
		printf("%s list is empty\n", startstop);
	}
}
