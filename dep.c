#define _GNU_SOURCE

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

	dp->ndep = 0;

	dp->dep = malloc(sizeof(char *));
	if (!dp->dep)
		error(2, errno, "%s", __FUNCTION__);
	*dp->dep = NULL;

	return dp;
}

void dep_free(dep_t *dp)
{
	int n;

	if (!dp)
		return;

	for (n = 0; n < dp->ndep; n++)
		d_string_free(dp->dep[n]);

	free(dp);
	dp = NULL;
}

void dep_add(dep_t *dp, const char *name)
{
	if (!dp)
		dp = dep_new();

	dp->dep = realloc(dp->dep, (dp->ndep + 2) * sizeof(char *));
	if (!dp->dep)
		error(2, errno, "%s", __FUNCTION__);

	dp->dep[dp->ndep++] = d_string_new(name);
	dp->dep[dp->ndep] = NULL;
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
