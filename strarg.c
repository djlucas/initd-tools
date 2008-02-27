#include <stdlib.h>
#include <error.h>
#include <errno.h>
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
