#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include "dep.h"
#include "str.h"

dep_t *dep_new(const char *name) {
	dep_t *new = malloc(sizeof(dep_t));
	if (!new)
		error(2, errno, "%s", __FUNCTION__);

	new->name = d_string_new(name);

	new->deps = strarg_new();

	new->prev = NULL;
	new->next = NULL;

	return new;
}

void dep_free(dep_t *dep)
{
	/* free the members of the deps array */
	strarg_free(dep->deps);

	d_string_free(dep->name);

	free(dep);
	dep = NULL;
}

void dep_add(dep_t *dp, const char *dep)
{
	/* create a new unnamed dep_t if necessary */
	if (!dp)
		dp = dep_new("");

	strarg_add(dp->deps, dep);
}

dep_t *dep_copy(dep_t *source)
{
	dep_t *dest;
	char *name;
	int n;

	if (!source) {
		dest = dep_new("");
		goto out;
	}

	if (source->name)
		name = source->name;
	else
		name = "";

	dest = dep_new(name);

	for (n = 0; n < source->deps->nstr; n++)
		dep_add(dest, source->deps->str[n]);

out:
	return dest;
}

/* Verify that a given named dep exists in the dep_t. Returns 0 for
 * success and 1 for failure.
 */
int dep_exists_name(dep_t *dp, const char *name)
{
	int n, ret = 1;

	if (!dp)
		goto out;

	for (n = 0; n < dp->deps->nstr; n++) {
		if (strcmp(name, dp->deps->str[n]) == 0) {
			ret = 0;
			break;
		}
	}
out:
	return ret;
}
