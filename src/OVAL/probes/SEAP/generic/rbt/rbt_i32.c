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

#include <errno.h>
#include <assume.h>
#include <stdlib.h>
#include "rbt_common.h"
#include "rbt_i32.h"

static struct rbt_node *rbt_i32_node_alloc(void)
{
        struct rbt_node *n = NULL;
#ifndef _WIN32
        if (posix_memalign((void **)(void *)(&n), sizeof(void *),
                           sizeof (struct rbt_node) + sizeof (struct rbt_i32_node)) != 0)
        {
                abort ();
        }
#else
        // https://msdn.microsoft.com/en-us/library/8z34s9c6.aspx
        n = _aligned_malloc(sizeof (struct rbt_node) + sizeof (struct rbt_i32_node), sizeof(void *));
#endif
        n->_chld[0] = NULL;
        n->_chld[1] = NULL;

        return(n);
}

static void rbt_i32_node_free(struct rbt_node *n)
{
        if (n != NULL)
                free(rbt_node_ptr(n));
}

rbt_t *rbt_i32_new (void)
{
        return rbt_new (RBT_I32KEY);
}

void rbt_i32_free (rbt_t *rbt)
{
        rbt_free(rbt, NULL);
}

void rbt_i32_free_cb (rbt_t *rbt, void (*callback)(rbt_i32_node_t *))
{
        rbt_free(rbt, (void(*)(void *))callback);
}

/*
 * Add new entry to the tree. If `coll' is not NULL and there is
 * already an entry with the same key (i.e. a collision), then
 * place the old data pointer at `*coll' and update it to the
 * new data pointer. Otherwise, if `coll' is NULL and there is a
 * collision, then an error is returned and no modification of
 * the tree is made.
 *
 * @param rbt pointer to the tree
 * @param key key
 * @param data data pointer
 * @param coll where to store the data pointer from an colliding entry
 */
int rbt_i32_add(rbt_t *rbt, int32_t key, void *data, void **coll)
{
        struct rbt_node fake;
        register struct rbt_node *h[4];
        register uint8_t dvec;
        register int32_t n_key, u_key;

        u_key = key;
        rbt_wlock(rbt);

        /*
         * Fake node
         */
        fake._chld[RBT_NODE_SL] = NULL;
        fake._chld[RBT_NODE_SR] = rbt->root;
        rbt_node_setcolor(&fake, RBT_NODE_CB);

        /*
         * Prepare node history stack & direction history vector
         */
        h[3] = NULL;
        h[2] = NULL;
        h[1] = &fake;
        h[0] = rbt->root;
        dvec = RBT_NODE_SR;

        for (;;) {
                if (rbt_node_ptr(h[0]) == NULL) {
                        /*
                         * Allocate new node aligned to sizeof(void *) bytes.
                         * On most systems, malloc already returns aligned
                         * memory but we want to ensure that its aligned using
                         * posix_memalign(3).
                         */
                        h[0] = rbt_i32_node_alloc ();

                        rbt_i32_node(h[0])->key = key;
                        rbt_i32_node(h[0])->data = data;

                        /*
                         * Set the new node as the child of the last node we've
                         * visited. The last direction is the lowest bit of the
                         * direction history vector.
                         */
                        rbt_node_setptr(rbt_node_ptr(h[1])->_chld[dvec & 1], h[0]);
                        rbt_node_setcolor(h[0], RBT_NODE_CR);

                        /*
                         * Since we are inserting a new red node, we need to fix
                         * a red violation if the parent of the new node is red.
                         */
                        if (rbt_node_getcolor(h[1]) == RBT_NODE_CR) {
                                rbt_redfix(h, dvec, rbt_node_ptr(h[3])->_chld[(dvec >> 2) & 1]);
                        }

                        /*
                         * Update root node and color it black
                         */
                        rbt->root = fake._chld[RBT_NODE_SR];
                        rbt_node_setcolor(rbt->root, RBT_NODE_CB);

                        /*
                         * Update node counter
                         */
                        ++(rbt->size);

                        break;
                } else if (rbt_node_getcolor(rbt_node_ptr(h[0])->_chld[0]) == RBT_NODE_CR &&
                           rbt_node_getcolor(rbt_node_ptr(h[0])->_chld[1]) == RBT_NODE_CR)
                {
                        /*
                         * Color switch
                         */
                        rbt_node_setcolor(h[0], RBT_NODE_CR);
                        rbt_node_setcolor(rbt_node_ptr(h[0])->_chld[0], RBT_NODE_CB);
                        rbt_node_setcolor(rbt_node_ptr(h[0])->_chld[1], RBT_NODE_CB);

                        /*
                         * Fix red violation
                         */
                        if (rbt_node_getcolor(h[1]) == RBT_NODE_CR) {
                                rbt_redfix(h, dvec, rbt_node_ptr(h[3])->_chld[(dvec >> 2) & 1]);
                        }
                } else if (rbt_node_getcolor(h[0]) == RBT_NODE_CR &&
                           rbt_node_getcolor(h[1]) == RBT_NODE_CR)
                {
                        /*
                         * Fix red violation
                         */
                        rbt_redfix(h, dvec, rbt_node_ptr(h[3])->_chld[(dvec >> 2)&1]);
                }

                n_key = rbt_i32_node(h[0])->key;

                if (u_key < n_key) {
                        rbt_hpush4(h, rbt_node_ptr(h[0])->_chld[RBT_NODE_SL]);
                        dvec = (dvec << 1) | RBT_NODE_SL;
                } else if (u_key > n_key) {
                        rbt_hpush4(h, rbt_node_ptr(h[0])->_chld[RBT_NODE_SR]);
                        dvec = (dvec << 1) | RBT_NODE_SR;
                } else {
			int r;

			if (coll != NULL) {
				*coll = rbt_i32_node(h[0])->data;
				rbt_i32_node(h[0])->data = data;
				r = 0;
			} else
				r = 1;

                        /*
                         * Update root node and color it black
                         */
                        rbt->root = fake._chld[RBT_NODE_SR];
                        rbt_node_setcolor(rbt->root, RBT_NODE_CB);

                        rbt_wunlock(rbt);
                        return (r);
                }
        }

        rbt_wunlock(rbt);
        return (0);
}

void *rbt_i32_rep(rbt_t *rbt, int32_t key, void *data)
{
        return (NULL);
}

int rbt_i32_del(rbt_t *rbt, int32_t key, void **n)
{
        struct rbt_node fake, *save;
        struct rbt_node *h[4];
        register uint8_t dvec;
        register int32_t n_key, u_key;

        save  = NULL;
        u_key = key;

        rbt_wlock(rbt);

        if (rbt->size == 0) {
                rbt_wunlock(rbt);
                return (1);
        }

        assume_d(rbt_node_ptr(rbt->root) != NULL, -1);

        /*
         * Fake node
         */
        fake._chld[RBT_NODE_SL] = NULL;
        fake._chld[RBT_NODE_SR] = rbt->root;
        rbt_node_setcolor(&fake, RBT_NODE_CB);

        /*
         * Prepare node history stack & direction history vector
         */
        h[2] = NULL;
        h[1] = NULL;
        h[0] = &fake;
        dvec = RBT_NODE_SR;

        for (;;) {
                if (rbt_node_ptr(rbt_node_ptr(h[0])->_chld[dvec & 1]) == NULL)
                        break;

                rbt_hpush4(h, rbt_node_ptr(h[0])->_chld[dvec & 1]);
                n_key = rbt_i32_node(h[0])->key;

                if (u_key < n_key) {
                        dvec = (dvec << 1) | RBT_NODE_SL;
                } else if (u_key > n_key) {
                        dvec = (dvec << 1) | RBT_NODE_SR;
                } else {
                        save = rbt_node_ptr(h[0]);
                        dvec = (dvec << 1) | RBT_NODE_SL;
                }

                if (rbt_node_getcolor(h[0]) == RBT_NODE_CB &&
                    rbt_node_getcolor(rbt_node_ptr(h[0])->_chld[dvec & 1]) == RBT_NODE_CB)
                {
                        if (rbt_node_getcolor(rbt_node_ptr(h[0])->_chld[!(dvec & 1)]) == RBT_NODE_CR) {
                                switch(dvec & 1) {
                                case RBT_NODE_SR:
                                        rbt_node_setptr(rbt_node_ptr(h[1])->_chld[(dvec >> 1) & 1],
                                                        rbt_node_rotate_R(h[0]));
                                        break;
                                case RBT_NODE_SL:
                                        rbt_node_setptr(rbt_node_ptr(h[1])->_chld[(dvec >> 1) & 1],
                                                        rbt_node_rotate_L(h[0]));
                                        break;
                                }

                                h[1] = rbt_node_ptr(h[1])->_chld[(dvec >> 1) & 1];
                        } else {
                                register struct rbt_node *s;

                                s = rbt_node_ptr(rbt_node_ptr(h[1])->_chld[!((dvec >> 1) & 1)]);

                                if (s != NULL) {
                                        if (rbt_node_getcolor(s->_chld[0]) == RBT_NODE_CB &&
                                            rbt_node_getcolor(s->_chld[1]) == RBT_NODE_CB)
                                        {
                                                rbt_node_setcolor(h[0], RBT_NODE_CR);
                                                rbt_node_setcolor(h[1], RBT_NODE_CB);
                                                rbt_node_setcolor(s,    RBT_NODE_CR);
                                        } else {
                                                register uint8_t d = rbt_node_ptr(rbt_node_ptr(h[2])->_chld[RBT_NODE_SR]) == rbt_node_ptr(h[1]);

                                                if (rbt_node_getcolor(s->_chld[(dvec >> 1) & 1]) == RBT_NODE_CR) {
                                                        switch((dvec >> 1) & 1) {
                                                        case RBT_NODE_SR:
                                                                rbt_node_setptr(rbt_node_ptr(h[2])->_chld[d],
                                                                                rbt_node_rotate_RL(h[1]));
                                                                break;
                                                        case RBT_NODE_SL:
                                                                rbt_node_setptr(rbt_node_ptr(h[2])->_chld[d],
                                                                                rbt_node_rotate_LR(h[1]));
                                                                break;
                                                        }
                                                } else if (rbt_node_getcolor(s->_chld[!((dvec >> 1) & 1)]) == RBT_NODE_CR) {
                                                        switch((dvec >> 1) & 1) {
                                                        case RBT_NODE_SR:
                                                                rbt_node_setptr(rbt_node_ptr(h[2])->_chld[d],
                                                                                rbt_node_rotate_R(h[1]));
                                                                break;
                                                        case RBT_NODE_SL:
                                                                rbt_node_setptr(rbt_node_ptr(h[2])->_chld[d],
                                                                                rbt_node_rotate_L(h[1]));
                                                                break;
                                                        }
                                                }

                                                s = rbt_node_ptr(rbt_node_ptr(h[2])->_chld[d]);

                                                rbt_node_setcolor(h[0], RBT_NODE_CR);
                                                rbt_node_setcolor(s,    RBT_NODE_CR);
                                                rbt_node_setcolor(s->_chld[0], RBT_NODE_CB);
                                                rbt_node_setcolor(s->_chld[1], RBT_NODE_CB);
                                        }
                                }
                        }
                }
        }

        if (save != NULL) {
                h[0] = rbt_node_ptr(h[0]);
                /*
                 * The node color of the node that will be delete is always
                 * red in case the node is not the root node.
                 */
                assume_d(rbt_node_ptr(fake._chld[RBT_NODE_SR]) == h[0] || rbt_node_getcolor(h[0]) == RBT_NODE_CR, -1);
                if (n != NULL)
                        *n = rbt_i32_node(save)->data;

                rbt_i32_node(save)->data = rbt_i32_node(h[0])->data;
                rbt_i32_node(save)->key = rbt_i32_node(h[0])->key;

                dvec = rbt_node_ptr(rbt_node_ptr(h[1])->_chld[1]) == h[0];

                if (rbt_node_ptr(h[0]->_chld[RBT_NODE_SR]) == NULL) {
                        rbt_node_setptr(rbt_node_ptr(h[1])->_chld[dvec], h[0]->_chld[RBT_NODE_SL]);
                } else {
                        rbt_node_setptr(rbt_node_ptr(h[1])->_chld[dvec], h[0]->_chld[RBT_NODE_SR]);
                }

                /*
                 * Free memory used by the deleted node and decrement
                 * the node counter.
                 */
                rbt_i32_node_free(h[0]);
                --(rbt->size);
        }

        /*
         * Update root node and color it black and unlock the tree.
         */
        rbt->root = fake._chld[RBT_NODE_SR];
        rbt_node_setcolor(rbt->root, RBT_NODE_CB);
        rbt_wunlock(rbt);

        return (save == NULL ? 1 : 0);
}

int rbt_i32_get(rbt_t *rbt, int32_t key, void **data)
{
        int r;
        register struct rbt_node *n;
        register int32_t u_key, n_key;

        u_key = key;
        r = -1;
        rbt_rlock(rbt);
        n = rbt_node_ptr(rbt->root);

        while (n != NULL) {
                n_key = rbt_i32_node(n)->key;

                if (u_key < n_key)
                        n = rbt_node_ptr(n->_chld[RBT_NODE_SL]);
                else if (u_key > n_key)
                        n = rbt_node_ptr(n->_chld[RBT_NODE_SR]);
                else {
                        r = 0;
                        *data = rbt_i32_node(n)->data;
                        break;
                }
        }

        rbt_runlock(rbt);
        return (r);
}

int rbt_i32_walk_preorder(rbt_t *rbt, int (*callback)(rbt_i32_node_t *), rbt_walk_t flags)
{
        errno = ENOSYS;
        return (-1);
}

int rbt_i32_walk_inorder(rbt_t *rbt, int (*callback)(rbt_i32_node_t *), rbt_walk_t flags)
{
        return rbt_walk_inorder(rbt, (int(*)(void *))callback, flags);
}

int rbt_i32_walk_postorder(rbt_t *rbt, int (*callback)(rbt_i32_node_t *), rbt_walk_t flags)
{
        errno = ENOSYS;
        return (-1);
}

int rbt_i32_walk_levelorder(rbt_t *rbt, int (*callback)(rbt_i32_node_t *), rbt_walk_t flags)
{
        errno = ENOSYS;
        return (-1);
}

int rbt_i32_walk(rbt_t *rbt, rbt_walk_t type, int (*callback)(rbt_i32_node_t *))
{
        assume_d (rbt      != NULL, -1, errno = EINVAL;);
        assume_d (callback != NULL, -1, errno = EINVAL;);

        switch (type & RBT_WALK_TYPEMASK) {
        case RBT_WALK_PREORDER:   return rbt_i32_walk_preorder(rbt, callback, type);
        case RBT_WALK_INORDER:    return rbt_i32_walk_inorder(rbt, callback, type);
        case RBT_WALK_POSTORDER:  return rbt_i32_walk_postorder(rbt, callback, type);
        case RBT_WALK_LEVELORDER: return rbt_i32_walk_levelorder(rbt, callback, type);
        }

        errno = EINVAL;
        return (-1);
}

size_t rbt_i32_size(rbt_t *rbt)
{
        return rbt_size(rbt);
}
