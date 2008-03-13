#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "initd.h"
#include "str.h"

int main(int argc, char *argv[])
{
	initd_t *t;
	char *tpath;

	if (setenv("INITD_DIR", ".", 0) < 0) {
		printf("Error setting the INITD_DIR envvar\n");
		exit(1);
	}

	if (argc > 1) {
		tpath = argv[1];
	} else {
		fprintf(stderr, "No script supplied to tparse\n");
		exit(1);
	}

	printf("Opening script %s\n", tpath);
	t = initd_new(tpath);
	initd_parse(t);

	return 0;
}
