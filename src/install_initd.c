#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <getopt.h>
#include "initd.h"
#include "util.h"

#ifndef _GNU_SOURCE
char *program_invocation_name = NULL;
char *program_invocation_short_name = NULL;
#endif
#define PROGNAME "install_initd"

static struct option long_opts[] = {
	{"directory", 1, NULL, 'd'},
	{"verbose", 0, NULL, 'v'},
	{"help", 0, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

static void usage(FILE *stream);

int main(int argc, char *argv[])
{
	char *id_dir = NULL;
	initd_list_t *all, *startlist, *stoplist;
	dep_t *need = dep_new();

	/* parse arguments */
	while (1) {
		int optc, index = 0;
		optc = getopt_long(argc, argv, "d:vh", long_opts, &index);

		if (optc == -1)
			break;

		switch (optc) {
		case 'd':
			id_dir = optarg;
			break;
		case 'h':
			usage(stdout);
			exit(EXIT_SUCCESS);
		case 'v':
			set_verbose(true);
			break;
		case '?':
			error(EXIT_FAILURE, 0,
				"Unrecognized option '%c'\n"
				"See `%s --help' for usage",
				optopt, program_invocation_name);
		}
	}

	/* At least one service must be provided */
	if (optind >= argc)
		error(EXIT_FAILURE, 0, "No services supplied\n"
			"See `%s --help' for usage",
			program_invocation_name);

	/* Process the services */
	while (optind < argc)
		dep_add(need, argv[optind++]);

	/* Set the initd directory from the arguments */
	if (!set_initd_dir(&id_dir, need))
		exit(EXIT_FAILURE);

	all = initd_list_from_dir(id_dir);

	startlist = initd_add_recurse_deps(all, SK_START, need);
	if (!startlist)
		exit(EXIT_FAILURE);

	stoplist = initd_add_recurse_deps(all, SK_STOP, need);
	if (!stoplist)
		exit(EXIT_FAILURE);

	if (!initd_installrm_links(startlist, id_dir, SK_START))
		exit(EXIT_FAILURE);
	if (!initd_installrm_links(stoplist, id_dir, SK_STOP))
		exit(EXIT_FAILURE);

	return 0;
}

static void usage(FILE *stream)
{
	FILE *out;

	if (!stream)
		out = stdout;
	if (!program_invocation_name)
		program_invocation_name = PROGNAME;

	fprintf(stderr,
		"Usage: %s [OPTION] [SERVICE]...\n"
		"Install initd SERVICES(s).\n"
		"\n"
		"  -d, --directory      set the service directory\n"
		"  -v, --verbose        show the actions being performed\n"
		"  -h, --help           show this text and exit\n"
		"\n"
		"When an absolute path to SERVICE is not given, it is assumed\n"
		"to be relative to the default service directory, %s.\n",
		program_invocation_name,
		DEF_INITD_DIR);
}
