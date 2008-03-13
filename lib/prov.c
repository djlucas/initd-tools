#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include "prov.h"
#include "str.h"

prov_t *prov_new(void)
{
	prov_t *pp = malloc(sizeof(prov_t));
	if (!pp)
		error(2, errno, "%s", __FUNCTION__);

	pp->prov = strarg_new(&pp->nprov);

	return pp;
}

void prov_free(prov_t *pp)
{
	if (!pp)
		return;

	strarg_free(pp->prov, pp->nprov);

	free(pp);
	pp = NULL;
}

void prov_add(prov_t *pp, const char *name)
{
	if (!pp)
		pp = prov_new();

	pp->prov = strarg_add(pp->prov, &pp->nprov, name);
}

prov_t *prov_copy(prov_t *source)
{
	int n;
	prov_t *tgt = prov_new();

	if (!source)
		goto out;

	for (n = 0; n < source->nprov; n++)
		prov_add(tgt, source->prov[n]);

out:
	return tgt;
}
