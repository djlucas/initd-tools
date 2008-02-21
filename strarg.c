#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include "str.h"

strarg_t *strarg_new(void)
{
	strarg_t *sa = malloc(sizeof(strarg_t));
	if (!sa)
		error(2, errno, "%s", __FUNCTION__);

	sa->nstr = 0;

	sa->str = malloc(sizeof(char *));
	if (!sa->str)
		error(2, errno, "%s", __FUNCTION__);
	*sa->str = NULL;

	return sa;
}

void strarg_free(strarg_t *sa)
{
	int n;

	if (!sa)
		return;

	for (n = 0; n < sa->nstr; n++)
		d_string_free(sa->str[n]);

	free(sa);
	sa = NULL;
}

void strarg_add(strarg_t *sa, const char *name)
{
	if (!sa)
		sa = strarg_new();

	sa->str = realloc(sa->str, (sa->nstr + 2) * sizeof(char *));
	if (!sa->str)
		error(2, errno, "%s", __FUNCTION__);

	sa->str[sa->nstr++] = d_string_new(name);
	sa->str[sa->nstr] = NULL;
}

strarg_t *strarg_copy(strarg_t *source)
{
	int n;
	strarg_t *tgt = strarg_new();

	if (!source)
		goto out;

	for (n = 0; n < source->nstr; n++)
		strarg_add(tgt, source->str[n]);

out:
	return tgt;
}
