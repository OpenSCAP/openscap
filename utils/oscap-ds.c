/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
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
 *      Martin Preisler <mpreisle@redhat.com>
 */

/* Standard header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/* DS */
#include <ds.h>

#include "oscap-tool.h"

static struct oscap_module* DS_SUBMODULES[];
bool getopt_ds(int argc, char **argv, struct oscap_action *action);
int app_ds_sds_split(const struct oscap_action *action);

struct oscap_module OSCAP_DS_MODULE = {
    .name = "ds",
    .parent = &OSCAP_ROOT_MODULE,
    .summary = "DataStream utilities",
    .submodules = DS_SUBMODULES
};

static struct oscap_module DS_SDS_SPLIT_MODULE = {
    .name = "sds_split",
    .parent = &OSCAP_DS_MODULE,
    .summary = "Split given SourceDataStream into separate files",
    .usage = "sds.xml target_directory/",
    .help = NULL,
    .opt_parser = getopt_ds,
    .func = app_ds_sds_split
};

static struct oscap_module* DS_SUBMODULES[] = {
    &DS_SDS_SPLIT_MODULE,
    NULL
};

bool getopt_ds(int argc, char **argv, struct oscap_action *action) {

    if( (action->module == &DS_SDS_SPLIT_MODULE) ) {
        if(  argc != 5 ) {
            oscap_module_usage(action->module, stderr, "Wrong number of parameteres.\n");
            return false;
        }
        action->ds_action = malloc(sizeof(struct cpe_action));
        action->ds_action->file = argv[3];
        action->ds_action->target_dir = argv[4];
    }

    return true;
}

int app_ds_sds_split(const struct oscap_action *action) {
    int ret;

    ds_sds_decompose(action->ds_action->file, NULL, action->ds_action->target_dir);

    ret = OSCAP_FAIL;

    return ret;
}
