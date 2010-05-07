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
#ifndef RBT_STR_H
#define RBT_STR_H

#include "rbt_common.h"

struct rbt_str_node {
        char    *key;
        void    *data;
};

typedef struct rbt_str_node rbt_str_node_t;

#define rbt_str_node_key(np) (((struct rbt_str_node *)(rbt_node_ptr(np)->_node))->key)
#define rbt_str_node_data(np) (((struct rbt_str_node *)(rbt_node_ptr(np)->_node))->data)

rbt_t *rbt_str_new (void);
void rbt_str_free (rbt_t *rbt);
int rbt_str_free_cb (rbt_t *rbt, int (*callback)(struct rbt_str_node *));

int rbt_str_add(rbt_t *rbt, char *key, void *data);
void *rbt_str_rep(rbt_t *rbt, const char *key, void *data);
int rbt_str_del(rbt_t *rbt, const char *key, void **n);
int rbt_str_get(rbt_t *rbt, const char *key, void **data);

int rbt_str_walk_preorder(rbt_t *rbt, int (*callback)(struct rbt_str_node *));
int rbt_str_walk_inorder(rbt_t *rbt, int (*callback)(struct rbt_str_node *));
int rbt_str_walk_postorder(rbt_t *rbt, int (*callback)(struct rbt_str_node *));
int rbt_str_walk_levelorder(rbt_t *rbt, int (*callback)(struct rbt_str_node *));
int rbt_str_walk(rbt_t *rbt, rbt_walk_t type, int (*callback)(struct rbt_str_node *));
size_t rbt_str_size(rbt_t *rbt);

#endif /* RBT_STR_H */
