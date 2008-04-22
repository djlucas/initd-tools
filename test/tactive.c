#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "initd.h"
#include "str.h"

static void print_actives(const initd_list_t *ilp);
static void print_rc_string(const initd_rc_t rc);

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

	if (!ilp)
		return;

	for (ip = ilp->first; ip; ip = ip->next) {
		printf("%s active start levels:", ip->name);
		print_rc_string(ip->astart);

		printf("%s active stop levels:", ip->name);
		print_rc_string(ip->astop);
	}
}

static void print_rc_string(const initd_rc_t rc)
{
	if (rc & RC_S)
		printf(" S");
	if (rc & RC_0)
		printf(" 0");
	if (rc & RC_1)
		printf(" 1");
	if (rc & RC_2)
		printf(" 2");
	if (rc & RC_3)
		printf(" 3");
	if (rc & RC_4)
		printf(" 4");
	if (rc & RC_5)
		printf(" 5");
	if (rc & RC_6)
		printf(" 6");
	printf("\n");
}
