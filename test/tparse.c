#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "initd.h"
#include "str.h"

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
	for (n = 0; n < ip->prov->nprov; n++)
		printf(" %s", ip->prov->prov[n]);
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
	if (rc & RC_SI) printf(" sysinit");
	if (rc & RC_0) printf(" 0");
	if (rc & RC_1) printf(" 1");
	if (rc & RC_2) printf(" 2");
	if (rc & RC_3) printf(" 3");
	if (rc & RC_4) printf(" 4");
	if (rc & RC_5) printf(" 5");
	if (rc & RC_6) printf(" 6");
}
