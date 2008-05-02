#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include "initd.h"
#include "str.h"

struct rcpair {
	char *dir;
	initd_rc_t rc;
};

static bool active_verbose = false;
static bool read_dir_symlinks(initd_list_t *ilp, const struct rcpair *rcp);
static char *rel_readlink(const char *path);
static void set_active_from_symlink(initd_list_t *ilp,
					const struct rcpair *rcp,
					const char *link, const char *tgt);

static struct rcpair rcsdirs[] = {
	{ "../rcS.d", RC_S },
	{ "../rcs.d", RC_S },
	{ "../rcSI.d", RC_S },
	{ "../rcsi.d", RC_S },
	{ "../rcSYSINIT.d", RC_S },
	{ "../rcsysinit.d", RC_S },
	{ NULL, 0 }
};

static struct rcpair rcdirs[] = {
	{ "../rc0.d", RC_0 },
	{ "../rc1.d", RC_1 },
	{ "../rc2.d", RC_2 },
	{ "../rc3.d", RC_3 },
	{ "../rc4.d", RC_4 },
	{ "../rc5.d", RC_5 },
	{ "../rc6.d", RC_6 },
	{ NULL, 0 }
};

void initd_list_set_actives(initd_list_t *ilp, const char *dir)
{
	char *cwd;
	struct rcpair *cur;

	if (!ilp || !dir)
		return;

	/* Change to the init.d directory */
	cwd = get_current_dir_name();
	if (chdir(dir) != 0)
		error(1, errno, "%s", dir);

	/* Check the various sysinit directories, stopping after the
	 * first one. */
	for (cur = rcsdirs; cur->dir; cur++) {
		/* if a sysinit dir was found, leave the loop */
		if (read_dir_symlinks(ilp, cur))
			break;
	}

	/* Check the all the numbered init directories */
	for (cur = rcdirs; cur->dir; cur++)
		read_dir_symlinks(ilp, cur);

	/* Return to the original directory */
	if (chdir(cwd) != 0)
		error(1, errno, "%s", cwd);
	free(cwd);
}

bool initd_is_active(const initd_t *ip, initd_rc_t rc, initd_sk_t sk)
{
	if (!ip)
		return false;

	switch (sk) {
	case SK_START:
		if (ip->astart & rc)
			return true;
		else
			return false;
	case SK_STOP:
		if (ip->astop & rc)
			return true;
		else
			return false;
	default:
		return false;
	}
}

static bool read_dir_symlinks(initd_list_t *ilp, const struct rcpair *rcp)
{
	char *dir, *link;
	DIR *dfd;
	struct dirent *de;

	if (!ilp || !rcp)
		return false;

	dir = rcp->dir;
	if (!dir)
		return false;

	if (active_verbose)
		printf("Changing to %s\n", dir);

	if (chdir(dir) != 0) {
		if (errno == ENOENT) {
			/* zero errno for later calls */
			errno = 0;
			return false;
		} else {
			error(1, errno, "%s", dir);
		}
	}

	dfd = opendir(".");
	if (!dfd)
		error(1, errno, "%s", dir);

	while ((de = readdir(dfd))) {
		if (!de || !de->d_name)
			continue;

		link = rel_readlink(de->d_name);
		if (!link)
			continue;

		if (active_verbose)
			printf("%s -> %s\n", de->d_name, link);

		set_active_from_symlink(ilp, rcp, de->d_name, link);
	}

	/* if errno is set, readdir had issues */
	if (errno)
		error(1, errno, "%s", dir);

	if (closedir(dfd) != 0)
		error(1, errno, "%s", dir);

	return true;
}

static char *rel_readlink(const char *path)
{
	struct stat ls;
	static char target[PATH_MAX];
	ssize_t tlen;

	if (lstat(path, &ls) != 0)
		error(1, errno, "%s", path);

	if (!S_ISLNK(ls.st_mode)) {
		if (active_verbose) {
			fprintf(stderr, "%s: Not a symlink, skipping\n",
				path);
		}
		return NULL;
	}

	/* Null all the bytes in target */
	memset(target, '\0', sizeof(target));

	tlen = readlink(path, target, sizeof(target));
	if (tlen < 0)
		error(1, errno, "%s", path);

	/* append a null byte if target was truncated */
	if (tlen == PATH_MAX)
		target[PATH_MAX - 1] = '\0';

	if (strncmp(target, "../init.d/", strlen("../init.d/")) != 0) {
		if (active_verbose) {
			fprintf(stderr,
				"%s -> %s: Not an init.d symlink, skipping\n",
				path, target);
		}
		return NULL;
	}

	return d_string_new(target);
}

static void set_active_from_symlink(initd_list_t *ilp,
					const struct rcpair *rcp,
					const char *link, const char *tgt)
{
	const char *tbase;
	size_t idlen;
	initd_t *ip;
	initd_key_t key;

	if (!(ilp || rcp || link || tgt))
		return;

	/* strip off the leading ../init.d/ in the target */
	idlen = strlen("../init.d/");
	if (strlen(tgt) <= idlen)
		return;
	tbase = tgt + idlen;

	/* see if this name exists in the initd pool */
	ip = initd_list_find_name(ilp, tbase);
	if (!ip) {
		if (active_verbose) {
			fprintf(stderr,
				"%s does not exist in initd list\n",
				tbase);
		}
		return;
	}

	/* is this a start (S) or stop (K) link? */
	if (link[0] == 'S') {
		key = KEY_ASTART;
	} else if (link[0] == 'K') {
		key = KEY_ASTOP;
	} else {
		fprintf(stderr, "link %s is not a valid S or K link\n",
			link);
		return;
	}

	/* set this level as active in the initd_t */
	initd_set_rc(ip, key, rcp->rc);
}