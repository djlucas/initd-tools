#ifndef _initd_h_
#define _initd_h_

#include "dep.h"

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

extern initd_list_t *initd_list_new(void);
extern void initd_list_free(initd_list_t *ilp);
extern void initd_list_add(initd_list_t *ilp, initd_t *ip);
extern initd_list_t *initd_list_copy(initd_list_t *source);
extern initd_t *initd_list_find_name(initd_list_t *ilp, const char *name);
extern int initd_list_exists_name(initd_list_t *ilp, const char *name);
extern char *initd_list_verify_all(initd_list_t *ilp);

#endif /* _initd_h_ */
