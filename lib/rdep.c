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
static bool _recurse_deps(initd_list_t *pool, initd_sk_t sk,
			const dep_t *needed, initd_list_t *all_deps,
			initd_list_t *chain_deps, bool optional,
			const char *parent);
static void msg_fill(FILE *fd, int num, const char *format, ...);

initd_list_t *initd_recurse_deps(initd_list_t *pool, initd_sk_t sk,
			const dep_t *needed)
{
	initd_list_t *all = NULL, *chain;
	bool success = false;

	if (!pool || !needed)
		goto out;

	/* initialize the initd lists */
	all = initd_list_new();
	chain = initd_list_new();

	/* FIXME: If there are other initd's in the pool that should
	 * start or should stop before any of the needed deps, they
	 * should also be added to the list. */

	/* recurse over needed */
	success = _recurse_deps(pool, sk, needed, all, chain, false, NULL);

	initd_list_free(chain);
out:
	if (success) {
		return all;
	} else {
		initd_list_free(all);
		return NULL;
	}
}

void initd_recurse_set_verbose(bool verbose)
{
	rdep_verbose = verbose;
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
		cip = initd_list_find_name(pool, cstr);
		if (!cip) {
			/* Don't error if the caller said this was an
			 * optional dependency. */
			if (!optional) {
				fprintf(stderr,
					"No init script named %s\n",
					cstr);
				success = false;
			}
			goto out;
		}

		if (rdep_verbose)
			msg_fill(stderr, level, "Checking %s\n", cip->name);

		/* if this initd is already in all_deps, continue */
		if (initd_list_exists_name(all_deps, cip->name)) {
			if (rdep_verbose)
				msg_fill(stderr, level, "Pruning %s\n",
					cip->name);
			continue;
		}

		/* if this dep is in chain_deps, we have a circular
		 * dependency */
		if (initd_list_exists_name(chain_deps, cip->name)) {
			fprintf(stderr,
				"Error: circular dependency %s -> %s\n",
				parent ? parent : "", cip->name);
			success = false;
			goto out;
		}

		/* add it to the chain */
		initd_list_add(chain_deps, initd_copy(cip));

		/* process its required dependencies */
		level++;
		switch (sk) {
		case RC_START:
			success = _recurse_deps(pool, sk, cip->rstart,
						all_deps, chain_deps,
						false, cip->name);
			break;
		case RC_STOP:
			success = _recurse_deps(pool, sk, cip->rstop,
						all_deps, chain_deps,
						false, cip->name);
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
		case RC_START:
			success = _recurse_deps(pool, sk, cip->sstart,
						all_deps, chain_deps,
						true, cip->name);
			break;
		case RC_STOP:
			success = _recurse_deps(pool, sk, cip->sstop,
						all_deps, chain_deps,
						true, cip->name);
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
		if (rdep_verbose)
			msg_fill(stderr, level, "Adding %s\n", cip->name);
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
