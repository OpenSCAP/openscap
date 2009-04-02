/*
 * ovalCollection.h
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */

#ifndef OVALCOLLECTION_H_
#define OVALCOLLECTION_H_

	struct oval_collection_item_s;
	struct oval_collection_s;
	struct oval_iterator_s;
	typedef void (*oval_item_free_func)(struct oval_collection_item_s*);

	struct oval_collection_s      *oval_collection_new();
	void                           oval_collection_free             (struct oval_collection_s*);
	void                           oval_collection_free_items       (struct oval_collection_s*, oval_item_free_func);
	void                           oval_collection_add              (struct oval_collection_s*, struct oval_collection_item_s*);
	struct oval_iterator_s        *oval_collection_iterator         (struct oval_collection_s*);
	struct oval_iterator_s        *oval_collection_iterator_new     ();
	void                           oval_collection_iterator_add     (struct oval_iterator_s*, struct oval_collection_item_s*);
	int                            oval_collection_iterator_has_more(struct oval_iterator_s*);
	struct oval_collection_item_s *oval_collection_iterator_next    (struct oval_iterator_s*);

	struct oval_iterator_string_s;

#endif /* OVALCOLLECTION_H_ */
