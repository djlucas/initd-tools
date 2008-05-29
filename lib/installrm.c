#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <glob.h>
#include "installrm.h"
#include "rcint.h"

static bool installrm_verbose = false;
static void install_level_links(const initd_list_t *ilp,
			const struct rcpair *rcp, initd_sk_t sk);
static void remove_existing_link(const initd_t *ip,
				const struct rcpair *rcp,
				initd_sk_t sk);
static void install_new_link(const initd_t *ip, const struct rcpair *rcp,
			initd_sk_t sk, int *prio);
static void set_current_priority(initd_sk_t sk, int *prio);

bool initd_installrm_links(const initd_list_t *ilp, const char *dir,
			initd_sk_t sk)
{
	bool ret = false;
	char *cwd;
	struct rcpair *cur;
	struct stat sbuf;

	if (!ilp)
		goto out;

	/* move to the init.d directory */
	cwd = get_current_dir_name();
        if (chdir(dir) != 0)
		error(1, errno, "%s", dir);

	/* See if we can can find an existing sysinit directory */
	for (cur = rcsdirs; cur->dir; cur++) {
		if (stat(cur->dir, &sbuf) != 0) {
			if (errno != ENOENT)
				error(1, errno, "%s", cur->dir);
		} else {
			/* found one */
			break;
		}
	}

	/* Fallback if no existing sysinit directory found */
	if (!cur->dir)
		cur = rcsdirs;

	/* Write the links for the sysinit directory */
	install_level_links(ilp, cur, sk);

	/* Loop over the rest of the directories */
	for (cur = rcdirs; cur->dir; cur++)
		install_level_links(ilp, cur, sk);

	/* Return to the original directory */
	if (chdir(cwd) != 0)
		error(1, errno, "%s", cwd);
	free(cwd);

	ret = true;
out:
	return ret;
}

void initd_installrm_set_verbose(bool verbose)
{
	installrm_verbose = verbose;
}

static void install_level_links(const initd_list_t *ilp,
			const struct rcpair *rcp, initd_sk_t sk)
{
	initd_t *ip;
	initd_rc_t rc = rcp->rc;
	char *dir = rcp->dir;

	/* Counters for next link (S20foo, S21bar, ...). Start at 0
	 * and count up for start links, start at 99 and count down
	 * for stop links. */
	int sk_prio = (sk == SK_START) ? 0 : 99;

#if 0
	if (installrm_verbose) {
		fprintf(stderr, "Installing %s links in %s\n",
			(sk == SK_START) ? "start" : "stop", dir);
	}
#endif

	/* Change to the rc directory */
	if (chdir(dir) != 0)
		error(1, errno, "%s", dir);

	/* First remove existing links, beginning with the head of the
	 * list for start links. */
	if (sk == SK_START) {
		for (ip = ilp->first; ip; ip = ip->next) {
			/* skip script if not active in this level */
			if (!initd_is_active(ip, rc, KEY_ASTART))
				continue;

			remove_existing_link(ip, rcp, sk);
		}
	} else {
		for (ip = ilp->last; ip; ip = ip->prev) {
			/* skip script if not active in this level */
			if (!initd_is_active(ip, rc, KEY_ASTOP))
				continue;

			remove_existing_link(ip, rcp, sk);
		}
	}

	/* Then install new links */
	if (sk == SK_START) {
		for (ip = ilp->first; ip; ip = ip->next)
			install_new_link(ip, rcp, sk, &sk_prio);
	} else {
		for (ip = ilp->last; ip; ip = ip->prev)
			install_new_link(ip, rcp, sk, &sk_prio);
	}
}

static void remove_existing_link(const initd_t *ip,
				const struct rcpair *rcp,
				initd_sk_t sk)
{
	initd_key_t key;
	strarg_t *links;
	int n, nlinks;
	initd_rc_t rc = rcp->rc;
	char *rdir = rcp->dir;

	if (sk == SK_START) {
		key = KEY_ASTART;
		links = ip->astart_links;
	} else {
		key = KEY_ASTOP;
		links = ip->astop_links;
	}

	/* See if any of the links exist in this directory */
	nlinks = strarg_get_num(links);
	for (n = 0; n < nlinks; n++) {
		char *cur = strarg_get_str(links, n);
		if (strncmp(cur, rdir, strlen(rdir)) == 0) {
			/* Remove it, stripping the leading dir + /.
			 * Bomb for any error besides nonexistent file */
			char *path = cur + strlen(rdir) + 1;
			if (installrm_verbose) {
				char *bdir = strrchr(rdir, '/') + 1;
				fprintf(stderr, "Removing link %s in %s\n",
					path, bdir);
			}
			if (remove(path) != 0) {
				if (errno != ENOENT)
					error(1, errno, "%s", path);
			}

			/* Clear the active bit for this level */
			initd_clear_rc(ip, key, rc);
		}
	}
}

static void install_new_link(const initd_t *ip, const struct rcpair *rcp,
			initd_sk_t sk, int *prio)
{
	char target[PATH_MAX];
	char path[PATH_MAX];
	char *tname;
	char skc;
	initd_rc_t rc;
	initd_key_t akey, ckey, dkey;

	if (!ip || !rcp)
		return;

	rc = rcp->rc;
	if (sk == SK_START) {
		akey = KEY_ASTART;
		ckey = KEY_CSTART;
		dkey = KEY_DSTART;
	} else {
		akey = KEY_ASTOP;
		ckey = KEY_CSTOP;
		dkey = KEY_DSTOP;
	}

	/* Check if this service should be run in this level */
	if (!initd_is_active(ip, rc, dkey))
		return;

	/* Check if this service is to be removed from this level */
	if (!initd_is_active(ip, rc, ckey))
		return;

	/* Set the current priority */
	set_current_priority(sk, prio);

	/* Set the name for the link [SK][0-9][0-9]name */
	tname = ip->name;
	skc = (sk == SK_START) ? 'S' : 'K';
	snprintf(path, PATH_MAX, "%c%02d%s", skc, *prio, tname);

	/* Construct the target */
	snprintf(target, PATH_MAX, "../init.d/%s", tname);

	/* Create the symlink */
	if (installrm_verbose) {
		char *bdir = strrchr(rcp->dir, '/') + 1;
		fprintf(stderr, "Creating link %s -> %s in %s\n",
			path, target, bdir);
	}
	if (symlink(target, path) < 0)
		error(1, errno, "%s -> %s", path, target);

	/* Set the active bit for this level */
	initd_set_rc(ip, akey, rc);
}

#define PRIOPATLEN 17

static void set_current_priority(initd_sk_t sk, int *prio)
{
	char pat[PRIOPATLEN];
	char skc;
	glob_t globbuf;

	/* We can only handle priorities from 0-99 */
	if (sk == SK_START) {
		if (*prio >= 99)
			return;
	} else {
		if (*prio <= 0)
			return;
	}

	do {
		/* Setup a glob pattern ([SK][0-9][0-9][![:digit:]]*) */
		skc = (sk == SK_START) ? 'S' : 'K';
		snprintf(pat, PRIOPATLEN, "%c%02d[![:digit:]]*",
			skc, *prio);

		/* Try to glob it and break if none found */
		glob(pat, 0, NULL, &globbuf);
		if (!globbuf.gl_pathc)
			break;
	} while (((sk == SK_START) && (++(*prio) < 99)) ||
		((sk == SK_STOP) && (--(*prio) > 0)));
}
