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
	dep_t *need = dep_new();

	if (argc <= 1)
		dep_add(need, "foo");

	while (argc > 1) {
		dep_add(need, argv[1]);
		argv++;
		argc--;
	}

	all = initd_list_from_dir("init.d");

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
	initd_t *ip;

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
		for (ip = list->first; ip; ip = ip->next)
			printf(" %s", ip->name);
		printf("\n");
	} else {
		printf("%s list is empty\n", startstop);
	}
}
