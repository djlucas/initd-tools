#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include "deps.h"
#include "str.h"

static void dep_grow(dep_t *dp, unsigned int num);

dep_t *dep_new(const char *name) {
	dep_t *new = malloc(sizeof(dep_t));
	if (!new)
		error(2, errno, "%s", __FUNCTION__);

	new->name = d_string_new(name);

	new->ndeps = 0;

	new->deps = malloc(sizeof(char *));
	if (!new->deps)
		error(2, errno, "%s", __FUNCTION__);
	*new->deps = NULL;

	return new;
}

void dep_free(dep_t *dep)
{
	int n;

	/* free the members of the deps array */
	for (n = 0; n < dep->ndeps; n++)
		free(dep->deps[n]);

	free(dep->name);

	free(dep);
	dep = NULL;
}

static void dep_grow(dep_t *dp, unsigned int num)
{
	unsigned int n = dp->ndeps;
	int i;

	dp->deps = realloc(dp->deps, (n + num + 1) * sizeof(char *));
	if (!dp->deps)
		error(2, errno, "%s", __FUNCTION__);

	for (i = n + 1; i < (n + num + 1); i++)
		dp->deps[i] = NULL;
}

void dep_add(dep_t *dp, const char *dep)
{
	/* create a new unnamed dep_t if necessary */
	if (!dp)
		dp = dep_new("");
	dep_grow(dp, 1);
	dp->deps[dp->ndeps++] = d_string_new(dep);
}
