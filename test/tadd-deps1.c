#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include "initd.h"

static void print_sk_list(const initd_list_t *list, initd_sk_t sk);

int main(int argc, char *argv[])
{
	initd_list_t *all, *startlist, *stoplist;
	initd_t *a, *b, *c, *d, *r;
	dep_t *need;

	a = initd_new("a");
	initd_add_prov(a, "a");
	initd_set_rc(a, KEY_DSTART, RC_5);
	initd_set_rc(a, KEY_DSTOP, (RC_0|RC_1|RC_2|RC_3|RC_4|RC_6));
	initd_add_rstart(a, "b");
	initd_add_rstop(a, "b");
	initd_add_rstart(a, "d");
	initd_add_rstop(a, "d");

	b = initd_new("b");
	initd_add_prov(b, "b");
	initd_set_rc(b, KEY_DSTART, (RC_3|RC_4|RC_5));
	initd_set_rc(b, KEY_DSTOP, (RC_0|RC_6));
	initd_add_rstart(b, "c");
	initd_add_rstop(b, "c");
	initd_add_sstart(b, "q");

	c = initd_new("c");
	initd_add_prov(c, "c");
	initd_set_rc(c, KEY_DSTART, RC_S);
	initd_set_rc(c, KEY_DSTOP, (RC_0|RC_6));
	initd_add_rstop(b, "$l");

	d = initd_new("d");
	initd_add_prov(d, "d");
	initd_set_rc(d, KEY_DSTART, RC_5);
	initd_set_rc(d, KEY_DSTOP, (RC_0|RC_1|RC_2|RC_3|RC_4|RC_6));
	initd_add_rstart(d, "c");
	initd_add_rstop(d, "c");

	r = initd_new("r");
	initd_add_prov(r, "$l");
	initd_set_rc(r, KEY_DSTART, RC_S);
	initd_set_rc(r, KEY_DSTOP, (RC_0|RC_6));
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
	startlist = initd_add_recurse_deps(all, SK_START, need);
	if (startlist) {
		print_sk_list(startlist, SK_START);
	} else {
		fprintf(stderr, "Failed recursing start scripts\n");
		return 1;
	}

	stoplist = initd_add_recurse_deps(all, SK_STOP, need);
	if (stoplist) {
		print_sk_list(stoplist, SK_STOP);
	} else {
		fprintf(stderr, "Failed recursing stop scripts\n");
		return 1;
	}

	return 0;
}

static void print_sk_list(const initd_list_t *list, initd_sk_t sk)
{
	char *startstop;
	initd_node_t *inp;

	switch (sk) {
	case SK_START:
		startstop = "start";
		break;
	case SK_STOP:
		startstop = "stop";
		break;
	default:
		startstop = "";
	}

	if (list && list->first) {
		printf("All deps for %s\n", startstop);
		printf("Ordered:");
		for (inp = list->first; inp; inp = inp->next)
			printf(" %s", inp->initd->name);
		printf("\n");
	} else {
		printf("%s list is empty\n", startstop);
	}
}
