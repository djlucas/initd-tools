#ifndef _types_h_
#define _types_h_

#include "str.h"

/* keys in the initd type */
typedef enum initd_key {
	KEY_ERR = -1,  /* Invalid key or failure reading key */
	KEY_NONE,      /* Empty field */
	KEY_NAME,      /* Filename of the script */
	KEY_PROV,      /* Provides */
	KEY_ASTART,    /* Active Start levels */
	KEY_ASTOP,     /* Active Stop levels */
	KEY_DSTART,    /* Default-Start */
	KEY_DSTOP,     /* Default-Stop */
	KEY_RSTART,    /* Required-Start */
	KEY_RSTOP,     /* Required-Stop */
	KEY_SSTART,    /* Should-Start */
	KEY_SSTOP,     /* Should-Stop */
	KEY_SDESC,     /* Short-Description */
	KEY_DESC,      /* Description */
	KEY_EXT        /* X- extension */
} initd_key_t;

/* prov type */
typedef strarg_t prov_t;

#define prov_new strarg_new
#define prov_free strarg_free
#define prov_add strarg_add
#define prov_push strarg_push
#define prov_pop strarg_pop
#define prov_get_num strarg_get_num
#define prov_get_prov strarg_get_str
#define prov_exists strarg_exists
#define prov_copy strarg_copy

/* dep type */
typedef strarg_t dep_t;

#define dep_new strarg_new
#define dep_free strarg_free
#define dep_add strarg_add
#define dep_push strarg_push
#define dep_pop strarg_pop
#define dep_get_num strarg_get_num
#define dep_get_dep strarg_get_str
#define dep_exists strarg_exists
#define dep_copy strarg_copy

/* rc type */
typedef enum initd_rc {
	RC_S = (1 << 0),  /* rcS.d or its variants */
	RC_0 = (1 << 1),  /* rc0.d */
	RC_1 = (1 << 2),  /* rc1.d */
	RC_2 = (1 << 3),  /* rc2.d */
	RC_3 = (1 << 4),  /* rc3.d */
	RC_4 = (1 << 5),  /* rc4.d */
	RC_5 = (1 << 6),  /* rc5.d */
	RC_6 = (1 << 7)   /* rc6.d */
} initd_rc_t;

/* start/stop (S/K) type */
typedef enum initd_sk {
	RC_START,  /* S??foo */
	RC_STOP    /* K??foo */
} initd_sk_t;

/* The initd type */
typedef struct initd {
	char *name;

	prov_t *prov;       /* Provides */

	initd_rc_t dstart;  /* Default-Start */
	initd_rc_t dstop;   /* Default-Stop */

	initd_rc_t astart;  /* Active Start levels */
	initd_rc_t astop;   /* Active Stop levels */

	dep_t *rstart;      /* Required-Start */
	dep_t *rstop;       /* Required-Stop */

	dep_t *sstart;      /* Should-Start */
	dep_t *sstop;       /* Should-Stop */

	char *sdesc;        /* Short-Description */
	char *desc;         /* Description */

	struct initd *prev;
	struct initd *next;
} initd_t;

/* linked list of initd types */
typedef struct initd_list {
	initd_t *first;
	initd_t *last;
} initd_list_t;

#endif /* _types_h_ */
