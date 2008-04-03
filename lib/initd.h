#ifndef _initd_h_
#define _initd_h_

#include <stdbool.h>
#include "types.h"
#include "rdep.h"

#ifndef DEF_INITD_DIR
#define DEF_INITD_DIR "/etc/init.d"
#endif

#ifndef INITD_LINE_MAX
#define INITD_LINE_MAX 256
#endif

extern initd_t *initd_new(const char *name);
extern void initd_free(initd_t *ip);
extern initd_t *initd_copy(initd_t *source);
extern initd_t *initd_parse(const char *path);

/* Setters */
#define initd_add_prov(ip, name) prov_add(ip->prov, name)
#define initd_add_rstart(ip, name) dep_add(ip->rstart, name)
#define initd_add_rstop(ip, name) dep_add(ip->rstop, name)
#define initd_add_sstart(ip, name) dep_add(ip->sstart, name)
#define initd_add_sstop(ip, name) dep_add(ip->sstop, name)
extern void initd_set_sdesc(initd_t *ip, const char *sdesc);
extern void initd_set_desc(initd_t *ip, const char *desc);
extern void initd_add_desc(initd_t *ip, const char *ext);

/* List functions */
extern initd_list_t *initd_list_new(void);
extern void initd_list_free(initd_list_t *ilp);
extern void initd_list_add(initd_list_t *ilp, initd_t *ip);
extern initd_list_t *initd_list_from_dir(const char *dir);
extern initd_list_t *initd_list_copy(initd_list_t *source);
extern initd_t *initd_list_find_name(initd_list_t *ilp, const char *name);
extern bool initd_list_exists_name(initd_list_t *ilp, const char *name);

/* Verification */
extern bool initd_provides(initd_t *ip, const char *serv);
extern bool initd_list_provides(initd_list_t *ilp, const char *serv);
extern char *initd_verify_deps(initd_list_t *ilp, initd_t *ip,
				initd_key_t key);
extern char *initd_list_verify_all(initd_list_t *ilp);

#endif /* _initd_h_ */
