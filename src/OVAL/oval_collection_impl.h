/*
 * ovalCollection.h
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */

#ifndef OVALCOLLECTION_H_
#define OVALCOLLECTION_H_

struct oval_collection;
struct oval_iterator;
typedef void (*oval_item_free_func) (void *);

struct oval_collection *oval_collection_new();
void oval_collection_free(struct oval_collection *);
void oval_collection_free_items(struct oval_collection *, oval_item_free_func);
void oval_collection_add(struct oval_collection *, void *);
struct oval_iterator *oval_collection_iterator(struct oval_collection *);
struct oval_iterator *oval_collection_iterator_new();
void oval_collection_iterator_add(struct oval_iterator *, void *);
int oval_collection_iterator_has_more(struct oval_iterator *);
void *oval_collection_iterator_next(struct oval_iterator *);

struct oval_iterator_string;

#endif				/* OVALCOLLECTION_H_ */
