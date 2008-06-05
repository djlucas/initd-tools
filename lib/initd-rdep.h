#ifndef _initd_rdep_h_
#define _initd_rdep_h_

#include <stdbool.h>
#include "initd-types.h"

extern initd_list_t *initd_add_recurse_deps(initd_list_t *pool,
				initd_sk_t sk, const dep_t *add);
extern initd_list_t *initd_remove_recurse_deps(initd_list_t *pool,
				initd_sk_t sk, const dep_t *remove);
extern void initd_recurse_set_verbose(bool verbose);
extern bool initd_list_verify_deps(const initd_list_t *ord, initd_sk_t sk);

#endif /* _initd_rdep_h_ */
