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

	new->ndeps = 0;

	new->deps = malloc(sizeof(char *));
	if (!new->deps)
		error(2, errno, "%s", __FUNCTION__);
	*new->deps = NULL;

	new->prev = NULL;
	new->next = NULL;

	return new;
}

void dep_free(dep_t *dep)
{
	int n;

	/* free the members of the deps array */
	for (n = 0; n < dep->ndeps; n++)
		d_string_free(dep->deps[n]);
	dep->deps = NULL;

	d_string_free(dep->name);

	free(dep);
	dep = NULL;
}

void dep_add(dep_t *dp, const char *dep)
{
	/* create a new unnamed dep_t if necessary */
	if (!dp)
		dp = dep_new("");

	dp->deps = realloc(dp->deps, (dp->ndeps + 2) * sizeof(char *));
	if (!dp->deps)
		error(2, errno, "%s", __FUNCTION__);

	dp->deps[dp->ndeps++] = d_string_new(dep);
	dp->deps[dp->ndeps] = NULL;
}

dep_t *dep_copy(dep_t *old)
{
	dep_t *new;
	char *name;
	int n;

	if (!old) {
		new = dep_new("");
		goto out;
	}

	if (old->name)
		name = old->name;
	else
		name = "";

	new = dep_new(name);

	for (n = 0; n < old->ndeps; n++)
		dep_add(new, old->deps[n]);

out:
	return new;
}

/* Verify that a given named dep exists in the dep_t. Returns 0 for
 * success and 1 for failure.
 */
int dep_exists_name(dep_t *dp, const char *name)
{
	int n, ret = 1;

	if (!dp)
		goto out;

	for (n = 0; n < dp->ndeps; n++) {
		if (strcmp(name, dp->deps[n]) == 0) {
			ret = 0;
			break;
		}
	}
out:
	return ret;
}
