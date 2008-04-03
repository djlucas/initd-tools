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

static bool rdep_verbose = true;
static void msg_fill(FILE *fd, int num, const char *format, ...);

dep_t *initd_recurse_deps(initd_list_t *pool, initd_sk_t sk,
			const dep_t *needed)
{
	int n;
	initd_t *cur;
	static dep_t *all_deps = NULL;
	static dep_t *chain_deps = NULL;
	static char *parent = NULL;
	static int level = 0;

	if (!pool || !needed)
		goto out;

	/* initialize the dep lists if needed */
	if (!all_deps)
		all_deps = dep_new();
	if (!chain_deps)
		chain_deps = dep_new();

	for (n = 0; n < dep_get_num((dep_t *)needed); n++) {
		/* find the initd in pool matching this name */
		char *cdep = dep_get_dep((dep_t *)needed, n);
		cur = initd_list_find_name(pool, cdep);
		if (!cur) {
			fprintf(stderr, "No init script named %s\n", cdep);
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
			dep_free(all_deps);
			goto out;
		}

		/* add it to the chain */
		dep_add(chain_deps, cur->name);

		/* process its dependencies */
		level++;
		parent = cur->name;
		switch (sk) {
		case RC_START:
			initd_recurse_deps(pool, sk, cur->rstart);
			break;
		case RC_STOP:
			initd_recurse_deps(pool, sk, cur->rstop);
			break;
		default:
			fprintf(stderr, "Invalid RC start/stop %d\n", sk);
		}
		parent = cur->name;
		level--;

		/* If we got here, all the subdeps have been processed.
		 * Add this dep to all_deps and remove it from
		 * chain_deps. */
		if (rdep_verbose)
			msg_fill(stderr, level, "Adding %s\n", cur->name);
		dep_add(all_deps, cur->name);
		dep_pop(chain_deps);
	}

out:
	if (level == 0)
		dep_free(chain_deps);
	return all_deps;
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
