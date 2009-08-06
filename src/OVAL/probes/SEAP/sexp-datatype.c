#include <string.h>
#include <errno.h>
#include "public/sm_alloc.h"
#include "_sexp-datatype.h"

RBNODECMP(datatype)
{
        return (strcmp (a->datatype.name, b->datatype.name));
}

RBNODEJOIN(datatype)
{
        (void)b;
        return (a);
}

RBTREECODE(datatype);

SEXP_datatypetbl_t g_datatypes = { {NULL, 0} };

SEXP_datatypetbl_t *SEXP_datatypetbl_new ()
{
        SEXP_datatypetbl_t *t;
        
        t = sm_talloc (SEXP_datatypetbl_t);
        t->tree.root = NULL;
        t->tree.size = 0;

        return (t);
}

int SEXP_datatypetbl_init (SEXP_datatypetbl_t *t)
{
        _A(t != NULL);
        
        t->tree.root = NULL;
        t->tree.size = 0;
        
        return (0);
}

SEXP_datatype_t *SEXP_datatype_get (SEXP_datatypetbl_t *t, const char *name)
{
        NODETYPE(datatype) *node, key;

        _A(t != NULL);
        _A(name != NULL);

        key.datatype.name = (char *)name;
        node = RB_SEARCH(datatype)(&(t->tree), &key);
        
        if (node == NULL) {
                _D("FAIL: datatype not found: t=%p, name=%s.\n", t, name);
                return (NULL);
        }
        
        return (&(node->datatype));
}

SEXP_datatype_t *SEXP_datatype_add (SEXP_datatypetbl_t *t, SEXP_datatype_t *datatype)
{
        NODETYPE(datatype) *node;

        _A(t != NULL);
        _A(datatype != NULL);

        node = RB_NEWNODE(datatype)();
        memcpy (&(node->datatype), datatype, sizeof (SEXP_datatype_t));
        
        if (RB_INSERT(datatype)(&(t->tree), node) != E_OK) {
                _D("FAIL: Can't add datatype: t=%p, name=%s.\n", t, datatype->name);
                sm_free (node);
                return (NULL);
        }
        
        _D("New datatype: name=%s.\n", datatype->name);
        return (&(node->datatype));
}

int SEXP_datatype_del (SEXP_datatypetbl_t *t, const char *name)
{
        errno = EOPNOTSUPP;
        return (-1);
}
