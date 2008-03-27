#ifndef _initd_h_
#define _initd_h_

#include <stdbool.h>
#include "dep.h"
#include "prov.h"

#ifndef DEF_INITD_DIR
#define DEF_INITD_DIR "/etc/init.d"
#endif

#ifndef INITD_LINE_MAX
#define INITD_LINE_MAX 256
#endif

typedef enum initd_key {
	KEY_ERR = -1,  /* Invalid key or failure reading key */
	KEY_NONE,      /* Empty field */
	KEY_NAME,      /* Filename of the script */
	KEY_PROV,      /* Provides */
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

typedef enum initd_rc {
	RC_SI = (1 << 0),  /* rcsysinit.d */
	RC_0  = (1 << 1),  /* rc0.d */
	RC_1  = (1 << 2),  /* rc1.d */
	RC_2  = (1 << 3),  /* rc2.d */
	RC_3  = (1 << 4),  /* rc3.d */
	RC_4  = (1 << 5),  /* rc4.d */
	RC_5  = (1 << 6),  /* rc5.d */
	RC_6  = (1 << 7)   /* rc6.d */
} initd_rc_t;

typedef struct initd {
	char *name;

	prov_t *prov;       /* Provides */

	initd_rc_t dstart;  /* Default-Start */
	initd_rc_t dstop;   /* Default-Stop */

	dep_t *rstart;      /* Required-Start */
	dep_t *rstop;       /* Required-Stop */

	dep_t *sstart;      /* Should-Start */
	dep_t *sstop;       /* Should-Stop */

	char *sdesc;        /* Short-Description */
	char *desc;         /* Description */

	struct initd *prev;
	struct initd *next;
} initd_t;

typedef struct initd_list {
	initd_t *first;
	initd_t *last;
} initd_list_t;

extern initd_t *initd_new(const char *name);
extern void initd_free(initd_t *ip);
extern initd_t *initd_copy(initd_t *source);
extern initd_t *initd_parse(const char *path);

/* Setters */
#define initd_add_prov(ip, name) prov_add(ip->prov, name)
#define initd_add_rstart(ip, name) dep_add(ip->rstart, name)
#define initd_add_rstop(ip, name) dep_add(ip->rstop, name)
#define initd_add_sstart(ip, name) dep_add(ip->sstart, name)
#define initd_add_sstop(ip, name) dep_add(ip->sstop, name)
extern void initd_set_sdesc(initd_t *ip, const char *sdesc);
extern void initd_set_desc(initd_t *ip, const char *desc);
extern void initd_add_desc(initd_t *ip, const char *ext);

/* List functions */
extern initd_list_t *initd_list_new(void);
extern void initd_list_free(initd_list_t *ilp);
extern void initd_list_add(initd_list_t *ilp, initd_t *ip);
extern initd_list_t *initd_list_from_dir(const char *dir);
extern initd_list_t *initd_list_copy(initd_list_t *source);
extern initd_t *initd_list_find_name(initd_list_t *ilp, const char *name);
extern bool initd_list_exists_name(initd_list_t *ilp, const char *name);

/* Verification */
extern bool initd_provides(initd_t *ip, const char *serv);
extern bool initd_list_provides(initd_list_t *ilp, const char *serv);
extern char *initd_verify_deps(initd_list_t *ilp, initd_t *ip,
				initd_key_t key);
extern char *initd_list_verify_all(initd_list_t *ilp);

#endif /* _initd_h_ */
