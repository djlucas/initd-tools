#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <getopt.h>
#include "initd.h"
#include "types.h"
#include "rdep.h"
#include "installrm.h"

#ifndef _GNU_SOURCE
char *program_invocation_name = NULL;
char *program_invocation_short_name = NULL;
#endif
#define PROGNAME "remove_initd"

static struct option long_opts[] = {
	{"directory", 1, NULL, 'd'},
	{"verbose", 0, NULL, 'v'},
	{"help", 0, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

static void usage(FILE *stream);
static void set_verbose(void);

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
			set_verbose();
			break;
		case '?':
			error(EXIT_FAILURE, 0,
				"Unrecognized option '%c'\n"
				"See `%s --help' for usage",
				optopt, program_invocation_name);
		}
	}

	if (optind < argc) {
		while (optind < argc)
			dep_add(need, argv[optind++]);
	} else {
		error(EXIT_FAILURE, 0, "No services supplied\n"
			"See `%s --help' for usage",
			program_invocation_name);
	}

	if (!id_dir)
		id_dir = DEF_INITD_DIR;

	all = initd_list_from_dir(id_dir);

	startlist = initd_remove_recurse_deps(all, SK_START, need);
	if (!startlist)
		exit(EXIT_FAILURE);

	stoplist = initd_remove_recurse_deps(all, SK_STOP, need);
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
		"Remove initd SERVICES(s).\n"
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

static void set_verbose(void)
{
	initd_recurse_set_verbose(true);
	initd_installrm_set_verbose(true);
}
