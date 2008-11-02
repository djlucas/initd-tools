#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include "initd-str.h"

strarg_t *strarg_new(void)
{
	strarg_t *sp = malloc(sizeof(strarg_t));
	if (!sp)
		error(EXIT_FAILURE, errno, "malloc");

	sp->str = malloc(sizeof(char *));
	if (!sp)
		error(EXIT_FAILURE, errno, "malloc");

	*(sp->str) = NULL;
	sp->nstr = 0;

	return sp;
}

void strarg_free(strarg_t *sp)
{
	int n;

	if (!sp)
		return;

	for (n = 0; n < sp->nstr; n++)
		d_string_free(sp->str[n]);

	free(sp->str);
	sp->str = NULL;

	free(sp);
	sp = NULL;
}

void strarg_add(strarg_t *sp, const char *s)
{
	if (!sp)
		sp = strarg_new();

	sp->str = realloc(sp->str, (sp->nstr + 2) * sizeof(char *));
	if (!sp)
		error(EXIT_FAILURE, errno, "realloc");

	sp->str[(sp->nstr)++] = d_string_new(s);
	sp->str[sp->nstr] = NULL;
}

/* remove the last element from the strarg */
void strarg_pop(strarg_t *sp)
{
	int len;

	if (!sp)
		return;

	len = sp->nstr;

	/* free the last (nth) element */
	d_string_free(sp->str[len - 1]);

	/* resize for one less element */
	sp->str = realloc(sp->str, len * sizeof(char *));
	if (!sp->str)
		error(EXIT_FAILURE, errno, "realloc");

	sp->str[--(sp->nstr)] = NULL;
}

/* Find if a given string exists in the strarg array. */
bool strarg_exists(const strarg_t *sp, const char *s)
{
	int n;
	bool found = false;

	if(!sp)
		goto out;

	for (n = 0; n < sp->nstr; n++) {
		if (strcmp(s, sp->str[n]) == 0) {
			found = true;
			break;
		}
	}

out:
	return found;
}

unsigned int strarg_get_num(const strarg_t *sp)
{
	if (sp)
		return sp->nstr;
	else
		return 0;
}

char *strarg_get_str(const strarg_t *sp, unsigned int index)
{
	if (sp && index <= sp->nstr)
		return sp->str[index];
	else
		return NULL;
}


strarg_t *strarg_copy(const strarg_t *source)
{
	int n;
	strarg_t *dest = strarg_new();

	if (!dest)
		goto out;

	for (n = 0; n < strarg_get_num(source); n++)
		strarg_add(dest, strarg_get_str(source, n));

out:
	return dest;
}
