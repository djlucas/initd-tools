typedef struct dep {
	char *name;
	unsigned int ndeps;
	char **deps;
} dep_t;

extern dep_t *dep_new(const char *name);
extern void dep_free(dep_t *dep);
extern void dep_add(dep_t *dp, const char *dep);
