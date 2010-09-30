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
#ifndef RBT_COMMON_H
#define RBT_COMMON_H

#include <stdint.h>
#include <stdbool.h>

#if defined(RBT_IMPLICIT_LOCKING)
# include <pthread.h>
#endif

typedef enum {
        RBT_GENKEY,
        RBT_STRKEY,
        RBT_I32KEY,
        RBT_I64KEY
} rbt_type_t;

typedef enum {
        RBT_WALK_PREORDER   = 0x01,
        RBT_WALK_INORDER    = 0x02,
        RBT_WALK_POSTORDER  = 0x03,
        RBT_WALK_LEVELORDER = 0x04,
        RBT_WALK_RAWNODE    = 0x10
} rbt_walk_t;

#define RBT_WALK_TYPEMASK 0x0f
#define RBT_WALK_FLAGMASK 0xf0

/**
 * Generic node structure
 * Lowest bit of _chld[0] holds the color bit
 */
struct rbt_node {
        struct rbt_node *_chld[2];
        uint8_t          _node[];
};

#define RBT_NODE_CB 0
#define RBT_NODE_CR 1
#define RBT_NODE_SL 0
#define RBT_NODE_SR 1

#define rbt_node_ptr(np) ((struct rbt_node *)((uintptr_t)(np)&(UINTPTR_MAX << 1)))
#define rbt_node_setptr(dst,src) (dst) = (struct rbt_node *)((uintptr_t)rbt_node_ptr(src)|((uintptr_t)(dst)&1))

#define rbt_node_setcolor(np, cb)                                       \
        do {                                                            \
                register struct rbt_node *__n = rbt_node_ptr(np);       \
                register uint8_t          __c = (cb) & 1;               \
                                                                        \
                if (__n != NULL) {                                      \
                        if (__c) __n->_chld[0] = (struct rbt_node *)((uintptr_t)(__n->_chld[0]) | 1); \
                        else     __n->_chld[0] = rbt_node_ptr(__n->_chld[0]); \
                }                                                       \
        } while(0)
#define rbt_node_getcolor_raw(cp) ((uintptr_t)(cp) & 1)
#define rbt_node_getcolor(np) (rbt_node_ptr(np) == NULL ? RBT_NODE_CB : rbt_node_getcolor_raw(rbt_node_ptr(np)->_chld[0]))
#define rbt_node_cpycolor(dn, sn) rbt_node_setcolor((dn), rbt_node_getcolor(sn))

#define rbt_hpush4(__a, __p)                    \
        do {                                    \
                __a[3] = __a[2];                \
                __a[2] = __a[1];                \
                __a[1] = __a[0];                \
                __a[0] = __p;                   \
        } while(0)

#define rbt_hpush3(__a, __p)                    \
        do {                                    \
                __a[2] = __a[1];                \
                __a[1] = __a[0];                \
                __a[0] = __p;                   \
        } while(0)

#define rbt_redfix(__h, __d, v)                                         \
        do {                                                            \
                if (((__d) & 3) < 2) {                                  \
                        if (((__d) & 3) == 0) {                         \
                                rbt_node_setptr(v, rbt_node_rotate_R(__h[2])); \
                        } else {                                        \
                                rbt_node_setptr(v, rbt_node_rotate_RL(__h[2])); \
                        }                                               \
                } else {                                                \
                        if (((__d) & 3) == 2) {                         \
                                rbt_node_setptr(v, rbt_node_rotate_LR(__h[2])); \
                        } else {                                        \
                                rbt_node_setptr(v, rbt_node_rotate_L(__h[2])); \
                        }                                               \
                }                                                       \
        } while(0)

struct rbt {
        struct rbt_node *root;
        size_t           size;
        rbt_type_t       type;
#if defined(RBT_IMPLICIT_LOCKING)
        pthread_rwlock_t lock;
#endif
};

typedef struct rbt rbt_t;

/**
 * Create new tree
 * @param type type of key that will be used in the tree
 */
rbt_t *rbt_new(rbt_type_t type);

/**
 * Free resources used to hold information about the tree. This
 * function does not free memory pointed to by the data pointer
 * in a node.
 * @param rbt tree pointer
 */
void rbt_free(rbt_t *rbt, void (*callback)(void *));
void rbt_free2(rbt_t *rbt, void (*callback)(void *, void *), void *user);

/**
 * Lock a tree for reading
 * @param rbt tree pointer
 */
int rbt_rlock(rbt_t *rbt);

/**
 * Unlock a tree that was locked for reading before
 * @param rbt tree pointer
 */
void rbt_runlock(rbt_t *rbt);

/**
 * Lock a tree for writing (i.e. adding, deleting, replacing)
 * @param rbt tree pointer
 */
int rbt_wlock(rbt_t *rbt);

/**
 * Unlock a tree that was locked for writing before
 * @param rbt tree pointer
 */
void rbt_wunlock(rbt_t *rbt);

struct rbt_node *rbt_node_rotate_L(struct rbt_node *);
struct rbt_node *rbt_node_rotate_R(struct rbt_node *);
struct rbt_node *rbt_node_rotate_LR(struct rbt_node *);
struct rbt_node *rbt_node_rotate_RL(struct rbt_node *);

size_t rbt_size(rbt_t *rbt);

#define rbt_walk_push(n) stack[depth++] = (n)
#define rbt_walk_pop()   stack[--depth]
#define rbt_walk_top()   stack[depth-1]

int rbt_walk_preorder(rbt_t *rbt, int (*callback)(void *), rbt_walk_t flags);
int rbt_walk_inorder(rbt_t *rbt, int (*callback)(void *), rbt_walk_t flags);
int rbt_walk_inorder2(rbt_t *rbt, int (*callback)(void *, void *), void *user, rbt_walk_t flags);
int rbt_walk_postorder(rbt_t *rbt, int (*callback)(void *), rbt_walk_t flags);

#endif /* RBT_COMMON_H */
