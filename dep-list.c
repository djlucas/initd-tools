#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include "dep.h"
#include "str.h"

dep_list_t *dep_list_new(void)
{
	dep_list_t *new = malloc(sizeof(dep_list_t));
	if (!new)
		error(2, errno, "%s", __FUNCTION__);

	new->first = NULL;
	new->last = NULL;

	return new;
}

void dep_list_free(dep_list_t *dlp)
{
	dep_t *cur;

	/* free the members of the deps array */
	for (cur = dlp->first; cur; cur = cur->next)
		dep_free(cur);

	free(dlp);
	dlp = NULL;
}

void dep_list_add(dep_list_t *dlp, dep_t *dp)
{
	dep_t *cur;

	/* create a new list if necessary */
	if (!dlp)
		dlp = dep_list_new();

	/* just return if NULL was passed as dep */
	if (!dp)
		return;

	cur = dlp->last;
	if (!cur) {
		/* this is the first element */
		dlp->first = dp;
	} else {
		cur->next = dp;
		dp->prev = cur;
	}
	dlp->last = dp;
}

dep_list_t *dep_list_copy(dep_list_t *old)
{
	dep_t *dold, *dnew;
	dep_list_t *new = dep_list_new();

	if (!old)
		goto out;

	for (dold = old->first; dold; dold = dold->next) {
		dnew = dep_copy(dold);
		dep_list_add(new, dnew);
	}

out:
	return new;
}

dep_t *dep_list_find_name(dep_list_t *dlp, const char *name)
{
	dep_t *dp = NULL;

	if (!(name || dlp))
		goto out;

	for (dp = dlp->first; dp; dp = dp->next) {
		if (strcmp(dp->name, name) == 0)
			break;
	}
out:
	return dp;
}

/* Verify that a given named dep exists in the list. Returns 0 for
 * success and 1 for failure.
 */
int dep_list_exists_name(dep_list_t *dlp, const char *name)
{
	if (dep_list_find_name(dlp, name))
		return 0;
	else
		return 1;
}

/* Ensure that the sub-dependencies for each dep_t in the list exist in
 * the list as their own dep_t. Returns NULL if all the sub-deps have
 * been found or the name of the first missing dep if not.
 */
char *dep_list_verify_all(dep_list_t *dlp)
{
	int n;
	char *missing = NULL;
	dep_t *dp;

	if (!dlp)
		goto out;

	for (dp = dlp->first; dp; dp = dp->next) {
		for (n = 0; n < dp->ndeps; n++) {
			if (dep_list_exists_name(dlp, dp->deps[n]) != 0) {
				missing = d_string_new(dp->deps[n]);
				break;
			}
		}
	}

out:
	return missing;
}
