typedef struct strarg {
	unsigned int nstr;
	char **str;
} strarg_t;

extern char *d_string_new(const char *new);
extern void d_string_free(char *str);
extern void d_string_append(char *ds, const char *extra);

extern strarg_t *strarg_new(void);
extern void strarg_free(strarg_t *sa);
extern void strarg_add(strarg_t *sa, const char *name);
extern strarg_t *strarg_copy(strarg_t *source);
