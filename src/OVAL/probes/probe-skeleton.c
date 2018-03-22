/**
 * @file   probe-skeleton.c
 * @brief  probe skeleton
 * @author "Daniel Kopecek" <dkopecek@redhat.com>
 *
 */

/*
 * Copyright 2011 Red Hat Inc., Durham, North Carolina.
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

#include <probe-api.h>

int probe_offline_mode_supported(void)
{
	return PROBE_OFFLINE_NONE;
}

void probe_preload()
{
	/* preload dynamic libraries */
	return;
}

void *probe_init(void)
{
        /* initialize stuff */
        return (NULL);
}

void probe_fini(void *probe_arg)
{
        /* cleanup stuff */
        return;
}

int probe_main(SEXP_t *probe_in, SEXP_t *probe_out, void *probe_arg, SEXP_t *filters)
{
        return (PROBE_EUNKNOWN);
}
