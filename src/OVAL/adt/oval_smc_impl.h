/*
 * Copyright 2013 Red Hat Inc., Durham, North Carolina.
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
 *      Šimon Lukašík
 *
 *
 * Structure providing 1--N mapping, by integrating two structures:
 * oval_string_map and oval_collection.
 */

#ifndef OVAL_SMC_H
#define OVAL_SMC_H

#include "common/util.h"


struct oval_smc;

struct oval_smc *oval_smc_new(void);

void oval_smc_put_last(struct oval_smc *map, const char *key, void *item);

void oval_smc_put_last_if_not_exists(struct oval_smc *map, const char *key, void *item);

struct oval_iterator *oval_smc_get_all_it(struct oval_smc *map, const char *key);

void *oval_smc_get_last(struct oval_smc *map, const char *key);

void oval_smc_free0(struct oval_smc *map);

void oval_smc_free(struct oval_smc *map, oscap_destruct_func destructor);

typedef void *(*oval_smc_user_clone_func) (void *user_data, void *item);

struct oval_smc *oval_smc_clone_user(struct oval_smc *oldmap, oval_smc_user_clone_func cloner, void *user_data);


#endif
