#include <seap.h>
#include <string.h>
#include <assert.h>
#include <common/alloc.h>
#include "probe-cache.h"

#ifndef _A
#define _A(x) assert (x)
#endif

RBNODECMP(pcache)
{
        return SEXP_string_cmp (a->id, b->id);
}

RBNODEJOIN(pcache)
{
        (void)b;
        return (a);
}

RBTREECODE(pcache);

pcache_t *pcache_new (void)
{
        pcache_t *cache;

        cache = oscap_talloc (pcache_t);
        cache->tree.root = NULL;
        cache->tree.size = 0;
        
        return (cache);
}

void pcache_free (pcache_t *cache)
{
        _A(cache != NULL);
        /* FIXME: free tree */
        oscap_free (cache);
        return;
}

int pcache_sexp_add (pcache_t *cache, const SEXP_t *id, SEXP_t *item)
{
        NODETYPE(pcache) *new;

        _A(cache != NULL);
        _A(id != NULL);
        _A(item != NULL);

        new = RB_NEWNODE(pcache)();
        new->id   = (SEXP_t *)SEXP_deepdup (id);
        new->item = item;
        
        if (RB_INSERT(pcache)(&(cache->tree), new) == E_OK) {
                return (0);
        } else {
                _D("Can't add item to cache: item=%p, id=%p.\n",
                   item, id);
                oscap_free (new);
                return (-1);
        }
        
        /* NOTREACHED */
        return (-1);
}

int pcache_cstr_add (pcache_t *cache, const char *id, SEXP_t *item)
{
        _A(cache != NULL);
        _A(id != NULL);
        _A(item != NULL);
        
        return (-1);
}

int pcache_sexp_del (pcache_t *cache, const SEXP_t *id)
{
        _A(cache != NULL);
        _A(id != NULL);
        
        return (-1);
}

int pcache_cstr_del (pcache_t *cache, const char *id)
{
        _A(cache != NULL);
        _A(id != NULL);
        
        return (-1);
}

SEXP_t *pcache_sexp_get (pcache_t *cache, const SEXP_t *id)
{
        NODETYPE(pcache) key, *node;
        
        _A(cache != NULL);
        _A(id != NULL);
        
        key.id = (SEXP_t *)id;
        node = RB_SEARCH(pcache)(&(cache->tree), &key);

        return (node == NULL ? NULL : node->item);
}

SEXP_t *pcache_cstr_get (pcache_t *cache, const char *id)
{
        NODETYPE(pcache) key, *node;
        
        _A(cache != NULL);
        _A(id != NULL);

        key.id = SEXP_string_new (id, strlen (id));
        node = RB_SEARCH(pcache)(&(cache->tree), &key);
        SEXP_free (key.id);
        
        return (node == NULL ? NULL : node->item);
}
