/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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

#pragma once
#ifndef REDBLACK_H
#define REDBLACK_H

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "../../../../common/util.h"


#ifndef NDEBUG
#  define _E(exp) exp
#else
#  define _E(exp) while(0)
#endif

#define XMALLOC  malloc
#define XREALLOC realloc
#define XCALLOC  calloc
#define XFREE    free

#define SIDE_LEFT ((side_t)0)
#define SIDE_RGHT ((side_t)1)

#define COLOR_BLK 0
#define COLOR_RED 1

#define  PREORDER 0
#define   INORDER 1
#define POSTORDER 2
#define LRTDLAYER 3
#define RLTDLAYER 4

#if !defined (E_OK)
# define E_OK   0
#endif
#if !defined (E_FAIL)
# define E_FAIL 1
#endif
#if !defined (E_COLL)
# define E_COLL 2
#endif

#define __NAME_PREFIX__ ___rb_
#define __TREETYPE_PREFIX rbtree_
#define __NODETYPE_PREFIX rbnode_
#define __NODETYPE_ATTRS__  __attribute__ ((packed))
#define __TREETYPE_ATTRS__  __attribute__ ((packed))

typedef uint8_t side_t;
typedef uint8_t color_t;

#define __MYCONCAT2(a, b) a ## b
#define __MYCONCAT3(a, b, c) a ## b ## c
#define CONCAT2(a, b) __MYCONCAT2(a, b)
#define CONCAT3(a, b, c) __MYCONCAT3(a, b, c)
#define EXPAND(a) a

#define TREETYPE(__t_name) struct CONCAT2(__TREETYPE_PREFIX, __t_name)
#define NODETYPE(__t_name) struct CONCAT2(__NODETYPE_PREFIX, __t_name)

/* Definition templates */
#define DEF_RBN_INITST(__t_name) int                  CONCAT3(__NAME_PREFIX__, __t_name, _initst) (TREETYPE(__t_name) *tree)

#define RBN_CREATE_NAME(__t_name) CONCAT3(__NAME_PREFIX__, __t_name, _create)
#define DEF_RBN_CREATE(__t_name) TREETYPE(__t_name) *  RBN_CREATE_NAME(__t_name) (void)
#define RB_CREATE RBN_CREATE_NAME

#define RBN_NEWNODE_NAME(__t_name) CONCAT3(__NAME_PREFIX__, __t_name, _newnode)
#define DEF_RBN_NEWNODE(__t_name) NODETYPE(__t_name) * RBN_NEWNODE_NAME(__t_name) (void)
#define RB_NEWNODE RBN_NEWNODE_NAME

#define RBN_INSERT_NAME(__t_name) CONCAT3(__NAME_PREFIX__, __t_name, _insert)
#define DEF_RBN_INSERT(__t_name) int RBN_INSERT_NAME(__t_name) (TREETYPE(__t_name) *tree, NODETYPE(__t_name) *new_node)
#define RB_INSERT RBN_INSERT_NAME

#define RBN_SEARCH_NAME(__t_name) CONCAT3(__NAME_PREFIX__, __t_name, _search)
#define DEF_RBN_SEARCH(__t_name) NODETYPE(__t_name) *  RBN_SEARCH_NAME(__t_name) (TREETYPE(__t_name) *tree, const NODETYPE(__t_name) *k_node)
#define RB_SEARCH RBN_SEARCH_NAME

#define DEF_RBN_DELETE(__t_name) int CONCAT3(__NAME_PREFIX__, __t_name, _delete) (TREETYPE(__t_name) *tree, const NODETYPE(__t_name) *k_node)

#define DEF_RBN_REMOVE(__t_name) DEF_RBN_DELETE(__t_name)

#define DEF_RBN_NODE_LEFT(__t_name) NODETYPE(__t_name) * CONCAT3(__NAME_PREFIX__, __t_name, _getLeftNode) (NODETYPE(__t_name) *node)
#define DEF_RBN_NODE_RGHT(__t_name) NODETYPE(__t_name) * CONCAT3(__NAME_PREFIX__, __t_name, _getRghtNode) (NODETYPE(__t_name) *node)
#define DEF_RBN_NODE_RIGHT(__t_name) RBN_NODE_RGHT(__t_name)

#define DEF_RBN_NODE_COLOR(__t_name) color_t CONCAT3(__NAME_PREFIX__, __t_name, _getColor) (NODETYPE(__t_name) *node)
#define DEF_RBN_NODE_SIDE(__t_name)  side_t  CONCAT3(__NAME_PREFIX__, __t_name, _getSide)  (NODETYPE(__t_name) *node)

#define DEF_RBN_ROT_R(__t_name)  NODETYPE(__t_name) * CONCAT3(__NAME_PREFIX__, __t_name, _rotR)  (NODETYPE(__t_name) *r)
#define DEF_RBN_ROT_L(__t_name)  NODETYPE(__t_name) * CONCAT3(__NAME_PREFIX__, __t_name, _rotL)  (NODETYPE(__t_name) *r)
#define DEF_RBN_ROT_RL(__t_name) NODETYPE(__t_name) * CONCAT3(__NAME_PREFIX__, __t_name, _rotRL) (NODETYPE(__t_name) *r)
#define DEF_RBN_ROT_LR(__t_name) NODETYPE(__t_name) * CONCAT3(__NAME_PREFIX__, __t_name, _rotLR) (NODETYPE(__t_name) *r)

#define ROTATE_ARR_NAME(__t_name) CONCAT3(__NAME_PREFIX__, __t_name, _rotate)

#define RBN_NODECMP_NAME(__t_name) CONCAT3(__NAME_PREFIX__, __t_name, _nodecmp)
#define DEF_RBN_NODECMP(__t_name) int RBN_NODECMP_NAME(__t_name) (const NODETYPE(__t_name) *a, const NODETYPE(__t_name) *b)
#define RBNODECMP DEF_RBN_NODECMP

#define RBN_NODEJOIN_NAME(__t_name) CONCAT3(__NAME_PREFIX__, __t_name, _nodejoin)
#define DEF_RBN_NODEJOIN(__t_name) NODETYPE(__t_name) * RBN_NODEJOIN_NAME(__t_name) (NODETYPE(__t_name) *a, NODETYPE(__t_name) *b)
#define RBNODEJOIN DEF_RBN_NODEJOIN

#define RBN_WALK_NAME(__t_name) CONCAT3(__NAME_PREFIX__, __t_name, _walk)
#define DEF_RBN_WALK(__t_name) void RBN_WALK_NAME(__t_name) (TREETYPE(__t_name) *tree, uint8_t order, void (*callback)(NODETYPE(__t_name) *, void *), void *cbarg)
#define RB_WALK RBN_WALK_NAME

#define RBN_CALLBACK_NAME(__t_name, myname) CONCAT3(__t_name, _CB_, myname)
#define DEF_RBN_CALLBACK(__t_name, myname) void RBN_CALLBACK_NAME(__t_name, myname) (NODETYPE(__t_name) *node, void *cbarg)
#define RBCBNAME RBN_CALLBACK_NAME
#define RBCALLBACK DEF_RBN_CALLBACK

#define NOARG 0

/* Main template */
#define DEFRBTREE(__t_name, __u_nitem)                                  \
        NODETYPE(__t_name) {						\
                /* META data */						\
                NODETYPE(__t_name) *___child[2];                        \
                color_t ___c: 1; /* Node color */                       \
                side_t  ___s: 1; /* Node side relative to parent */     \
                /* USER data */						\
                __u_nitem;                                              \
        } __NODETYPE_ATTRS__;                                           \
                                                                        \
        TREETYPE(__t_name) {						\
                NODETYPE(__t_name) *root;                               \
                size_t size;                                            \
        } __TREETYPE_ATTRS__;                                           \
                                                                        \
        DEF_RBN_NODEJOIN(__t_name);					\
        DEF_RBN_NODECMP(__t_name);					\
        DEF_RBN_CREATE(__t_name);					\
        DEF_RBN_NEWNODE(__t_name);					\
        DEF_RBN_WALK(__t_name);                                         \
        DEF_RBN_INSERT(__t_name);                                       \
        DEF_RBN_SEARCH(__t_name)
/*
  DEF_RBN_INITST(__t_name);					\
  DEF_RBN_SEARCH(__t_name, __keytype);				\
  DEF_RBN_DELETE(__t_name, __keytype);				\
  DEF_RBN_NODE_LEFT(__t_name);					\
  DEF_RBN_NODE_RGHT(__t_name);					\
  DEF_RBN_NODE_COLOR(__t_name);					\
  DEF_RBN_NODE_SIDE(__t_name)
*/

#define __isRED(n) ((n)->___c == COLOR_RED)
#define isRED(n) (((n) == NULL) ? 0 : __isRED(n))
#define PTRMOVE(next) {                         \
                ggp = grp;                      \
                grp = par;                      \
                par = cur;                      \
                cur = next; }

#define lch (cur->___child[SIDE_LEFT])
#define rch (cur->___child[SIDE_RGHT])

/* Code templates */
//#define RBN_INITST()

#define RBN_NEWNODE_CODE(__t_name)					\
        DEF_RBN_NEWNODE(__t_name) {                                     \
                NODETYPE(__t_name) *new;                                \
                new = XMALLOC(sizeof(NODETYPE(__t_name)));              \
                new->___s = 0;                                          \
                new->___c = 0;                                          \
                new->___child[0] = NULL;                                \
                new->___child[1] = NULL;                                \
                return (new);                                           \
        }

#define RBN_ROTATE_CODE(__t_name)					\
        static DEF_RBN_ROT_L(__t_name) {                                \
                register NODETYPE(__t_name) *nr;                        \
    									\
                nr = r->___child[SIDE_RGHT];                            \
                r->___child[SIDE_RGHT] = nr->___child[SIDE_LEFT];       \
                nr->___child[SIDE_LEFT] = r;                            \
    									\
                nr->___s = r->___s;                                     \
                r->___s = SIDE_LEFT;                                    \
    									\
                if (r->___child[SIDE_RGHT] != NULL)                     \
                        r->___child[SIDE_RGHT]->___s = SIDE_RGHT;       \
    									\
                if (nr->___child[SIDE_RGHT] != NULL)                    \
                        nr->___child[SIDE_RGHT]->___c = COLOR_BLK;      \
    									\
                return (nr);                                            \
        }                                                               \
  									\
        static DEF_RBN_ROT_R(__t_name) {                                \
                register NODETYPE(__t_name) *nr;                        \
    									\
                nr = r->___child[SIDE_LEFT];                            \
                r->___child[SIDE_LEFT] = nr->___child[SIDE_RGHT];       \
                nr->___child[SIDE_RGHT] = r;                            \
    									\
                nr->___s = r->___s;                                     \
                r->___s = SIDE_RGHT;                                    \
    									\
                if (r->___child[SIDE_LEFT] != NULL)                     \
                        r->___child[SIDE_LEFT]->___s = SIDE_LEFT;       \
    									\
                if (nr->___child[SIDE_LEFT] != NULL)                    \
                        nr->___child[SIDE_LEFT]->___c = COLOR_BLK;      \
    									\
                return (nr);                                            \
        }                                                               \
  									\
        static DEF_RBN_ROT_LR(__t_name) {                               \
                register NODETYPE(__t_name) *nr;                        \
    									\
                nr = r->___child[SIDE_RGHT]->___child[SIDE_LEFT];       \
                nr->___s = r->___s;                                     \
                r->___s = SIDE_LEFT;                                    \
                r->___child[SIDE_RGHT]->___child[SIDE_LEFT] = nr->___child[SIDE_RGHT]; \
    									\
                if (nr->___child[SIDE_RGHT] != NULL)                    \
                        nr->___child[SIDE_RGHT]->___s = SIDE_LEFT;      \
    									\
                nr->___child[SIDE_RGHT] = r->___child[SIDE_RGHT];       \
                r->___child[SIDE_RGHT] = nr->___child[SIDE_LEFT];       \
    									\
                if (nr->___child[SIDE_LEFT] != NULL)                    \
                        nr->___child[SIDE_LEFT]->___s = SIDE_RGHT;      \
    									\
                nr->___child[SIDE_LEFT] = r;                            \
                nr->___child[SIDE_RGHT]->___c = COLOR_BLK;              \
    									\
                return (nr);                                            \
        }                                                               \
  									\
        static DEF_RBN_ROT_RL(__t_name) {                               \
                register NODETYPE(__t_name) *nr;                        \
    									\
                nr = r->___child[SIDE_LEFT]->___child[SIDE_RGHT];       \
                nr->___s = r->___s;                                     \
                r->___s = SIDE_RGHT;                                    \
                r->___child[SIDE_LEFT]->___child[SIDE_RGHT] = nr->___child[SIDE_LEFT]; \
    									\
                if (nr->___child[SIDE_LEFT] != NULL)                    \
                        nr->___child[SIDE_LEFT]->___s = SIDE_RGHT;      \
									\
                nr->___child[SIDE_LEFT] = r->___child[SIDE_LEFT];       \
                r->___child[SIDE_LEFT] = nr->___child[SIDE_RGHT];       \
									\
                if (nr->___child[SIDE_RGHT] != NULL)                    \
                        nr->___child[SIDE_RGHT]->___s = SIDE_LEFT;      \
    									\
                nr->___child[SIDE_RGHT] = r;                            \
                nr->___child[SIDE_LEFT]->___c = COLOR_BLK;              \
    									\
                return (nr);                                            \
        }                                                               \
  									\
        static NODETYPE(__t_name) * (*ROTATE_ARR_NAME(__t_name)[4])(NODETYPE(__t_name) *) = { \
                &CONCAT3(__NAME_PREFIX__, __t_name, _rotR),             \
                &CONCAT3(__NAME_PREFIX__, __t_name, _rotLR),            \
                &CONCAT3(__NAME_PREFIX__, __t_name, _rotRL),            \
                &CONCAT3(__NAME_PREFIX__, __t_name, _rotL)              \
        }

#define RBN_CREATE_CODE(__t_name)					\
        DEF_RBN_CREATE(__t_name) {                                      \
                TREETYPE(__t_name) *new = XMALLOC (sizeof (TREETYPE(__t_name))); \
                new->root = NULL;                                       \
                new->size = 0;                                          \
                return (new);                                           \
        }						

#define RBN_INSERT_CODE(__t_name)					\
        DEF_RBN_INSERT(__t_name) {                                      \
                NODETYPE(__t_name) *cur, *par, *grp, *ggp;              \
                side_t lastdir;						\
                int cmp;                                                \
                NODETYPE(__t_name) fake;                                \
    									\
                fake.___c = COLOR_BLK;                                  \
                fake.___child[SIDE_RGHT] = tree->root;                  \
                fake.___child[SIDE_LEFT] = NULL;                        \
                ggp = grp = NULL;                                       \
                par = &fake;                                            \
                cur = tree->root;                                       \
                lastdir = SIDE_RGHT;                                    \
                                                                        \
                for (;;) {                                              \
                        /* Search loop BEGIN */                         \
                        if (cur == NULL) {                              \
                                par->___child[lastdir] = cur = new_node; \
                                cur->___s = lastdir;                    \
                                cur->___c =	COLOR_RED;              \
                                cur->___child[SIDE_LEFT] = cur->___child[SIDE_RGHT]; \
	    								\
                                if (__isRED(par)) /* red violation */   \
                                        ggp->___child[grp->___s] = ROTATE_ARR_NAME(__t_name)[(lastdir << 1)|(par->___s)](grp); \
	    								\
                                tree->root = fake.___child[SIDE_RGHT];  \
                                tree->root->___c = COLOR_BLK;           \
                                ++(tree->size);                         \
									\
                                return (E_OK);                          \
                        } else if (isRED(lch) && isRED(rch)) {          \
                                /* color switch */                      \
                                cur->___c = COLOR_RED;                  \
                                lch->___c = rch->___c = COLOR_BLK;      \
	    								\
                                if (__isRED(par)) /* red violation */   \
                                        ggp->___child[grp->___s] = ROTATE_ARR_NAME(__t_name)[(cur->___s << 1)|(par->___s)](grp); \
                        } else if (__isRED(par) && __isRED(cur)) {      \
                                /* red violation */                     \
                                ggp->___child[grp->___s] = ROTATE_ARR_NAME(__t_name)[(cur->___s << 1)|(par->___s)](grp); \
                        }                                               \
                                                                        \
                        cmp = RBN_NODECMP_NAME(__t_name) (cur, new_node); \
                                                                        \
                        if (cmp == 0) {                                 \
                                lastdir = cur->___s;                    \
                                _E(color_t tmp_c = cur->___c;)          \
                                _E(NODETYPE(__t_name) *tmp_l = cur->___child[SIDE_LEFT];) \
                                _E(NODETYPE(__t_name) *tmp_r = cur->___child[SIDE_RGHT];) \
                                tree->root = fake.___child[SIDE_RGHT]; \
                                tree->root->___c = COLOR_BLK;           \
                                RBN_NODEJOIN_NAME(__t_name) (cur, new_node); \
                                                                        \
                                assert(cur->___s == lastdir);           \
                                assert(cur->___c == tmp_c);             \
                                assert(cur->___child[SIDE_LEFT] == tmp_l); \
                                assert(cur->___child[SIDE_RGHT] == tmp_r); \
									\
                                return (E_COLL);                        \
                        } else if (cmp < 0) {                           \
                                /* go right */                          \
                                PTRMOVE(rch);                           \
                                lastdir = SIDE_RGHT;                    \
                        } else {                                        \
                                /* go left */                           \
                                PTRMOVE(lch);                           \
                                lastdir = SIDE_LEFT;                    \
                        }                                               \
                } /* Search loop END */                                 \
    									\
                abort ();                                               \
                return (E_FAIL);                                        \
        }

#define RBN_SEARCH_CODE(__t_name)                                       \
        DEF_RBN_SEARCH(__t_name) {                                      \
                NODETYPE(__t_name) *node;                               \
                int cmp;                                                \
                                                                        \
                node = tree->root;                                      \
                                                                        \
                while (node != NULL) {                                  \
                        cmp = RBN_NODECMP_NAME(__t_name) (node, k_node); \
                                                                        \
                        if (cmp == 0)                                   \
                                break;                                  \
                        else if (cmp < 0)                               \
                                node = node->___child[SIDE_RGHT];       \
                        else                                            \
                                node = node->___child[SIDE_LEFT];       \
                }                                                       \
                                                                        \
                return (node);                                          \
        }

#define __WALK_STACK_SIZE 32
#define __WALK_STACK_GROW 16

#define PUSH(n) node_stack[node_stack_count] = (n), ++node_stack_count
#define POP(n)  (n) = node_stack[--node_stack_count]
#define TOP     (node_stack[node_stack_count-1])
#define CNT      node_stack_count

#define RBN_WALK_CODE(__t_name)						\
        DEF_RBN_WALK(__t_name) {                                        \
                NODETYPE(__t_name) **node_stack;                        \
                register uint16_t    node_stack_size;                   \
                register uint16_t    node_stack_count;                  \
    									\
                node_stack_count = 0;                                   \
                node_stack_size  = __WALK_STACK_SIZE;                   \
                node_stack = XCALLOC(sizeof (NODETYPE(__t_name) *), node_stack_size); \
    									\
                PUSH(tree->root);                                       \
    									\
                switch (order) {                                        \
                case PREORDER:                                          \
                        while (CNT > 0) {                               \
                                callback (TOP, cbarg);                  \
                                if (TOP->___child[SIDE_LEFT] != NULL) { \
                                        PUSH(TOP->___child[SIDE_LEFT]); \
                                } else {                                \
                                __pre:                                  \
                                        if (TOP->___child[SIDE_RGHT] != NULL) { \
                                                TOP = TOP->___child[SIDE_RGHT]; \
                                        } else {                        \
                                                if (--CNT > 0)          \
                                                        goto __pre;     \
                                                else                    \
                                                        break;          \
                                        }                               \
                                }                                       \
                        }                                               \
                        break;                                          \
                case INORDER:                                           \
                        while (CNT > 0) {                               \
                                if (TOP->___child[SIDE_LEFT] != NULL) { \
                                        PUSH(TOP->___child[SIDE_LEFT]); \
                                } else {                                \
                                __in:                                   \
                                        callback (TOP, cbarg);          \
                                        if (TOP->___child[SIDE_RGHT] != NULL) { \
                                                TOP = TOP->___child[SIDE_RGHT]; \
                                        } else {                        \
                                                if (--CNT > 0)          \
                                                        goto __in;      \
                                                else                    \
                                                        break;          \
                                        }                               \
                                }                                       \
                        }                                               \
                        break;                                          \
                case POSTORDER:                                         \
                        break;                                          \
                default:                                                \
                        abort ();                                       \
                }                                                       \
                XFREE(node_stack);                                      \
                return;                                                 \
        }

/*
  #undef PUSH
  #undef POP
  #undef TOP
  #undef COUNT						
*/

/*
  #define RBN_DELETE()
  #define RBN_REMOVE() RBN_DELETE()
*/

/*
  #define RBN_NODE_LEFT()
  #define RBN_NODE_RGHT()
  #define RBN_NODE_RIGHT() RBN_NODE_RGHT()
  #define RBN_NODE_COLOR()
  #define RBN_NODE_SIDE()
*/

#define RBTREECODE(__t_name)					\
        RBN_CREATE_CODE(__t_name)                               \
        RBN_ROTATE_CODE(__t_name);                              \
        RBN_NEWNODE_CODE(__t_name)                              \
        RBN_SEARCH_CODE(__t_name)                               \
        RBN_INSERT_CODE(__t_name)                               \
        RBN_WALK_CODE(__t_name)					\
        static const char CONCAT2(__t_name, dummy) = 0


#endif /* REDBLACK_H */
