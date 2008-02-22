#ifndef _str_h_
#define _str_h_

extern char *d_string_new(const char *new);
extern void d_string_free(char *str);
extern void d_string_append(char *ds, const char *extra);

#endif /* _str_h_ */
