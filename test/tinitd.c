#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include "initd.h"

static void print_initd(initd_t *ip);

int main(int argc, char *argv[])
{
	initd_t *a, *b;

	/* test single initds */
	a = initd_new("test");
	printf("Created initd with name '%s'\n", a->name);

	initd_add_prov(a, "service1");
	initd_add_prov(a, "service2");

	initd_add_rstart(a, "$network");
	initd_add_rstop(a, "$network");
	initd_add_sstart(a, "dbus");
	initd_add_sstop(a, "dbus");

	initd_set_sdesc(a, "A test service");
	initd_set_desc(a, "This is a dummy test service");
	initd_add_desc(a, "just for testing our functions");

	print_initd(a);

	/* copy initd */
	b = initd_copy(a);
	printf("Contents of copied initd:\n");
	print_initd(b);

	if (initd_provides(b, "service1"))
		printf("Found service \"service1\" in b\n");
	else
		printf("Fail finding service \"service1\" in b\n");

	initd_free(a);
	initd_free(b);

	return 0;
}

static void print_initd(initd_t *ip)
{
	int n;
	if (!ip) {
		printf("No initd provided");
		return;
	}

	printf("initd \"%s\": %s\n", ip->name, ip->sdesc);
	printf("Description: %s\n", ip->desc);

	printf("initd \"%s\" provides %d services:\n", ip->name,
		ip->prov->nprov);
	for (n = 0; n < ip->prov->nprov; n++)
		printf(" %d: %s\n", n, ip->prov->prov[n]);

	printf("initd \"%s\" must start after %d services:\n", ip->name,
		ip->rstart->ndep);
	for (n = 0; n < ip->rstart->ndep; n++)
		printf(" %d: %s\n", n, ip->rstart->dep[n]);

	printf("initd \"%s\" must stop before %d services:\n", ip->name,
		ip->rstop->ndep);
	for (n = 0; n < ip->rstop->ndep; n++)
		printf(" %d: %s\n", n, ip->rstop->dep[n]);

	printf("initd \"%s\" should start after %d services:\n", ip->name,
		ip->sstart->ndep);
	for (n = 0; n < ip->sstart->ndep; n++)
		printf(" %d: %s\n", n, ip->sstart->dep[n]);

	printf("initd \"%s\" should stop before %d services:\n", ip->name,
		ip->sstop->ndep);
	for (n = 0; n < ip->sstop->ndep; n++)
		printf(" %d: %s\n", n, ip->sstop->dep[n]);
}
