#ifndef _initd_str_h_
#define _initd_str_h_

#include <stdbool.h>

extern char *d_string_new(const char *new);
extern void d_string_free(char *str);
extern char *d_string_append(char *ds, const char *extra);

typedef struct strarg {
	unsigned int nstr;
	char **str;
} strarg_t;

extern strarg_t *strarg_new(void);
extern void strarg_free(strarg_t *sp);
extern void strarg_add(strarg_t *sp, const char *s);
#define strarg_push strarg_add
extern void strarg_pop(strarg_t *sp);
extern bool strarg_exists(const strarg_t *sp, const char *s);
extern unsigned int strarg_get_num(const strarg_t *sp);
extern char *strarg_get_str(const strarg_t *sp, unsigned int index);
extern strarg_t *strarg_copy(const strarg_t *source);

#endif /* _initd_str_h_ */
