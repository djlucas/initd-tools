#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <ctype.h>
#include "initd.h"

static char *initd_get_initddir(void);
static FILE *initd_open(initd_t *ip);
static void initd_close(FILE *ifd);
static initd_key_t initd_parse_line(initd_t *ip, const char *line,
					initd_key_t prev);
static initd_key_t initd_string_to_key(const char *s);

/* Parse an initd script. Returns 0 on success. */
int initd_parse(initd_t *ip)
{
	char line[INITD_LINE_MAX];
	size_t nchar;
	int in_header;
	initd_key_t key;
	FILE *ifd = initd_open(ip);

	in_header = 0;
	key = KEY_NONE;
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
			key = initd_parse_line(ip, line, key);
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

static initd_key_t initd_parse_line(initd_t *ip, const char *line,
					initd_key_t prev)
{
	initd_key_t key = KEY_NONE;
	char *pos, *tmp;
	char *kstring = NULL, *value = NULL;
	int n;

	if (!line)
		goto out;
	pos = (char *)line;

	/* valid lines begin with # */
	if (*pos != '#') {
		/* blank lines are not an error */
		if (*pos != '\0')
			key = KEY_ERR;
		goto out;
	}
	pos++;

	/* If the next character is a TAB, this must be a continued
	 * description. Otherwise, it's an error. */
	if (*pos == '\t') {
		/* check if the previous key was a description */
		if (prev == KEY_DESC) {
			pos++;
			value = d_string_new(pos);
			key = KEY_DESC;
			goto print;
		} else {
			key = KEY_ERR;
			goto out;
		}
	}

	/* Otherwise, the next character must be a space or this line
	 * line in invalid. */
	if (*pos != ' ') {
		key = KEY_ERR;
		goto out;
	}
	pos++;

	/* If the # is followed by two spaces, then we need to check if
	 * this is an extended description. */
	if (prev == KEY_DESC && *pos == ' ') {
		pos++;
		value = d_string_new(pos);
		key = KEY_DESC;
		goto print;
	}

	/* The special case of the description field has now been
	 * handled and we can move on to finding a new key */
	while (isspace(*pos))
		pos++;

	/* Did we reach the end? */
	if (*pos == '\0')
		goto out;

	/* Store the beginning of the key */
	tmp = pos;

	/* Move until ':'. If we reach the end, it's an error */
	while (1) {
		pos++;
		if (*pos == ':')
			break;
		if (*pos == '\0')
			goto out;
	}

	/* Store the key string */
	kstring = strndup(tmp, pos - tmp);
	if (!kstring)
		error(2, errno, "%s", __FUNCTION__);

	/* Strip any trailing spaces from the key string */
	for (n = strlen(kstring) - 1; n >= 0; n--) {
		if (isspace(kstring[n]))
			kstring[n] = '\0';
		else
			break;
	}

	/* Convert the key string to an initd_key_t */
	key = initd_string_to_key(kstring);
	if (key == KEY_NONE || key == KEY_ERR)
		goto out;

	/* Move past the ':' and find the beginning of the value */
	pos++;
	while (isspace(*pos))
		pos++;

	/* store to the end */
	value = d_string_new(pos);

	/* Strip any trailing spaces from the value string */
	for (n = strlen(value) - 1; n >= 0; n--) {
		if (isspace(value[n]))
			value[n] = '\0';
		else
			break;
	}

print:
	if (!kstring)
		kstring = "Description";
	if (!value)
		value = "";
	printf("key type = %d, key = '%s', value = '%s'\n", key,
		kstring, value);
out:
	return key;
}

static initd_key_t initd_string_to_key(const char *s)
{
	initd_key_t key = KEY_NONE;

	if (!s)
		goto out;

	/* Walk through all the matches */
	if (strcmp(s, "Provides") == 0)
		key = KEY_PROV;
	else if (strcmp(s, "Default-Start") == 0)
		key = KEY_DSTART;
	else if (strcmp(s, "Default-Stop") == 0)
		key = KEY_DSTOP;
	else if (strcmp(s, "Required-Start") == 0)
		key = KEY_RSTART;
	else if (strcmp(s, "Required-Stop") == 0)
		key = KEY_RSTOP;
	else if (strcmp(s, "Should-Start") == 0)
		key = KEY_SSTART;
	else if (strcmp(s, "Should-Stop") == 0)
		key = KEY_SSTOP;
	else if (strcmp(s, "Short-Description") == 0)
		key = KEY_SDESC;
	else if (strcmp(s, "Description") == 0)
		key = KEY_DESC;
	else if (strncmp(s, "X-", 2) == 0)
		key = KEY_EXT;
	else
		key = KEY_ERR;
out:
	return key;
}
