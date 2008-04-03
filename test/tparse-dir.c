#include <stdio.h>
#include "initd.h"

static void print_list(const initd_list_t *list);

int main(int argc, char *argv[])
{
	char *dir;
	initd_list_t *test;

	if (argc > 1)
		dir = argv[1];
	else
		dir = ".";

	test = initd_list_from_dir(dir);
	if (test)
		print_list(test);

	return 0;
}

static void print_list(const initd_list_t *list)
{
	initd_t *ip;
	int n;
	for (ip = list->first; ip; ip = ip->next) {
		printf("Service %s provides:", ip->name);
		for (n = 0; n < prov_get_num(ip->prov); n++)
			printf(" %s", prov_get_prov(ip->prov, n));
		printf("\n");
	}
}
