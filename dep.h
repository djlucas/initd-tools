#ifndef _dep_h_
#define _dep_h_

#include "str.h"

typedef struct dep {
	char *name;
	strarg_t *deps;

	struct dep *prev;
	struct dep *next;
} dep_t;

typedef struct dep_list {
	dep_t *first;
	dep_t *last;
} dep_list_t;

extern dep_t *dep_new(const char *name);
extern void dep_free(dep_t *dep);
extern void dep_add(dep_t *dp, const char *dep);
extern dep_t *dep_copy(dep_t *source);
extern int dep_exists_name(dep_t *dp, const char *name);

extern dep_list_t *dep_list_new(void);
extern void dep_list_free(dep_list_t *dlp);
extern void dep_list_add(dep_list_t *dlp, dep_t *dp);
extern dep_list_t *dep_list_copy(dep_list_t *old);
extern dep_t *dep_list_find_name(dep_list_t *dlp, const char *name);
extern int dep_list_exists_name(dep_list_t *dlp, const char *name);
extern char *dep_list_verify_all(dep_list_t *dlp);

#endif /* _dep_h_ */
