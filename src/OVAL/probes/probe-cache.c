#ifndef __STUB_PROBE
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
        return (SEXP_string_cmp (a->id, b->id));
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
        new->id   = SEXP_ref (id);
        new->item = SEXP_ref (item);
        
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

        return (node == NULL ? NULL : SEXP_ref (node->item));
}

SEXP_t *pcache_cstr_get (pcache_t *cache, const char *id)
{
        NODETYPE(pcache) key, *node;
        
        _A(cache != NULL);
        _A(id != NULL);

        key.id = SEXP_string_new (id, strlen (id));
        node = RB_SEARCH(pcache)(&(cache->tree), &key);
        SEXP_free (key.id);
        
        return (node == NULL ? NULL : SEXP_ref (node->item));
}

#if 0
ncache_t *ncache_new  (void)
{
        ncache_t *c;

        c = oscap_talloc (ncache_t);
        c->count = 0;
        
        switch (pthread_rwlock_init (&c->lock, NULL)) {
        case 0:
                break;
        default:
                _D("Can't initialize rwlock: %u, %s.\n", errno, strerror (errno));
                oscap_free (c);
                
                return (NULL);
        }
        
        return (c);
}

void ncache_free (ncache_t *cache)
{
        if (cache == NULL)
                return;

        if (cache->count > 0) {
                ncache_item_t *i;
                uint16_t       c;
                
                for (; i_beg != i_end; i_beg = (i_beg + 1) % NCACHE_MAX_ITEMS) {
                        i = items + i_beg;
                        
                        while (i->count > 0) {
                                SEXP_free  (i->names + i->count);
                                oscap_free (i->iname);
                                
                                --(i->count);
                        }
                }
        }
        
        switch (pthread_rwlock_destroy (cache->lock)) {
        case 0:
                break;
        default:
                _D("Can't destroy rwlock: %u, %s.\n", errno, strerror (errno));
        }
        
        oscap_free (cache);
        return;
}

int ncache_item_add (ncache_t *cache, const char *iname, SEXP_t *names, uint16_t count)
{
        return (-1);
}

int ncache_name_add (ncache_t *cache, const char *iname, SEXP_t *name)
{
        return (-1);
}

int ncache_item_exists (ncache_t *cache, const char *iname)
{
        return (-1);
}

int ncache_name_exists (ncache_t *cache)
{
        return (-1);
}
#endif /* 0 */

#endif
