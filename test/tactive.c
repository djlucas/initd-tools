#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "initd.h"
#include "str.h"

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

	if (!ilp)
		return;

	for (ip = ilp->first; ip; ip = ip->next) {
		printf("%s active start levels:", ip->name);
		print_rc_string(ip, RC_START);

		printf("%s active stop levels:", ip->name);
		print_rc_string(ip, RC_STOP);
	}
}

static void print_rc_string(const initd_t *ip, initd_sk_t sk)
{
	if (initd_is_active(ip, RC_S, sk))
		printf(" S");
	if (initd_is_active(ip, RC_0, sk))
		printf(" 0");
	if (initd_is_active(ip, RC_1, sk))
		printf(" 1");
	if (initd_is_active(ip, RC_2, sk))
		printf(" 2");
	if (initd_is_active(ip, RC_3, sk))
		printf(" 3");
	if (initd_is_active(ip, RC_4, sk))
		printf(" 4");
	if (initd_is_active(ip, RC_5, sk))
		printf(" 5");
	if (initd_is_active(ip, RC_6, sk))
		printf(" 6");
	printf("\n");
}
