#ifndef _util_h_
#define _util_h_
#include <stdbool.h>
#include "initd-types.h"

extern bool set_initd_dir(char **initd_dir, const strarg_t *args);
extern void set_verbose(bool verbose);
extern bool installrm_action(const dep_t *services,
			const char *initd_dir, bool install);

#endif /* _util_h_ */
