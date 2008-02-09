#include "str.h"

dstring_t *d_string_new(const char *init)
{
	size_t len;
	dstring_t *new;

	new = malloc(sizeof(dstring_t));
	if (!new) {
		perror(__FUNCTION__);
		goto out;
	}

	new->str = strdup(init);
	if (!new->str) {
		perror(__FUNCTION__);
		free(new);
		new = NULL;
		goto out;
	}

	new->len = strlen(new->str);
out:
	return new;
}
