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
	ip->astart = ip->astop = 0;

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

initd_t *initd_copy(const initd_t *source)
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

void initd_set_rc(initd_t *ip, initd_key_t key, initd_rc_t level)
{
	if (!ip)
		return;

	switch (key) {
	case KEY_ASTART:
		ip->astart |= level;
		break;
	case KEY_ASTOP:
		ip->astop |= level;
		break;
	case KEY_DSTART:
		ip->dstart |= level;
		break;
	case KEY_DSTOP:
		ip->dstop |= level;
		break;
	default:
		break;
	}
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

/* Find whether a given service is provided by an initd. */
bool initd_provides(const initd_t *ip, const char *serv)
{
	bool found = false;
	int n;

	if (!ip || !ip->prov)
		goto out;

	for (n = 0; n < prov_get_num(ip->prov); n++) {
		if (strcmp(serv, prov_get_prov(ip->prov, n)) == 0) {
			found = true;
			break;
		}
	}

out:
	return found;
}

/* Kludge the name into the provides array in case any scripts want to
 * depend on a service by its name. */
void initd_add_implicit_prov(initd_t *ip)
{
	if (!ip || !ip->name)
		return;

	if (!initd_provides(ip, ip->name))
		initd_add_prov(ip, ip->name);
}

char initd_rc_level_char(initd_rc_t rc)
{
	char c = '\0';

	switch (rc) {
	case RC_S:
		c = 'S';
		break;
	case RC_0:
		c = '0';
		break;
	case RC_1:
		c = '1';
		break;
	case RC_2:
		c = '2';
		break;
	case RC_3:
		c = '3';
		break;
	case RC_4:
		c = '4';
		break;
	case RC_5:
		c = '5';
		break;
	case RC_6:
		c = '6';
		break;
	}

	return c;
}
