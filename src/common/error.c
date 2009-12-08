#include <pthread.h>
#include <string.h>
#include "public/alloc.h"
#include "_error.h"

static pthread_key_t       __key;
static pthread_once_t      __once = PTHREAD_ONCE_INIT;

static void oscap_errkey_init (void)
{
        (void) pthread_key_create (&__key, NULL);
}

static struct oscap_err_t *oscap_err_new (oscap_errfamily_t family, oscap_errcode_t code, const char *desc,
                                          const char *func, uint32_t line, const char *file)
{
        struct oscap_err_t *err;

        err = oscap_talloc (struct oscap_err_t);
        err->family = family;
        err->code   = code;
        err->desc   = (desc == NULL ? NULL : strdup (desc));
        err->func   = func;
        err->line   = line;
        err->file   = file;
        
        return (err);
}

void  __oscap_seterr (const char *file,
                      uint32_t    line,
                      const char *func,
                      oscap_errfamily_t  family,
                      oscap_errcode_t code,
                      const char     *desc)
{
        struct oscap_err_t *err;

        (void) pthread_once (&__once, oscap_errkey_init);
  
        oscap_clearerr ();
        err = oscap_err_new (family, code, desc, func, line, file);
        (void)pthread_setspecific (__key, err);
        
        return;
}

void oscap_clearerr(void)
{
        struct oscap_err_t *err;

        (void) pthread_once (&__once, oscap_errkey_init);
        
        err = pthread_getspecific (__key);
        (void)pthread_setspecific (__key, NULL);
        
        if (err != NULL) {
                if (err->desc != NULL)
                        oscap_free (err->desc);
                oscap_free (err);
        }
        
        return;
}

bool oscap_err(void)
{
        (void) pthread_once (&__once, oscap_errkey_init);
        return (pthread_getspecific (__key) != NULL);
}

oscap_errfamily_t oscap_err_family (void)
{
        struct oscap_err_t *err;
        
        (void) pthread_once (&__once, oscap_errkey_init);
        err = pthread_getspecific (__key);
        
        return (err == NULL ? 0 : err->family);
}

oscap_errcode_t oscap_err_code (void)
{
        struct oscap_err_t *err;
        
        (void) pthread_once (&__once, oscap_errkey_init);
        err = pthread_getspecific (__key);
        
        return (err == NULL ? 0 : err->code);
}

const char *oscap_err_desc (void)
{
        struct oscap_err_t *err;
        
        (void) pthread_once (&__once, oscap_errkey_init);
        err = pthread_getspecific (__key);
        
        return (err == NULL ? 0 : (const char *)err->desc);        
}
