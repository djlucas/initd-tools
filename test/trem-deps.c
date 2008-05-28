#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include "initd.h"
#include "types.h"
#include "rdep.h"

static void print_sk_list(const initd_list_t *list, initd_sk_t sk);
static bool check_scripts_active(const initd_list_t *list, dep_t *scripts);

int main(int argc, char *argv[])
{
	initd_list_t *all, *startlist, *stoplist;
	dep_t *rem = dep_new();

	if (argc <= 1) {
		dep_add(rem, "network");
		dep_add(rem, "bar");
		dep_add(rem, "foo");
	}

	while (argc > 1) {
		dep_add(rem, argv[1]);
		argv++;
		argc--;
	}

	all = initd_list_from_dir("init.d");

	if (!check_scripts_active(all, rem))
		return 1;

	initd_recurse_set_verbose(true);
	startlist = initd_remove_recurse_deps(all, SK_START, rem);
	if (startlist) {
		print_sk_list(startlist, SK_START);
	} else {
		fprintf(stderr, "Failed recursing start scripts\n");
		return 1;
	}

	stoplist = initd_remove_recurse_deps(all, SK_STOP, rem);
	if (stoplist) {
		print_sk_list(stoplist, SK_STOP);
	} else {
		fprintf(stderr, "Failed recursing stop scripts\n");
		return 1;
	}

	if (!check_scripts_active(all, rem))
		return 1;

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
		printf("All deps for %s after removal\n", startstop);
		printf("Ordered:");
		for (ip = list->first; ip; ip = ip->next)
			printf(" %s", ip->name);
		printf("\n");
	} else {
		printf("%s list is empty\n", startstop);
	}
}

static bool check_scripts_active(const initd_list_t *list, dep_t *scripts)
{
	int n;
	initd_t *ip;

	/* Check that all the requested scripts are active */
	for (n = 0; n < dep_get_num(scripts); n++) {
		ip = initd_list_find_name(list, dep_get_dep(scripts, n));
		if (!ip) {
			fprintf(stderr, "No script named %s\n",
				dep_get_dep(scripts, n));
			return false;
		}
		if (!(ip->astart & RC_ALL) || !(ip->astop & RC_ALL)) {
			fprintf(stderr, "Script %s is not active\n",
				ip->name);
			return false;
		}
	}

	return true;
}
