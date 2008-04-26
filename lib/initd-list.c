#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#ifndef _DIRENT_HAVE_D_TYPE
#include <sys/stat.h>
#include <unistd.h>
#endif
#include <limits.h>
#include <stdio.h>
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

/* Remove the last element from an initd list */
void initd_list_pop(initd_list_t *ilp)
{
	initd_t *cur;

	if (!ilp || !ilp->last)
		return;

	if (ilp->first == ilp->last) {
		initd_free(ilp->first);
		ilp->first = ilp->last = NULL;
	} else {
		cur = ilp->last->prev;
		initd_free(cur->next);
		cur->next = NULL;
		ilp->last = cur;
	}
}

initd_list_t *initd_list_from_dir(const char *dir)
{
	initd_list_t *ilp;
	DIR *dfd;
	struct dirent *de;
	initd_t *ip;

	ilp = initd_list_new();

	if (!dir)
		goto out;

	dfd = opendir(dir);
	if (!dfd)
		error(2, errno, "%s", dir);

	while ((de = readdir(dfd))) {
		char ip_path[PATH_MAX];
		int plen;
		if (!de->d_name)
			continue;

		plen = snprintf(ip_path, PATH_MAX, "%s/%s", dir, de->d_name);
		if (plen < 0)
			error(2, errno, "%s", de->d_name);

#ifdef _DIRENT_HAVE_D_TYPE
		if ((!de->d_type) || (de->d_type != DT_REG))
			continue;
#else
		struct stat ip_buf;
		if (stat(ip_path, &ip_buf) < 0)
			error(2, errno, "%s", ip_path);
		if (!S_ISREG(ip_buf.st_mode))
			continue;
#endif /* _DIRENT_HAVE_D_TYPE */

		ip = initd_parse(ip_path);
		if (ip) {
			initd_add_implicit_prov(ip);
			initd_list_add(ilp, ip);
		}
	}

	/* if errno is set, readdir had issues */
	if (errno)
		error(2, errno, "%s", dir);

	if (closedir(dfd) != 0)
		error(2, errno, "%s", dir);
out:
	return ilp;
}

initd_list_t *initd_list_copy(const initd_list_t *source)
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

initd_t *initd_list_find_name(const initd_list_t *ilp, const char *name)
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

/* Return an initd from a list that provides the given service. Returns
 * NULL when not found. */
initd_t *initd_list_find_provides(const initd_list_t *ilp, const char *serv)
{
	initd_t *cur;

	if (!ilp)
		goto err;

	for (cur = ilp->first; cur; cur = cur->next) {
		if (initd_provides(cur, serv))
			return cur;
	}

err:
	return NULL;
}

/* Verify that a given named initd exists in the list. */
bool initd_list_exists_name(const initd_list_t *ilp, const char *name)
{
	if (initd_list_find_name(ilp, name))
		return true;
	else
		return false;
}

/* Find whether a given service is provided by any script in the list. */
bool initd_list_provides(const initd_list_t *ilp, const char *serv)
{
	if (initd_list_find_provides(ilp, serv))
		return true;
	else
		return false;
}

/* Given an initd, verify that all the Required-Start scripts exist in
 * the supplied list. */
char *initd_verify_deps(const initd_list_t *ilp, const initd_t *ip,
			initd_key_t key)
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

	for (n = 0; n < dep_get_num(type); n++) {
		if (!initd_list_provides(ilp, dep_get_dep(type, n))) {
			missing = d_string_new(dep_get_dep(type, n));
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
char *initd_list_verify_all(const initd_list_t *ilp)
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
