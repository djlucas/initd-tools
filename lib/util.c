#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <error.h>
#include <errno.h>
#include "initd-util.h"

/* Wrappers around basename() and dirname() */
char *initd_basename(const char *path)
{
	char *tbase, *base;

	if (path) {
		tbase = strdup(path);
		if (!tbase)
			error(EXIT_FAILURE, errno, "strdup");
	} else {
		tbase = NULL;
	}

	base = strdup(basename(tbase));
	if (!base)
		error(EXIT_FAILURE, errno, "strdup");

	free(tbase);
	return base;
}

char *initd_dirname(const char *path)
{
	char *tdir, *dir;

	if (path) {
		tdir = strdup(path);
		if (!tdir)
			error(EXIT_FAILURE, errno, "strdup");
	} else {
		tdir = NULL;
	}

	dir = strdup(dirname(tdir));
	if (!dir)
		error(EXIT_FAILURE, errno, "strdup");

	free(tdir);
	return dir;
}
