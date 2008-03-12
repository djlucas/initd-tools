#ifndef _prov_h_
#define _prov_h_

typedef struct prov {
	unsigned int nprov;
	char **prov;
} prov_t;

extern prov_t *prov_new(void);
extern void prov_free(prov_t *pp);
extern void prov_add(prov_t *pp, const char *name);
extern prov_t *prov_copy(prov_t *source);

#endif /* _prov_h_ */