#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include "types.h"
#include "rdep.h"

static bool rdep_verbose = false;
static dep_t *add_all_active(const initd_list_t *pool,
			const dep_t *init, initd_sk_t sk);
static bool _recurse_deps(initd_list_t *pool, initd_sk_t sk,
			const dep_t *needed, initd_list_t *all_deps,
			initd_list_t *chain_deps, bool optional,
			const char *parent);
static void msg_fill(FILE *fd, int num, const char *format, ...);
static bool initd_list_verify_level(const initd_list_t *ord,
				initd_rc_t rc, initd_sk_t sk,
				bool required);

/* Given a list of services to add, recurse through their Required and
 * Should fields to determine an ordered list. NULL is returned on
 * errors. */
initd_list_t *initd_add_recurse_deps(initd_list_t *pool, initd_sk_t sk,
			const dep_t *add)
{
	initd_list_t *all = NULL, *chain = NULL;
	dep_t *all_needed = NULL;

	if (!pool)
		goto out;

	/* initialize the returned initd list */
	all = initd_list_new();

	/* just return an empty list if no deps were passed */
	if (!add || !dep_get_num(add))
		goto out;

	/* initialize the chain list */
	chain = initd_list_new();

	/* Add all the currently active services to the needed list so
	 * they are properly reordered. */
	all_needed = add_all_active(pool, add, sk);

	/* recurse over needed */
	if (!_recurse_deps(pool, sk, all_needed, all, chain, false, NULL))
		goto err;

	if (initd_list_verify_deps(all, sk))
		goto out;
	else
		fprintf(stderr, "Failed to verify %s scripts\n",
			(sk == SK_START) ? "start" : "stop");

err:
	initd_list_free(all);
	all = NULL;
out:
	initd_list_free(chain);
	dep_free(all_needed);
	return all;
}

/* Given a list of services to remove, recurse through the Required and
 * Should fields of the other active services to determine a valid,
 * ordered list. Returns NULL on error. */
initd_list_t *initd_remove_recurse_deps(initd_list_t *pool,
				initd_sk_t sk, const dep_t *remove)
{
	initd_list_t *save = NULL, *all = NULL, *chain = NULL;
	initd_t *orig, *copy;
	int n;
	dep_t *all_active = NULL;
	char *rm;

	if (!pool)
		goto out;

	/* initialize the returned initd list */
	all = initd_list_new();

	/* just return an empty list if no deps were passed */
	if (!remove || !dep_get_num(remove))
		goto out;

	/* initialize the initd save list */
	save = initd_list_new();

	/* Check that the specified services to remove are in the pool.
	 * If they are, add them to a saved list so we can clear their
	 * active field and then restore it afterwards. */
	for (n = 0; n < dep_get_num(remove); n++) {
		rm = dep_get_dep(remove, n);
		orig = initd_list_find_provides(pool, rm);
		if (!orig) {
			fprintf(stderr, "No init script provides %s\n",
				rm);
			goto err;
		}
		if (initd_is_active(orig, RC_ALL, sk)) {
			copy = initd_copy(orig);
			initd_list_add(save, copy);
			orig->astart = orig->astop = 0;
		}
	}

	/* If none of the specified services are currently active, the
	 * save list will be empty and we can just return. */
	if (!save->first)
		goto out;

	/* Find all the currently active services */
	all_active = add_all_active(pool, NULL, sk);

	/* initialize the chain recursion lists */
	chain = initd_list_new();

	/* recurse over needed */
	if (!_recurse_deps(pool, sk, all_active, all, chain, false, NULL))
		goto err;

	if (!initd_list_verify_deps(all, sk)) {
		fprintf(stderr, "Failed to verify %s scripts\n",
			(sk == SK_START) ? "start" : "stop");
		goto err;
	}

	/* Restore the saved active levels */
	for (copy = save->first; copy; copy = copy->next) {
		orig = initd_list_find_name(pool, copy->name);
		if (orig) {
			orig->astart = copy->astart;
			orig->astop = copy->astop;
		} else {
			fprintf(stderr, "Saved script %s not in "
				"original pool\n", copy->name);
		}
	}

	/* If we got here, we're successful */
	goto out;
err:
	initd_list_free(all);
	all = NULL;
out:
	initd_list_free(chain);
	dep_free(all_active);
	initd_list_free(save);
	return all;
}

void initd_recurse_set_verbose(bool verbose)
{
	rdep_verbose = verbose;
}

/* Given an ordered initd list (such as determined in initd_*_recurse_deps)
 * determine if the start and stop order is valid in each run level. */
bool initd_list_verify_deps(const initd_list_t *ord, initd_sk_t sk)
{
	initd_rc_t rc;
	bool ret = true;

	if (!ord)
		return false;

	for (rc = RC_S; rc <= RC_6; rc = rc << 1) {
		if (!initd_list_verify_level(ord, rc, sk, true)) {
			fprintf(stderr, "Failed to verify the required "
				"%s scripts for level %c\n",
				(sk == SK_START) ? "start" : "stop",
				initd_rc_level_char(rc));
			ret = false;
			break;
		}
		if (!initd_list_verify_level(ord, rc, sk, false)) {
			fprintf(stderr, "Failed to verify the optional "
				"%s scripts for level %c\n",
				(sk == SK_START) ? "start" : "stop",
				initd_rc_level_char(rc));
			ret = false;
			break;
		}
	}

	return ret;
}

static dep_t *add_all_active(const initd_list_t *pool,
			const dep_t *init, initd_sk_t sk)
{
	dep_t *active;
	initd_t *ip;

	if (!pool)
		return NULL;

	active = dep_copy(init);
	for (ip = pool->first; ip; ip = ip->next) {
		if (initd_is_active(ip, RC_ALL, sk))
			dep_add(active, ip->name);
	}

	return active;
}

static bool _recurse_deps(initd_list_t *pool, initd_sk_t sk,
			const dep_t *needed, initd_list_t *all_deps,
			initd_list_t *chain_deps, bool optional,
			const char *parent)
{
	int n;
	char *cstr;
	initd_t *cip;
	bool success = true;
	static int level = 0;

	if (!(pool || needed || all_deps || chain_deps)) {
		success = false;
		goto out;
	}

	for (n = 0; n < dep_get_num(needed); n++) {
		/* find the initd in pool matching this name */
		cstr = dep_get_dep(needed, n);
		cip = initd_list_find_provides(pool, cstr);
		if (!cip) {
			/* Don't error if the caller said this was an
			 * optional dependency. */
			if (!optional) {
				fprintf(stderr,
					"No init script provides %s\n",
					cstr);
				success = false;
			}
			goto out;
		}

		if (rdep_verbose) {
			if (strcmp(cstr, cip->name) == 0) {
				msg_fill(stderr, level, "Checking %s\n",
					cstr);
			} else {
				msg_fill(stderr, level,
					"Checking %s (%s)\n",
					cstr, cip->name);
			}
		}

		/* if this initd is already in all_deps, continue */
		if (initd_list_exists_name(all_deps, cip->name)) {
			if (rdep_verbose) {
				if (strcmp(cstr, cip->name) == 0) {
					msg_fill(stderr, level,
						"Pruning %s\n",
						cstr);
				} else {
					msg_fill(stderr, level,
						"Pruning %s (%s)\n",
						cstr, cip->name);
				}
			}
			continue;
		}

		/* if this dep is in chain_deps, we have a circular
		 * dependency */
		if (initd_list_exists_name(chain_deps, cip->name)) {
			if (strcmp(cstr, cip->name) == 0) {
				fprintf(stderr,
					"Error: circular dependency"
					" %s -> %s\n",
					parent ? parent : "", cstr);
			} else {
				fprintf(stderr,
					"Error: circular dependency"
					" %s -> %s (%s)\n",
					parent ? parent : "", cstr,
					cip->name);
			}
			success = false;
			goto out;
		}

		/* add it to the chain */
		initd_list_add(chain_deps, initd_copy(cip));

		/* process its required dependencies */
		level++;
		switch (sk) {
		case SK_START:
			success = _recurse_deps(pool, sk, cip->rstart,
						all_deps, chain_deps,
						false, cstr);
			break;
		case SK_STOP:
			success = _recurse_deps(pool, sk, cip->rstop,
						all_deps, chain_deps,
						false, cstr);
			break;
		default:
			fprintf(stderr, "Invalid RC start/stop %d\n", sk);
		}
		level--;

		/* Break from the loop if the recursion was a failure */
		if (!success)
			goto out;

		/* process its optional dependencies */
		level++;
		switch (sk) {
		case SK_START:
			success = _recurse_deps(pool, sk, cip->sstart,
						all_deps, chain_deps,
						true, cstr);
			break;
		case SK_STOP:
			success = _recurse_deps(pool, sk, cip->sstop,
						all_deps, chain_deps,
						true, cstr);
			break;
		default:
			fprintf(stderr, "Invalid RC start/stop %d\n", sk);
		}
		level--;

		/* Break from the loop if the recursion was a failure */
		if (!success)
			goto out;

		/* If we got here, all the subdeps have been processed.
		 * Add this dep to all_deps and remove it from
		 * chain_deps. */
		if (rdep_verbose) {
			if (strcmp(cstr, cip->name) == 0) {
				msg_fill(stderr, level, "Adding %s\n",
					cstr);
			} else {
				msg_fill(stderr, level,
					"Adding %s (%s)\n",
					cstr, cip->name);
			}
		}
		initd_list_add(all_deps, initd_copy(cip));
		initd_list_pop(chain_deps);
	}

out:
	return success;
}

static void msg_fill(FILE *fd, int num, const char *format, ...)
{
	char fill[LINE_MAX];
	va_list args;

	if (num > 0) {
		if (num >= LINE_MAX)
			num = LINE_MAX - 1;

		memset(fill, '.', num);
		fill[num] = '\0';
		fprintf(fd, "%s", fill);
	}

	va_start(args, format);
	vfprintf(fd, format, args);
	va_end(args);
}

static bool initd_list_verify_level(const initd_list_t *ord,
				initd_rc_t rc, initd_sk_t sk,
				bool required)
{
	initd_t *ip, *dep;
	initd_rc_t iprc;
	dep_t *iprcdep;
	char *dstr;
	int n;
	bool match;

	if (!ord)
		return false;

	/* Check the deps are valid in this level */
	for (ip = ord->first; ip; ip = ip->next) {
		if (sk == SK_START) {
			iprc = ip->dstart;
			if (required)
				iprcdep = ip->rstart;
			else
				iprcdep = ip->sstart;
		} else {
			iprc = ip->dstop;
			if (required)
				iprcdep = ip->rstop;
			else
				iprcdep = ip->sstop;
		}

		/* Skip if the script isn't active at this level */
		if (!(iprc & rc))
			continue;

		/* Check the deps */
		for (n = 0; n < dep_get_num(iprcdep); n++) {
			dstr = dep_get_dep(iprcdep, n);
			dep = initd_list_find_provides(ord, dstr);
			if (!dep) {
				if (required) {
					fprintf(stderr,
						"No init script "
						"provides %s\n",
						dstr);
					return false;
				} else {
					continue;
				}
			}

			/* Check if the dep is started in this level
			 * or if it's in a special level */
			if (sk == SK_START) {
				match = dep->dstart & rc;
				if (!match)
					match = dep->dstart & RC_S;
			} else {
				match = dep->dstop & rc;
				if (!match) {
					match = (dep->dstop & RC_0) &&
						(dep->dstop & RC_6);
				}
			}

			if (!match && sk == SK_START) {
				fprintf(stderr,
					"%s dependency %s does not "
					"start in level %c or sysinit\n",
					ip->name, dstr,
					initd_rc_level_char(rc));
				return false;
			} else if (!match) {
				fprintf(stderr,
					"%s dependency %s does not stop "
					"in level %c or halt and reboot\n",
					ip->name, dstr,
					initd_rc_level_char(rc));
				return false;
			}
		}
	}

	/* If we get here, then we were successful */
	return true;
}
