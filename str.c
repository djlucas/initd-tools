#define _GNU_SOURCE

#include <string.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "str.h"

static void d_string_grow(char *ds, size_t len);

char *d_string_new(const char *init)
{
	char *new;

	new = strdup(init);
	if (!new)
		error(2, errno, "%s", __FUNCTION__);

	return new;
}

void d_string_free(char *str) {
	free(str);
	str = NULL;
}

static void d_string_grow(char *ds, size_t len) {
	char *newbuf;

	if (len == 0)
		return;

	if ((newbuf = realloc(ds, strlen(ds) + len + 1)))
		ds = newbuf;
	else
		error(2, errno, "%s", __FUNCTION__);
}

void d_string_append(char *ds, const char *extra) {
	size_t extlen = strlen(extra);

	if (!ds) {
		ds = d_string_new(extra);
		goto out;
	}

	if (extlen == 0)
		goto out;

	d_string_grow(ds, extlen);
	ds = strncat(ds, extra, extlen);

out:;
}
