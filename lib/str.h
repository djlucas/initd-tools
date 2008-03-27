#ifndef _str_h_
#define _str_h_

#include <stdbool.h>

extern char *d_string_new(const char *new);
extern void d_string_free(char *str);
extern char *d_string_append(char *ds, const char *extra);

typedef struct strarg {
	unsigned int nstr;
	char **str;
} strarg_t;

extern char **strarg_new(unsigned int *num);
extern void strarg_free(char **strarg, unsigned int num);
extern char **strarg_add(char **strarg, unsigned int *num, const char *s);
extern char **strarg_pop(char **strarg, unsigned int *num);
extern bool strarg_exists(char **strarg, unsigned int num, const char *s);

#endif /* _str_h_ */
