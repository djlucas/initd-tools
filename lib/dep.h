#ifndef _dep_h_
#define _dep_h_

#include "str.h"

typedef struct dep {
	unsigned int ndep;
	char **dep;
} dep_t;

extern dep_t *dep_new(void);
extern void dep_free(dep_t *dp);
extern void dep_add(dep_t *dp, const char *name);
extern dep_t *dep_copy(dep_t *source);

#endif /* _dep_h_ */
