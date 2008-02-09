#include <stdio.h>
#include "str.h"

int main(int argc, char *argv[])
{
	dstring_t *foo = d_string_new("hello world");
	if (!foo)
		return 1;

	printf("String '%s' has '%d' characters\n", foo->str, foo->len);
	return 0;
}
