#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "initd.h"

static void print_actives(const initd_list_t *ilp);
static void print_rc_string(const initd_t *ip, initd_sk_t sk);

int main(int argc, char *argv[])
{
	char *dir;
	initd_list_t *list;

	if (argc > 1)
		dir = argv[1];
	else
		dir = "init.d";

	list = initd_list_from_dir(dir);
	if (!list) {
		fprintf(stderr, "Error reading scripts from %s\n", dir);
		exit(1);
	}

	initd_list_set_actives(list, dir);
	print_actives(list);

	return 0;
}

static void print_actives(const initd_list_t *ilp)
{
	initd_t *ip;
	int n, nstr;

	if (!ilp)
		return;

	for (ip = ilp->first; ip; ip = ip->next) {
		printf("%s active start levels:", ip->name);
		print_rc_string(ip, SK_START);

		nstr = ip->astart_links->nstr;
		if (nstr > 0) {
			printf("%s active start links:", ip->name);
			for (n = 0; n < nstr; n++)
				printf(" %s", ip->astart_links->str[n]);
			printf("\n");
		}

		printf("%s active stop levels:", ip->name);
		print_rc_string(ip, SK_STOP);

		nstr = ip->astop_links->nstr;
		if (nstr > 0) {
			printf("%s active stop links:", ip->name);
			for (n = 0; n < nstr; n++)
				printf(" %s", ip->astop_links->str[n]);
			printf("\n");
		}
	}
}

static void print_rc_string(const initd_t *ip, initd_sk_t sk)
{
	initd_rc_t rc;

	for (rc = RC_S; rc <= RC_6; rc = (rc << 1)) {
		if (initd_is_active(ip, rc, sk))
			printf(" %c", initd_rc_level_char(rc));
	}
	printf("\n");
}
