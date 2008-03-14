#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include "initd.h"
#include "str.h"

initd_t *initd_new(const char *name) {
	initd_t *ip = malloc(sizeof(initd_t));
	if (!ip)
		error(2, errno, "%s", __FUNCTION__);

	ip->name = d_string_new(name);

	ip->prov = prov_new();

	ip->dstart = ip->dstop = 0;

	ip->rstart = dep_new();
	ip->rstop = dep_new();
	ip->sstart = dep_new();
	ip->sstop = dep_new();

	ip->sdesc = ip->desc = NULL;

	ip->prev = NULL;
	ip->next = NULL;

	return ip;
}

void initd_free(initd_t *ip)
{
	/* free the dependencies */
	dep_free(ip->rstart);
	dep_free(ip->rstop);
	dep_free(ip->sstart);
	dep_free(ip->sstop);

	d_string_free(ip->sdesc);
	d_string_free(ip->desc);

	prov_free(ip->prov);

	d_string_free(ip->name);

	free(ip);
	ip = NULL;
}

initd_t *initd_copy(initd_t *source)
{
	initd_t *dest;
	char *name;

	if (!source) {
		dest = initd_new("");
		goto out;
	}

	if (source->name)
		name = source->name;
	else
		name = "";

	dest = initd_new(name);

	dest->prov = prov_copy(source->prov);

	dest->dstart = source->dstart;
	dest->dstop = source->dstop;

	dest->rstart = dep_copy(source->rstart);
	dest->rstop = dep_copy(source->rstop);
	dest->sstart = dep_copy(source->sstart);
	dest->sstop = dep_copy(source->sstop);

	dest->sdesc = d_string_new(source->sdesc);
	dest->desc = d_string_new(source->desc);

out:
	return dest;
}

void initd_set_sdesc(initd_t *ip, const char *sdesc)
{
	if (!ip)
		return;

	if (ip->sdesc)
		d_string_free(ip->sdesc);

	ip->sdesc = d_string_new(sdesc);
}

void initd_set_desc(initd_t *ip, const char *desc)
{
	if (!ip)
		return;

	if (ip->desc)
		d_string_free(ip->desc);

	ip->desc = d_string_new(desc);
}

void initd_add_desc(initd_t *ip, const char *ext)
{
	if (!ip || !ext)
		return;

	if (ip->desc) {
		ip->desc = d_string_append(ip->desc, " ");
		ip->desc = d_string_append(ip->desc, ext);
	} else {
		ip->desc = d_string_new(ext);
	}
}
