#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include "dep.h"

dep_t *dep_new(void)
{
	dep_t *dp = malloc(sizeof(dep_t));
	if (!dp)
		error(2, errno, "%s", __FUNCTION__);

	dp->dep = strarg_new(&dp->ndep);

	return dp;
}

void dep_free(dep_t *dp)
{
	if (!dp)
		return;

	strarg_free(dp->dep, dp->ndep);

	free(dp);
	dp = NULL;
}

void dep_add(dep_t *dp, const char *name)
{
	if (!dp)
		dp = dep_new();

	dp->dep = strarg_add(dp->dep, &dp->ndep, name);
}

dep_t *dep_copy(dep_t *source)
{
	int n;
	dep_t *tgt = dep_new();

	if (!source)
		goto out;

	for (n = 0; n < source->ndep; n++)
		dep_add(tgt, source->dep[n]);

out:
	return tgt;
}
