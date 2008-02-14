#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include "dep.h"
#include "str.h"

static void dep_list_grow(dep_list_t *dlp, unsigned int num);

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
		dp = dep_list_new();

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
