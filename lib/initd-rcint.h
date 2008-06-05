#ifndef _initd_rcint_h_
#define _initd_rcint_h_

struct rcpair {
	char *dir;
	initd_rc_t rc;
};

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

#endif /* _initd_rcint_h_ */
