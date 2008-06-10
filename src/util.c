#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <string.h>
#include <error.h>
#include "initd.h"
#include "util.h"

static char *get_arg_dir(const char *path);

/* Provided a list of arguments, find the initd directory from their
 * directory names. If the arguments specify a directory differning from
 * the currently set directory, return false. If the directory is NULL
 * after all arguments have been processed, set to DEF_INITD_DIR */
bool set_initd_dir(char **initd_dir, const strarg_t *args)
{
	int n;
	char *srv, *srvdir;

	if (!initd_dir) {
		error(0, 0, "No string address passed to %s", __FUNCTION__);
		return false;
	}

	for (n = 0; n < strarg_get_num(args); n++) {
		srv = strarg_get_str(args, n);
		srvdir = get_arg_dir(srv);

		if (!srvdir)
			continue;

		/* See if it conflicts with a previous setting */
		if (*initd_dir && (strcmp(srvdir, *initd_dir) != 0)) {
			error(0, 0, "Directory %s conflicts with "
				"previous setting %s",
				srvdir, *initd_dir);
			return false;
		}

		*initd_dir = srvdir;
	}

	/* If still NULL, set to DEF_INITD_DIR */
	if (!*initd_dir)
		*initd_dir = DEF_INITD_DIR;

	return true;
}

/* Given a path, return the directory name or NULL if there is no
 * directory component. This slightly alters the behavior of dirname() */
static char *get_arg_dir(const char *path)
{
	/* return NULL for empty string */
	if ((!path) || (*path == '\0'))
		return NULL;

	/* return NULL if there is no directory component */
	if (!strchr(path, '/'))
		return NULL;

	/* otherwise, use dirname */
	return initd_dirname(path);
}

/* Turn on verbose messages in the various initd functions */
void set_verbose(bool verbose)
{
	initd_recurse_set_verbose(verbose);
	initd_installrm_set_verbose(verbose);
}
