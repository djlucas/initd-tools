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
			const dep_t *needed, dep_t *all_deps,
			dep_t *chain_deps, bool optional,
			const char *parent);
static void msg_fill(FILE *fd, int num, const char *format, ...);

dep_t *initd_recurse_deps(initd_list_t *pool, initd_sk_t sk,
			const dep_t *needed)
{
	dep_t *all = NULL, *chain;
	bool success = false;

	if (!pool || !needed)
		goto out;

	/* initialize the dep lists */
	all = dep_new();
	chain = dep_new();

	/* FIXME: If there are other initd's in the pool that should
	 * start or should stop before any of the needed deps, they
	 * should also be added to the list. */

	/* recurse over needed */
	success = _recurse_deps(pool, sk, needed, all, chain, false, NULL);

	dep_free(chain);
out:
	if (success) {
		return all;
	} else {
		dep_free(all);
		return NULL;
	}
}

void initd_recurse_set_verbose(bool verbose)
{
	rdep_verbose = verbose;
}

static bool _recurse_deps(initd_list_t *pool, initd_sk_t sk,
			const dep_t *needed, dep_t *all_deps,
			dep_t *chain_deps, bool optional,
			const char *parent)
{
	int n;
	initd_t *cur;
	bool success = true;
	static int level = 0;

	if (!(pool || needed || all_deps || chain_deps)) {
		success = false;
		goto out;
	}

	for (n = 0; n < dep_get_num((dep_t *)needed); n++) {
		/* find the initd in pool matching this name */
		char *cdep = dep_get_dep((dep_t *)needed, n);
		cur = initd_list_find_name(pool, cdep);
		if (!cur) {
			/* Don't error if the caller said this was an
			 * optional dependency. */
			if (!optional) {
				fprintf(stderr,
					"No init script named %s\n",
					cdep);
				success = false;
			}
			goto out;
		}

		if (rdep_verbose)
			msg_fill(stderr, level, "Checking %s\n", cur->name);

		/* if this dep is already in all_deps, continue */
		if (dep_exists(all_deps, cur->name)) {
			if (rdep_verbose)
				msg_fill(stderr, level, "Pruning %s\n",
					cur->name);
			continue;
		}

		/* if this dep is in chain_deps, we have a circular
		 * dependency */
		if (dep_exists(chain_deps, cur->name)) {
			fprintf(stderr,
				"Error: circular dependency %s -> %s\n",
				parent ? parent : "", cur->name);
			success = false;
			goto out;
		}

		/* add it to the chain */
		dep_add(chain_deps, cur->name);

		/* process its required dependencies */
		level++;
		switch (sk) {
		case RC_START:
			success = _recurse_deps(pool, sk, cur->rstart,
						all_deps, chain_deps,
						false, cur->name);
			break;
		case RC_STOP:
			success = _recurse_deps(pool, sk, cur->rstop,
						all_deps, chain_deps,
						false, cur->name);
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
			success = _recurse_deps(pool, sk, cur->sstart,
						all_deps, chain_deps,
						true, cur->name);
			break;
		case RC_STOP:
			success = _recurse_deps(pool, sk, cur->sstop,
						all_deps, chain_deps,
						true, cur->name);
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
			msg_fill(stderr, level, "Adding %s\n", cur->name);
		dep_add(all_deps, cur->name);
		dep_pop(chain_deps);
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
