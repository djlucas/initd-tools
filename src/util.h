#ifndef _util_h_
#define _util_h_
#include <stdbool.h>

extern bool set_initd_dir(char **initd_dir, const strarg_t *args);
extern void set_verbose(bool verbose);

#endif /* _util_h_ */
