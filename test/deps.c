#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include "initd.h"
#include "str.h"

static bool verbose = true;
static dep_t *process_deps(initd_list_t *pool, const char **needed);
static void msg_fill(FILE *fd, int num, const char *format, ...);

int main(int argc, char *argv[])
{
	initd_list_t *all;
	initd_t *a, *b, *c, *d;
	dep_t *deplist;
	int n;

	a = initd_new("a");
	initd_add_prov(a, "a");
	initd_add_rstart(a, "b");
	initd_add_rstop(a, "b");
	initd_add_rstart(a, "d");
	initd_add_rstop(a, "d");

	b = initd_new("b");
	initd_add_prov(b, "b");
	initd_add_rstart(b, "c");
	initd_add_rstop(b, "c");

	c = initd_new("c");
	initd_add_prov(c, "c");

	d = initd_new("d");
	initd_add_prov(d, "d");
	initd_add_rstart(d, "c");
	initd_add_rstop(d, "c");

	all = initd_list_new();
	initd_list_add(all, a);
	initd_list_add(all, b);
	initd_list_add(all, c);
	initd_list_add(all, d);

	char *need[] = { "a", NULL };

	deplist = process_deps(all, (const char **) need);

	if (deplist) {
		printf("%d deps\n", deplist->ndep);
		printf("Ordered:");
		for (n = 0; n < deplist->ndep; n++)
			printf(" %s", deplist->dep[n]);
		printf("\n");
	} else {
		printf("Deplist is empty\n");
	}

	return 0;
}

static dep_t *process_deps(initd_list_t *pool, const char **needed)
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

	for (n = 0; needed[n]; n++) {
		/* find the initd in pool matching this name */
		cur = initd_list_find_name(pool, needed[n]);
		if (!cur) {
			fprintf(stderr, "No init script named %s\n", needed[n]);
			goto out;
		}

		if (verbose)
			msg_fill(stderr, level, "Checking %s\n", cur->name);

		/* if this dep is already in all_deps, continue */
		if (dep_exists(all_deps, cur->name)) {
			if (verbose)
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
		process_deps(pool, (const char **) cur->rstart->dep);
		parent = cur->name;
		level--;

		/* If we got here, all the subdeps have been processed.
		 * Add this dep to all_deps and remove it from
		 * chain_deps. */
		if (verbose)
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
