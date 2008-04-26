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
extern initd_t *initd_copy(const initd_t *source);
extern initd_t *initd_parse(const char *path);

/* Setters */
#define initd_add_prov(ip, name) prov_add(ip->prov, name)
#define initd_add_rstart(ip, name) dep_add(ip->rstart, name)
#define initd_add_rstop(ip, name) dep_add(ip->rstop, name)
#define initd_add_sstart(ip, name) dep_add(ip->sstart, name)
#define initd_add_sstop(ip, name) dep_add(ip->sstop, name)
extern void initd_set_rc(initd_t *ip, initd_key_t key, initd_rc_t level);
extern void initd_set_sdesc(initd_t *ip, const char *sdesc);
extern void initd_set_desc(initd_t *ip, const char *desc);
extern void initd_add_desc(initd_t *ip, const char *ext);
extern void initd_add_implicit_prov(initd_t *ip);

/* List functions */
extern initd_list_t *initd_list_new(void);
extern void initd_list_free(initd_list_t *ilp);
extern void initd_list_add(initd_list_t *ilp, initd_t *ip);
#define initd_list_push initd_list_add
extern void initd_list_pop(initd_list_t *ilp);
extern initd_list_t *initd_list_from_dir(const char *dir);
extern void initd_list_set_actives(initd_list_t *ilp, const char *dir);
extern initd_list_t *initd_list_copy(const initd_list_t *source);
extern initd_t *initd_list_find_name(const initd_list_t *ilp,
				const char *name);
extern initd_t *initd_list_find_provides(const initd_list_t *ilp,
					const char *serv);

/* Verification */
extern bool initd_list_exists_name(const initd_list_t *ilp,
				const char *name);
extern bool initd_provides(const initd_t *ip, const char *serv);
extern bool initd_list_provides(const initd_list_t *ilp,
				const char *serv);
extern bool initd_is_active(const initd_t *ip, initd_rc_t rc,
				initd_sk_t sk);
extern char *initd_verify_deps(const initd_list_t *ilp,
				const initd_t *ip, initd_key_t key);
extern char *initd_list_verify_all(const initd_list_t *ilp);

#endif /* _initd_h_ */
