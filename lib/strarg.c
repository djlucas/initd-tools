#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include "str.h"

char **strarg_new(unsigned int *num)
{
	char **sa = malloc(sizeof(char *));
	if (!sa)
		error(2, errno, "%s", __FUNCTION__);

	*num = 0;
	*sa = NULL;

	return sa;
}

void strarg_free(char **strarg, unsigned int num)
{
	int n;

	if (!strarg)
		goto out;

	for (n = 0; n < num; n++)
		d_string_free(strarg[n]);

	free(strarg);
out:
	strarg = NULL;
}

char **strarg_add(char **strarg, unsigned int *num, const char *s)
{
	if (!strarg)
		strarg = strarg_new(num);

	strarg = realloc(strarg, (*num + 2) * sizeof(char *));
	if (!strarg)
		error(2, errno, "%s", __FUNCTION__);

	strarg[(*num)++] = d_string_new(s);
	strarg[*num] = NULL;

	return strarg;
}

/* remove the last element from the strarg */
char **strarg_pop(char **strarg, unsigned int *num)
{
	int len;

	if (!strarg || !num)
		goto out;

	len = *num;

	/* free the last (nth) element */
	d_string_free(strarg[len - 1]);

	/* resize for one less element */
	strarg = realloc(strarg, len * sizeof(char *));
	if (!strarg)
		error(2, errno, "%s", __FUNCTION__);

	strarg[--(*num)] = NULL;
out:
	if (!strarg)
		strarg = NULL;
	return strarg;
}

/* Find if a given string exists in the strarg array. */
bool strarg_exists(char **strarg, unsigned int num, const char *s)
{
	int n;
	bool found = false;

	if(!strarg)
		goto out;

	for (n = 0; n < num; n++) {
		if (strcmp(s, strarg[n]) == 0) {
			found = true;
			break;
		}
	}

out:
	return found;
}
