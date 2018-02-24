/**
 * @file   oval_probe_handler.c
 * @brief  OVAL probe handler API implementation
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 * @addtogroup PROBEHANDLERS
 * @{
 */
/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "common/alloc.h"
#include "common/bfind.h"
#include "_oval_probe_handler.h"

oval_phtbl_t *oval_phtbl_new(void)
{
        oval_phtbl_t *phtbl;

        phtbl = oscap_talloc(oval_phtbl_t);
        phtbl->ph = NULL;
        phtbl->sz = 0;

        return(phtbl);
}

void oval_phtbl_free(oval_phtbl_t *phtbl)
{
        register uint32_t i;

        for (i = 0; i < phtbl->sz; ++i) {
                if (phtbl->ph[i]->func)
                        phtbl->ph[i]->func(phtbl->ph[i]->type, phtbl->ph[i]->uptr, PROBE_HANDLER_ACT_FREE);
                free(phtbl->ph[i]);
        }

        free(phtbl->ph);
        free(phtbl);
}

static int oval_handler_subtype_cmp(oval_subtype_t *a, oval_ph_t **b)
{
        return(*a - (*b)->type);
}

static int oval_phtbl_subtype_cmp(oval_ph_t **a, oval_ph_t **b)
{
        return((*a)->type - (*b)->type);
}

oval_ph_t *oval_probe_handler_get(oval_phtbl_t *phtbl, oval_subtype_t type)
{
        oval_ph_t **ph = oscap_bfind((void *)phtbl->ph, phtbl->sz, sizeof(oval_ph_t *), &type,
                                     (int(*)(void *, void *))oval_handler_subtype_cmp);
        return (ph != NULL ? *ph : NULL);
}

int oval_probe_handler_set(oval_phtbl_t *phtbl, oval_subtype_t type, oval_probe_handler_t *handler, void *uptr)
{
        int  ret  = 0;
        bool sort = false;
        oval_ph_t *phrec;

        if (phtbl->sz > 0) {
                phrec = oval_probe_handler_get(phtbl, type);

                if (phrec != NULL) {
                        if ((ret = phrec->func(type, phrec->uptr, PROBE_HANDLER_ACT_CLOSE)) != 0 ||
                            (ret = phrec->func(type, phrec->uptr, PROBE_HANDLER_ACT_FREE))  != 0)
                        {
                                return(ret);
                        }

                        goto fillrec;
                }
        }

        phtbl->ph = realloc(phtbl->ph, sizeof(oval_ph_t *) * ++phtbl->sz);
        phrec = phtbl->ph[phtbl->sz - 1] = oscap_talloc(oval_ph_t);
        sort  = true;
fillrec:
	if (phrec == NULL) {
		return -1;
	}

        phrec->type = type;
        phrec->func = handler;
        phrec->uptr = uptr;

        if ((ret = phrec->func(type, phrec->uptr, PROBE_HANDLER_ACT_INIT)) != 0) {
                phrec->func = PROBE_HANDLER_IGNORE;
                phrec->uptr = NULL;
        }

        if (sort)
                qsort(phtbl->ph, phtbl->sz, sizeof(oval_ph_t *),
                      (int(*)(const void *, const void *))oval_phtbl_subtype_cmp);

        return(ret);
}

/// @}
