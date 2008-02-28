#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include "initd.h"

static char *initd_get_initddir(void);
static FILE *initd_open(initd_t *ip);
static void initd_close(FILE *ifd);

/* Parse an initd script. Returns 0 on success. */
int initd_parse(initd_t *ip)
{
	char line[INITD_LINE_MAX];
	size_t nchar;
	int in_header;
	FILE *ifd = initd_open(ip);

	in_header = 0;
	while (fgets(line, sizeof(line), ifd)) {
		/* replace the newline */
		nchar = strlen(line);
		if (line[nchar - 1] == '\n')
			line[nchar - 1] = '\0';

		/* check for the opening header marker */
		if (!in_header && strcmp(line, "### BEGIN INIT INFO") == 0) {
			in_header = 1;
			continue;
		}

		/* check for the closing header marker */
		if (in_header && strcmp(line, "### END INIT INFO") == 0)
			break;

		/* print the line if we're in the header */
		if (in_header)
			puts(line);
	}

	if (ferror(ifd))
		error(2, errno, "%s", __FUNCTION__);

	initd_close(ifd);
	return 0;
}

/* Where to search for scripts: check environment variable INITD_DIR and
 * fall back to DEF_INITD_DIR.
 */
char *initd_get_initddir(void) {
	char *id;

	id = getenv("INITD_DIR");
	if (!id)
		id = DEF_INITD_DIR;

	return id;
}

FILE *initd_open(initd_t *ip)
{
	FILE *fd = NULL;
	char *initd_dir, *path;
	int plen;

	/* return if no name for the initd */
	if (!ip || !ip->name)
		goto out;

	initd_dir = initd_get_initddir();

	plen = strlen(initd_dir) + strlen(ip->name) + 2;
	path = malloc(plen * sizeof(char));
	if (!path)
		error(2, errno, "%s", __FUNCTION__);
	if (sprintf(path, "%s/%s", initd_dir, ip->name) >= plen)
		error(2, errno, "%s", __FUNCTION__);

	fd = fopen(path, "r");
	if (!fd)
		error(2, errno, "%s", __FUNCTION__);

out:
	return fd;
}

static void initd_close(FILE *ifd)
{
	if (!ifd)
		return;

	if (fclose(ifd) != 0)
		error(2, errno, "%s", __FUNCTION__);
}
