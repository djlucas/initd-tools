#ifndef _dep_h_
#define _dep_h_

#include <stdbool.h>
#include "initd.h"
#include "types.h"

extern dep_t *initd_recurse_deps(initd_list_t *pool, initd_sk_t sk,
				const dep_t *needed);
extern void initd_recurse_set_verbose(bool verbose);

#endif /* _dep_h_ */
