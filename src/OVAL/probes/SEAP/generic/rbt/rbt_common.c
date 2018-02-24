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

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "rbt_common.h"

rbt_t *rbt_new(rbt_type_t type)
{
        rbt_t *rbt;

        switch (type) {
        case RBT_GENKEY:
        case RBT_STRKEY:
        case RBT_I32KEY:
        case RBT_I64KEY:
                break;
        default:
                errno = EINVAL;
                return (NULL);
        }

        rbt = malloc (sizeof (rbt_t));
        if (rbt == NULL)
	        return NULL;

        rbt->type = type;
        rbt->root = NULL;
        rbt->size = 0;

#if defined(RBT_IMPLICIT_LOCKING)
        pthread_rwlock_init (&rbt->lock, NULL);
#endif
        return (rbt);
}

/**
 * Free resources used to hold information about the tree. This
 * function does not free memory pointed to by the data pointer
 * in a node - you have to free this memory by walking thru the
 * tree or use the callback version of rbt_free for a specific
 * tree type.
 * @param rbt tree pointer
 */
void rbt_free(rbt_t *rbt, void (*callback)(void *))
{
        struct rbt_node *stack[48], *n;
        register uint8_t depth;

        rbt_wlock(rbt);

        depth = 0;
        n = rbt_node_ptr(rbt->root);

        if (n != NULL) {
                rbt_walk_push(n);

                while(depth > 0) {
                        n = rbt_node_ptr(rbt_walk_top()->_chld[RBT_NODE_SL]);

                        if (n != NULL)
                                rbt_walk_push(n);
                        else {
                        found_in:
                                if (callback != NULL)
                                        callback((void *)&(rbt_walk_top()->_node));

                                n = rbt_node_ptr(rbt_walk_top()->_chld[RBT_NODE_SR]);
#ifndef _WIN32
                                free(rbt_walk_top());
#else
				// using free for memory allocated through _aligned_malloc is illegal
				// rbt_str.c -> rbt_str_node_alloc
				// https://msdn.microsoft.com/en-us/library/8z34s9c6.aspx
				_aligned_free(rbt_walk_top());
#endif

                                if (n != NULL)
                                        rbt_walk_top() = n;
                                else {
                                        if (--depth > 0)
                                                goto found_in;
                                        else
                                                break;
                                }
                        }
                }
        }
        rbt_wunlock(rbt);

        rbt->root = NULL;
        rbt->size = 0;
        rbt->type = -1;

#if defined(RBT_IMPLICIT_LOCKING)
        pthread_rwlock_destroy (&rbt->lock);
#endif
        free (rbt);
        return;
}

void rbt_free2(rbt_t *rbt, void (*callback)(void *, void *), void *user)
{
        struct rbt_node *stack[48], *n;
        register uint8_t depth;

        rbt_wlock(rbt);

        depth = 0;
        n = rbt_node_ptr(rbt->root);

        if (n != NULL) {
                rbt_walk_push(n);

                while(depth > 0) {
                        n = rbt_node_ptr(rbt_walk_top()->_chld[RBT_NODE_SL]);

                        if (n != NULL)
                                rbt_walk_push(n);
                        else {
                        found_in:
                                if (callback != NULL)
                                        callback((void *)&(rbt_walk_top()->_node), user);

                                n = rbt_node_ptr(rbt_walk_top()->_chld[RBT_NODE_SR]);
#ifndef _WIN32
                                free(rbt_walk_top());
#else
				// using free for memory allocated through _aligned_malloc is illegal
				// rbt_str.c -> rbt_str_node_alloc
				// https://msdn.microsoft.com/en-us/library/8z34s9c6.aspx
				_aligned_free(rbt_walk_top());
#endif

                                if (n != NULL)
                                        rbt_walk_top() = n;
                                else {
                                        if (--depth > 0)
                                                goto found_in;
                                        else
                                                break;
                                }
                        }
                }
        }
        rbt_wunlock(rbt);

        rbt->root = NULL;
        rbt->size = 0;
        rbt->type = -1;

#if defined(RBT_IMPLICIT_LOCKING)
        pthread_rwlock_destroy (&rbt->lock);
#endif
        free (rbt);
        return;
}

#if defined(RBT_IMPLICIT_LOCKING)
int rbt_rlock(rbt_t *rbt)
{
	if (rbt == NULL) {
		return -1;
	}

        if (pthread_rwlock_rdlock(&rbt->lock) != 0)
                return (-1);
        else
                return (0);
}

void rbt_runlock(rbt_t *rbt)
{
	if (rbt == NULL) {
		return;
	}

        if (pthread_rwlock_unlock(&rbt->lock) != 0) {
                /*
                 * Unlock failed - we abort() here so as
                 * not to cause a deadlock
                 */
                abort ();
        }

        return;
}

int rbt_wlock(rbt_t *rbt)
{
	if (rbt == NULL) {
		return -1;
	}
        if (pthread_rwlock_wrlock(&rbt->lock) != 0)
                return (-1);
        else
                return (0);
}

void rbt_wunlock(rbt_t *rbt)
{
	if (rbt == NULL) {
		return;
	}
        rbt_runlock(rbt);
        return;
}
#else
int  rbt_rlock(rbt_t *rbt) { return(0); }
void rbt_runlock(rbt_t *rbt) { return; }
int  rbt_wlock(rbt_t *rbt) { return(0); }
void rbt_wunlock(rbt_t *rbt) { return; }
#endif /* RBT_IMPLICIT_LOCKING */

/*
 * Single rotation to the left. Used for fixing a black violation.
 *
 * == Assumptions ==
 *  - input node is black
 *  - output node is black
 *  - child nodes of the new subtree node (the output node) are red
 */
struct rbt_node *rbt_node_rotate_L(struct rbt_node *r)
{
        register struct rbt_node *n, *o;

        o = rbt_node_ptr(r);

        /*
         * New subtree root node `n' is the left child node of
         * the old subtree root node `r'. This node has the
         * color black.
         */
        n = rbt_node_ptr(o->_chld[RBT_NODE_SR]);

        /*
         * Now the left child has to be updated so that it
         * points to the right child of the old left child
         * node. The old left child node pointer is stored
         * in `n'.
         */
        o->_chld[RBT_NODE_SR] = n->_chld[RBT_NODE_SL];
        n->_chld[RBT_NODE_SL] = o;

        /*
         * Make sure that nodes have proper colors set.
         */
        rbt_node_setcolor(n, RBT_NODE_CB);
        rbt_node_setcolor(o, RBT_NODE_CR);

        return (struct rbt_node *)((uintptr_t)(n)|((uintptr_t)(r)&1));
}

/*
 * Single rotation to the right. Used for fixing a black violation.
 * No-recoloring needed here.
 */
struct rbt_node *rbt_node_rotate_R(struct rbt_node *r)
{
        register struct rbt_node *n, *o;

        o = rbt_node_ptr(r);

        /*
         * New subtree root node `n' is the left child node of
         * the old subtree root node `r'. This node has the
         * color black.
         */
        n = rbt_node_ptr(o->_chld[RBT_NODE_SL]);

        /*
         * Now the right child has to be updated so that it
         * points to the left child of the old right child
         * node. The old right child node pointer is stored
         * in `n'.
         */
        o->_chld[RBT_NODE_SL] = n->_chld[RBT_NODE_SR];
        n->_chld[RBT_NODE_SR] = o;

        /*
         * Make sure that nodes have proper colors set.
         */
        rbt_node_setcolor(n, RBT_NODE_CB);
        rbt_node_setcolor(o, RBT_NODE_CR);

        return (struct rbt_node *)((uintptr_t)(n)|((uintptr_t)(r)&1));
}

/*
 * Double rotation to the left and then to the right. Used for fixing a red violation.
 */
struct rbt_node *rbt_node_rotate_LR(struct rbt_node *r)
{
        register struct rbt_node *n, *o;

        o = rbt_node_ptr(r);
        n = rbt_node_ptr(rbt_node_ptr(o->_chld[RBT_NODE_SR])->_chld[RBT_NODE_SL]);

        rbt_node_setptr(rbt_node_ptr(o->_chld[RBT_NODE_SR])->_chld[RBT_NODE_SL], n->_chld[RBT_NODE_SR]);

        n->_chld[RBT_NODE_SR] = o->_chld[RBT_NODE_SR];
        o->_chld[RBT_NODE_SR] = n->_chld[RBT_NODE_SL];
        n->_chld[RBT_NODE_SL] = o;

        rbt_node_setcolor(n->_chld[RBT_NODE_SR], RBT_NODE_CR);
        rbt_node_setcolor(n->_chld[RBT_NODE_SL], RBT_NODE_CR);
        rbt_node_setcolor(n, RBT_NODE_CB);

        return (struct rbt_node *)((uintptr_t)(n)|((uintptr_t)(r)&1));
}

/*
 * Double rotation to the right and then to the left. Used for fixing a red violation.
 */
struct rbt_node *rbt_node_rotate_RL(struct rbt_node *r)
{
        register struct rbt_node *n, *o;

        o = rbt_node_ptr(r);
        n = rbt_node_ptr(rbt_node_ptr(o->_chld[RBT_NODE_SL])->_chld[RBT_NODE_SR]);

        rbt_node_ptr(o->_chld[RBT_NODE_SL])->_chld[RBT_NODE_SR] = n->_chld[RBT_NODE_SL];

        n->_chld[RBT_NODE_SL] = o->_chld[RBT_NODE_SL];
        rbt_node_setptr(o->_chld[RBT_NODE_SL], n->_chld[RBT_NODE_SR]);
        n->_chld[RBT_NODE_SR] = o;

        rbt_node_setcolor(n->_chld[RBT_NODE_SR], RBT_NODE_CR);
        rbt_node_setcolor(n->_chld[RBT_NODE_SL], RBT_NODE_CR);
        rbt_node_setcolor(n, RBT_NODE_CB);

        return (struct rbt_node *)((uintptr_t)(n)|((uintptr_t)(r)&1));
}

size_t rbt_size(rbt_t *rbt)
{
        size_t size;

        rbt_rlock(rbt);
        size = rbt->size;
        rbt_runlock(rbt);

        return (size);
}

int rbt_walk_preorder(rbt_t *rbt, int (*callback)(void *), rbt_walk_t flags)
{
        return(-1);
}

int rbt_walk_inorder(rbt_t *rbt, int (*callback)(void *), rbt_walk_t flags)
{
        struct rbt_node *stack[48], *n;
        register uint8_t depth, delta;
        register int     r;

	if (rbt == NULL) {
		return -1;
	}

        if (flags & RBT_WALK_RAWNODE)
                delta = 0;
        else {
                delta = sizeof(void *) * 2;
		if ((sizeof(void *) * 2) != (size_t)(((struct rbt_node *)(NULL))->_node)) {
			return -1;
		}
        }

        depth = 0;
        n = rbt_node_ptr(rbt->root);

        if (n != NULL)
                rbt_walk_push(n);
        else
                return (0);

        while(depth > 0) {
                n = rbt_node_ptr(rbt_walk_top()->_chld[RBT_NODE_SL]);

                if (n != NULL)
                        rbt_walk_push(n);
                else {
                found_in:
                        r = callback((void *)((uintptr_t)(rbt_walk_top()) + delta));

                        if (r != 0)
                                return (r);

                        n = rbt_node_ptr(rbt_walk_top()->_chld[RBT_NODE_SR]);

                        if (n != NULL)
                                rbt_walk_top() = n;
                        else {
                                if (--depth > 0)
                                        goto found_in;
                                else
                                        break;
                        }
                }
        }

        return(0);
}

int rbt_walk_inorder2(rbt_t *rbt, int (*callback)(void *, void *), void *user, rbt_walk_t flags)
{
        struct rbt_node *stack[48], *n;
        register uint8_t depth, delta;
        register int     r;

	if (rbt == NULL) {
		return -1;
	}

        if (flags & RBT_WALK_RAWNODE)
                delta = 0;
        else {
                delta = sizeof(void *) * 2;
		if ((sizeof(void *) * 2) != (size_t)(((struct rbt_node *)(NULL))->_node)) {
			return -1;
		}
        }

        depth = 0;
        n = rbt_node_ptr(rbt->root);

        if (n != NULL)
                rbt_walk_push(n);
        else
                return (0);

        while(depth > 0) {
                n = rbt_node_ptr(rbt_walk_top()->_chld[RBT_NODE_SL]);

                if (n != NULL)
                        rbt_walk_push(n);
                else {
                found_in:
                        r = callback((void *)((uintptr_t)(rbt_walk_top()) + delta), user);

                        if (r != 0)
                                return (r);

                        n = rbt_node_ptr(rbt_walk_top()->_chld[RBT_NODE_SR]);

                        if (n != NULL)
                                rbt_walk_top() = n;
                        else {
                                if (--depth > 0)
                                        goto found_in;
                                else
                                        break;
                        }
                }
        }

        return(0);
}

int rbt_walk_postorder(rbt_t *rbt, int (*callback)(void *), rbt_walk_t flags)
{
        return(-1);
}

#ifndef HAVE_POSIX_MEMALIGN
# ifdef HAVE_MEMALIGN

/* Implementing posix_memalign using memalign */
int posix_memalign(void **memptr, size_t alignment, size_t size)
{
        if ((alignment % sizeof(void *)) != 0)
        {
                return EINVAL; 
        }

        *memptr = memalign(alignment, size);

        /* The spec for posix_memalign requires that alignment be a power
          of 2.   Memalign checks for that case, and returns NULL on failure. */
        if (*memptr == NULL)
        {
                /* posix_memalign must return an appropriate error code */
                return EINVAL;
        }
        return 0;
}
# endif /* HAVE_MEMALIGN */
#endif /* HAVE_POSIX_MEMALIGN */
