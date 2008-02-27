#include <stdio.h>
#include <string.h>
#include "str.h"

int main(int argc, char *argv[])
{
	char *foo = d_string_new("hello world");
	printf("String '%s' has '%d' characters\n", foo, strlen(foo));
	printf("foo has size '%d'\n", sizeof(*foo) * strlen(foo));

	foo = d_string_append(foo, ", goodbye");
	printf("String '%s' has '%d' characters\n", foo, strlen(foo));
	printf("foo has size '%d'\n", sizeof(*foo) * strlen(foo));

	foo = d_string_append(foo, "");
	printf("String '%s' has '%d' characters\n", foo, strlen(foo));
	printf("foo has size '%d'\n", sizeof(*foo) * strlen(foo));

	d_string_free(foo);

	foo = d_string_new(NULL);
	printf("String '%s' has '%d' characters\n", foo, strlen(foo));

	return 0;
}
