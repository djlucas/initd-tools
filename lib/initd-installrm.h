#ifndef _initd_installrm_h_
#define _initd_installrm_h_

#include <stdbool.h>
#include "initd-types.h"

extern bool initd_installrm_links(const initd_list_t *ilp,
				const char *dir, initd_sk_t sk);
extern void initd_installrm_set_verbose(bool verbose);

#endif /* _initd_installrm_h_ */
