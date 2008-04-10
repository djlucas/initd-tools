#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <ctype.h>
#include <libgen.h>
#include "initd.h"

static FILE *initd_open(const char *path);
static void initd_close(FILE *ifd);
static initd_key_t initd_parse_line(initd_t *ip, const char *line,
					initd_key_t prev);
static initd_key_t initd_string_to_key(const char *s);
static initd_rc_t initd_convert_to_rc(const char *tok);
static void initd_parse_line_tokens(initd_t *ip, const char *line,
					initd_key_t key);

/* Parse an initd script. Returns an allocated initd_t or NULL if the
   script was not valid. */
initd_t *initd_parse(const char *path)
{
	char line[INITD_LINE_MAX];
	size_t nchar;
	int in_header, is_initd;
	initd_key_t key;
	initd_t *ip = NULL;
	FILE *ifd = initd_open(path);

	ip = initd_new(basename((char *)path));

	in_header = is_initd = 0;
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
		if (in_header && strcmp(line, "### END INIT INFO") == 0) {
			is_initd = 1;
			break;
		}

		/* parse the line if we're in the header */
		if (in_header)
			key = initd_parse_line(ip, line, key);
	}

	if (ferror(ifd))
		error(2, errno, "%s", __FUNCTION__);

	initd_close(ifd);

	/* return NULL if this wasn't a valid init script */
	if (!is_initd) {
		initd_free(ip);
		ip = NULL;
	}

	return ip;
}

FILE *initd_open(const char *path)
{
	FILE *fd = NULL;

	/* return if no script name passed */
	if (!path)
		goto out;

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
			goto desc;
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
		goto desc;
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

#if 0
	if (!kstring)
		kstring = "Description";
	if (!value)
		value = "";
	printf("key type = %d, key = '%s', value = '%s'\n", key,
		kstring, value);
#endif

	switch(key) {
	case KEY_PROV:
	case KEY_DSTART:
	case KEY_DSTOP:
	case KEY_RSTART:
	case KEY_RSTOP:
	case KEY_SSTART:
	case KEY_SSTOP:
		initd_parse_line_tokens(ip, value, key);
		break;
	case KEY_SDESC:
		initd_set_sdesc(ip, value);
		break;
	case KEY_DESC:
		initd_set_desc(ip, value);
		break;
	default:
		break;
	}
	goto out;

desc:
	/* we have an extended description */
	initd_add_desc(ip, value);
out:
	d_string_free(value);
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

/* Convert a single token from Default-{Start,Stop} strings into
 * an initd_rc_t */
static initd_rc_t initd_convert_to_rc(const char *tok)
{
	initd_rc_t lev = 0;

	if (!tok)
		goto out;

	if (strcmp(tok, "si") == 0)
		lev = RC_SI;
	else if (*tok == '0')
		lev = RC_0;
	else if (*tok == '1')
		lev = RC_1;
	else if (*tok == '2')
		lev = RC_2;
	else if (*tok == '3')
		lev = RC_3;
	else if (*tok == '4')
		lev = RC_4;
	else if (*tok == '5')
		lev = RC_5;
	else if (*tok == '6')
		lev = RC_6;
out:
	return lev;
}

/* Parse tokens from a line and convert them to the key types */
static void initd_parse_line_tokens(initd_t *ip, const char *line,
					initd_key_t key)
{
	char *a, *b;

	if (!ip || !line)
		return;

	b = (char *)line;

	while (1) {
		char *tok;

		while (isspace(*b))
			b++;
		if (*b == '\0')
			break;
		a = b++;
		while (!(*b == '\0' || isspace(*b)))
			b++;

		/* reached end of line or space following token */
		tok = strndup(a, b-a);
		if (!tok)
			error(2, errno, "%s", __FUNCTION__);

		switch(key) {
		case KEY_DSTART:
		case KEY_DSTOP:
			initd_set_rc(ip, key, initd_convert_to_rc(tok));
			break;
		case KEY_PROV:
			initd_add_prov(ip, tok);
			break;
		case KEY_RSTART:
			initd_add_rstart(ip, tok);
			break;
		case KEY_RSTOP:
			initd_add_rstop(ip, tok);
			break;
		case KEY_SSTART:
			initd_add_sstart(ip, tok);
			break;
		case KEY_SSTOP:
			initd_add_sstop(ip, tok);
			break;
		default:
			break;
		}

		d_string_free(tok);

		if (*b == '\0')
			break;
	}
}
