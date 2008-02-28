#include <stdlib.h>
#include <stdio.h>
#include "initd.h"

int main(int argc, char *argv[])
{
	initd_t *t;

	if (setenv("INITD_DIR", ".", 0) < 0) {
		printf("Error setting the INITD_DIR envvar\n");
		exit(1);
	}

	t = initd_new("test-script");
	initd_parse(t);

	return 0;
}
