#ifndef _initd_h_
#define _initd_h_

#include "dep.h"

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

typedef struct initd {
	char *name;
	dep_t *deps;

	struct initd *prev;
	struct initd *next;
} initd_t;

typedef struct initd_list {
	initd_t *first;
	initd_t *last;
} initd_list_t;

extern initd_t *initd_new(const char *name);
extern void initd_free(initd_t *ip);
extern void initd_add(initd_t *ip, const char *name);
extern initd_t *initd_copy(initd_t *source);
extern int initd_exists_name(initd_t *ip, const char *name);
extern int initd_parse(initd_t *ip);

extern initd_list_t *initd_list_new(void);
extern void initd_list_free(initd_list_t *ilp);
extern void initd_list_add(initd_list_t *ilp, initd_t *ip);
extern initd_list_t *initd_list_copy(initd_list_t *source);
extern initd_t *initd_list_find_name(initd_list_t *ilp, const char *name);
extern int initd_list_exists_name(initd_list_t *ilp, const char *name);
extern char *initd_list_verify_all(initd_list_t *ilp);

#endif /* _initd_h_ */
