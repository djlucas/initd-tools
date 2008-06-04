#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "initd.h"

static void print_initd(initd_t *ip);
static void print_level(initd_rc_t rc);

int main(int argc, char *argv[])
{
	initd_t *t;
	char *tpath;

	if (argc > 1) {
		tpath = argv[1];
	} else {
		fprintf(stderr, "No script supplied to tparse\n");
		exit(1);
	}

	printf("Opening script %s\n", tpath);
	t = initd_parse(tpath);
	if (t)
		print_initd(t);
	else
		printf("Invalid script %s, skipping\n", tpath);

	return 0;
}

static void print_initd(initd_t *ip)
{
	int n;

	printf("%s: %s\n\n%s\n\n", ip->name, ip->sdesc, ip->desc);
	printf("Provides:");
	for (n = 0; n < prov_get_num(ip->prov); n++)
		printf(" %s", prov_get_prov(ip->prov, n));
	putchar('\n');
	printf("Default start levels:");
	print_level(ip->dstart);
	putchar('\n');
	printf("Default stop levels:");
	print_level(ip->dstop);
	putchar('\n');

	printf("Required start after:");
	for (n = 0; n < dep_get_num(ip->rstart); n++)
		printf(" %s", dep_get_dep(ip->rstart, n));
	putchar('\n');
	printf("Required stop before:");
	for (n = 0; n < dep_get_num(ip->rstop); n++)
		printf(" %s", dep_get_dep(ip->rstop, n));
	putchar('\n');
	printf("Should start after:");
	for (n = 0; n < dep_get_num(ip->sstart); n++)
		printf(" %s", dep_get_dep(ip->sstart, n));
	putchar('\n');
	printf("Should stop before:");
	for (n = 0; n < dep_get_num(ip->sstop); n++)
		printf(" %s", dep_get_dep(ip->sstop, n));
	putchar('\n');
}

static void print_level(initd_rc_t rc)
{
	initd_rc_t lev;

	for (lev = RC_S; lev <= RC_6; lev = (lev << 1)) {
		if (rc & lev)
			printf(" %c", initd_rc_level_char(lev));
	}
}
