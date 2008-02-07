#include <stdlib.h>
#include <string.h>

typedef struct dstring {
	char *str;
	size_t len;
} dstring_t;

dstring_t *d_string_new(const char *new);
