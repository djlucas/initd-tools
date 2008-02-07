#include "str.h"

dstring_t *d_string_new(const char *init)
{
	size_t len;
	dstring_t *new;

	new = malloc(sizeof(dstring_t));
	if (!new)
		goto out;

	new->str = strdup(init);
	new->len = strlen(new->str);
out:
	return new;
}
