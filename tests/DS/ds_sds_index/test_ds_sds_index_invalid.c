/*
 * Copyright 2012 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * Authors:
 *      Martin Preisler <mpreisle@redhat.com>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scap_ds.h"
#include "common/list.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Invalid arguments, usage: ./test_ds_sds_index_invalid INVALID_FILE");
		return 2;
	}

	struct ds_sds_index* idx = ds_sds_index_import("nonexistant_file");
	if (idx != NULL)
	{
		printf("Expected to get NULL when importing nonexistant file with ds_sds_index_import, "
		       "got '%p' instead!\n", idx);

		return 1;
	}

	struct ds_sds_index* idx2 = ds_sds_index_import(".");
	if (idx2 != NULL)
	{
		printf("Expected to get NULL when importing a directory instead of a file with ds_sds_index_import, "
		       "got '%p' instead!\n", idx2);

		return 1;
	}

	struct ds_sds_index* index3 = ds_sds_index_import(argv[1]);
	if (index3 != NULL)
	{
		printf("Expected to get NULL when importing an invalid file with ds_sds_index_import, "
		       "got '%p' instead!\n", index3);

		return 1;
	}

	return 0;
}
