#include <stdio.h>
#include "dep.h"

int main(int argc, char *argv[])
{
	dep_t *d1, *d2, *d3, *d4;
	dep_list_t *dl1, *dl2;
	char *miss;

	/* test dep list */
	d1 = dep_new("d1");
	dep_add(d1, "d2");
	d2 = dep_new("d2");
	dep_add(d2, "d3");
	d3 = dep_new("d3");
	d4 = dep_new("d4");
	dep_add(d4, "barf");

	dl1 = dep_list_new();
	dep_list_add(dl1, d1);
	dep_list_add(dl1, d2);
	dep_list_add(dl1, d3);

	/* another list, but with the bogus d4 element */
	dl2 = dep_list_copy(dl1);
	dep_list_add(dl2, d4);

	miss = dep_list_verify_all(dl1);
	if (miss)
		printf("Dep %s was not found in the dl1 list\n", miss);
	else
		printf("Found all deps in the dl1 list\n");
	free(miss);

	miss = dep_list_verify_all(dl2);
	if (miss)
		printf("Dep %s was not found in the dl2 list\n", miss);
	else
		printf("Found all deps in the dl2 list\n");
	free(miss);

	dep_list_free(dl1);
	dep_list_free(dl2);

	return 0;
}
