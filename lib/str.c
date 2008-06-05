#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <string.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "initd-str.h"

static char *d_string_grow(char *ds, size_t len);

char *d_string_new(const char *init)
{
	char *new;

	new = strdup(init ? init : "");
	if (!new)
		error(2, errno, "%s", __FUNCTION__);

	return new;
}

void d_string_free(char *str) {
	free(str);
	str = NULL;
}

static char *d_string_grow(char *ds, size_t len) {
	if (len == 0)
		goto out;

	ds = realloc(ds, strlen(ds) + len + 1);
	if (!ds)
		error(2, errno, "%s", __FUNCTION__);

out:
	return ds;
}

char *d_string_append(char *ds, const char *extra) {
	size_t extlen = strlen(extra);

	if (!ds) {
		ds = d_string_new(extra);
		goto out;
	}

	if (extlen == 0)
		goto out;

	ds = d_string_grow(ds, extlen);
	ds = strncat(ds, extra, extlen);

out:
	return ds;
}
