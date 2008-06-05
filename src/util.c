#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <string.h>
#include "initd.h"
#include "util.h"

/* Given a path, return the directory name or NULL if there is no
 * directory component. This slightly alters the behavior of dirname() */
char *get_initd_dir(const char *path)
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
