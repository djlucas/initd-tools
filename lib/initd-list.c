#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "initd.h"
#include "str.h"

initd_list_t *initd_list_new(void)
{
	initd_list_t *ilp = malloc(sizeof(initd_list_t));
	if (!ilp)
		error(2, errno, "%s", __FUNCTION__);

	ilp->first = NULL;
	ilp->last = NULL;

	return ilp;
}

void initd_list_free(initd_list_t *ilp)
{
	initd_t *cur, *nxt;

	/* free the members of the initd list */
	for (cur = ilp->first; cur; cur = nxt) {
		nxt = cur->next;
		initd_free(cur);
	}

	free(ilp);
	ilp = NULL;
}

void initd_list_add(initd_list_t *ilp, initd_t *ip)
{
	initd_t *cur;

	/* create a new list if necessary */
	if (!ilp)
		ilp = initd_list_new();

	/* just return if NULL was passed as initd */
	if (!ip)
		return;

	cur = ilp->last;
	if (!cur) {
		/* this is the first element */
		ilp->first = ip;
	} else {
		cur->next = ip;
		ip->prev = cur;
	}
	ilp->last = ip;
}

initd_list_t *initd_list_copy(initd_list_t *source)
{
	initd_t *iold, *inew;
	initd_list_t *dest = initd_list_new();

	if (!source)
		goto out;

	for (iold = source->first; iold; iold = iold->next) {
		inew = initd_copy(iold);
		initd_list_add(dest, inew);
	}

out:
	return dest;
}

initd_t *initd_list_find_name(initd_list_t *ilp, const char *name)
{
	initd_t *ip = NULL;

	if (!(name || ilp))
		goto out;

	for (ip = ilp->first; ip; ip = ip->next) {
		if (strcmp(ip->name, name) == 0)
			break;
	}
out:
	return ip;
}

/* Verify that a given named initd exists in the list. Returns 0 for
 * success and 1 for failure.
 */
int initd_list_exists_name(initd_list_t *ilp, const char *name)
{
	if (initd_list_find_name(ilp, name))
		return 0;
	else
		return 1;
}

/* Find whether a given service is provided by an initd. */
bool initd_provides(initd_t *ip, const char *serv)
{
	bool found = false;
	int n;

	if (!ip || !ip->prov)
		goto out;

	for (n = 0; n < ip->prov->nprov; n++) {
		if (strcmp(serv, ip->prov->prov[n]) == 0) {
			found = true;
			break;
		}
	}

out:
	return found;
}

/* Find whether a given service is provided by any script in the list. */
bool initd_list_provides(initd_list_t *ilp, const char *serv)
{
	bool found = false;
	initd_t *cur;

	if (!ilp)
		goto out;

	for (cur = ilp->first; cur; cur = cur->next) {
		if (initd_provides(cur, serv)) {
			found = true;
			break;
		}
	}

out:
	return found;
}

/* Given an initd, verify that all the Required-Start scripts exist in
 * the supplied list. */
char *initd_verify_deps(initd_list_t *ilp, initd_t *ip, initd_key_t key)
{
	dep_t *type;
	char *missing = NULL;
	int n;

	if (!ilp || !ip)
		goto out;

	switch (key) {
	case KEY_RSTART:
		type = ip->rstart;
		break;
	case KEY_RSTOP:
		type = ip->rstop;
		break;
	case KEY_SSTART:
		type = ip->sstart;
		break;
	case KEY_SSTOP:
		type = ip->sstop;
		break;
	default:
		/* Not a valid key for this function */
		goto out;
	}

	if (!type)
		goto out;

	for (n = 0; n < type->ndep; n++) {
		if (!initd_list_provides(ilp, type->dep[n])) {
			missing = d_string_new(type->dep[n]);
			break;
		}
	}

out:
	return missing;
}

/* Ensure that the dependencies for each initd in the list exist in the
 * list as their own initd. Returns NULL if all the deps have been found
 * or the name of the first missing dep if not.
 */
char *initd_list_verify_all(initd_list_t *ilp)
{
	char *missing = NULL;
	initd_t *cur;

	if (!ilp)
		goto out;

	for (cur = ilp->first; cur; cur = cur->next) {
		missing = initd_verify_deps(ilp, cur, KEY_RSTART);
		if (missing)
			break;
		missing = initd_verify_deps(ilp, cur, KEY_RSTOP);
		if (missing)
			break;
	}

out:
	return missing;
}
