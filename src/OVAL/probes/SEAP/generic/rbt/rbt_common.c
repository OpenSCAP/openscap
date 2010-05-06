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
#include <stdint.h>
#include <errno.h>
#include <assume.h>
#include "rbt_common.h"
#include "sm_alloc.h"

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

        rbt = sm_talloc (rbt_t);
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
void rbt_free(rbt_t *rbt)
{
        /*
         * TODO: free nodes
         */
        rbt->root = NULL;
        rbt->size = 0;
        rbt->type = -1;

#if defined(RBT_IMPLICIT_LOCKING)
        pthread_rwlock_destroy (&rbt->lock);
#endif
        sm_free (rbt);
        return;
}

#if defined(RBT_IMPLICIT_LOCKING)
int rbt_rlock(rbt_t *rbt)
{
        assume_d (rbt != NULL, -1);

        if (pthread_rwlock_rdlock(&rbt->lock) != 0)
                return (-1);
        else
                return (0);
}

void rbt_runlock(rbt_t *rbt)
{
        assume_d (rbt != NULL, /* void */);

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
        assume_d (rbt != NULL, -1);
        if (pthread_rwlock_wrlock(&rbt->lock) != 0)
                return (-1);
        else
                return (0);
}

void rbt_wunlock(rbt_t *rbt)
{
        assume_d (rbt != NULL, /* void */);
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
