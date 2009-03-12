/*
 * ovalCollection.h
 *
 *  Created on: Mar 2, 2009
 *      Author: david.niemoller
 */

#ifndef OVALCOLLECTION_H_
#define OVALCOLLECTION_H_

	typedef void* OvalCollection_target;
	typedef struct OvalCollection_s OvalCollection_t;
	typedef OvalCollection_t* OvalCollection_ptr;

	OvalCollection_ptr newOvalCollection(OvalCollection_target*);
	int OvalCollection_hasNext(OvalCollection_ptr);
	OvalCollection_target OvalCollection_next(OvalCollection_ptr);

	OvalCollection_string newOvalCollection_string(char**);

#endif /* OVALCOLLECTION_H_ */
