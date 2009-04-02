/*
 * oval_string_map_impl.h
 *
 *  Created on: Mar 9, 2009
 *      Author: david.niemoller
 */

#include "oval_collection_impl.h"

#ifndef OVAL_STRING_MAP_IMPL_H_
#define OVAL_STRING_MAP_IMPL_H_

	struct oval_string_map_s;

	struct oval_string_map_s      *oval_string_map_new      ();
	void                           oval_string_map_put      (struct oval_string_map_s*, char*,struct oval_collection_item_s*);
	struct oval_iterator_s        *oval_string_map_keys     (struct oval_string_map_s*);
	struct oval_iterator_s        *oval_string_map_values   (struct oval_string_map_s*);
	struct oval_collection_item_s *oval_string_map_get_value(struct oval_string_map_s*, char*);
	void                           oval_string_map_free     (struct oval_string_map_s*, oval_item_free_func);

#endif /* OVAL_STRING_MAP_IMPL_H_ */
