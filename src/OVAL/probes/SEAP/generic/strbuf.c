#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "strbuf.h"

strbuf_t *strbuf_new  (size_t max)
{
        strbuf_t *buf;

        buf = malloc (sizeof (strbuf_t));
        
        if (buf != NULL) {
                buf->beg = NULL;
                buf->lbo = NULL;
                buf->blkmax = max;
                buf->blkoff = 0;
                buf->size   = 0;
        }
        
        return (buf);
}

void strbuf_free (strbuf_t *buf)
{
        struct strblk *blk, *next;
        
        next = buf->beg;
        
        while (next != NULL) {
                blk  = next;
                next = next->next;
                free (blk);
        }
        
        memset (buf, 0, sizeof (strbuf_t));
        free (buf);
        return;
}

static struct strblk *__strblk_new (size_t len)
{
        struct strblk *blk;
        
        blk = malloc (sizeof (struct strblk *) +
                      sizeof (size_t) +
                      (sizeof (char) * len));
        blk->next = NULL;
        blk->size = 0;
        
        return (blk);
}

static int __strbuf_add (strbuf_t *buf, char *str, size_t len)
{
        struct strblk *cur, *prev;
        size_t cpylen;
        
        if (buf->beg != NULL)
                cur = buf->beg;
        else {
                cur = buf->beg = malloc (sizeof (struct strblk *) +
                                         sizeof (size_t) +
                                         (sizeof (char) * buf->blkmax));
                if (cur == NULL)
                        return (-1);
        }
        
        prev = buf->lbo;
        
        for (;;) {
                cpylen = len % (buf->blkoff + 1);
                memcpy (cur->data + buf->blkoff, str, sizeof (char) * cpylen);
                buf->size += cpylen;
                
                len -= cpylen;
                str += cpylen;

                if (len == 0)
                        break;
                else {
                        cur->next = __strblk_new (buf->blkmax);
                        prev = cur;
                        cur  = cur->next;
                        buf->blkoff = 0;
                }
        }
        
        buf->lbo = prev;
        
        return (0);
}

int strbuf_add (strbuf_t *buf, const char *str, size_t len)
{
        return __strbuf_add (buf, (char *)str, len);
}

int strbuf_addf (strbuf_t *buf, char *str, size_t len)
{
        int ret;
        
        ret = __strbuf_add (buf, str, len);
        
        if (ret == 0) {
                free (str);
                return (0);
        } else
                return (ret);
}

int strbuf_add0 (strbuf_t *buf, const char *str)
{
        return __strbuf_add (buf, (char *)str, strlen (str));
}

int strbuf_add0f (strbuf_t *buf, char *str)
{
        int ret;

        ret = __strbuf_add (buf, str, strlen (str));

        if (ret == 0) {
                free (str);
                return (0);
        }

        return (ret);
}

int strbuf_trunc (strbuf_t *buf, ssize_t len)
{
        return (-1);
}

size_t strbuf_length (strbuf_t *buf)
{
        return (buf->size);
}

char *strbuf_cstr (strbuf_t *buf)
{
        struct strblk *cur;
        char  *stroff, *strbeg;
        
        strbeg = malloc (sizeof (char) * buf->size);
        stroff = strbeg;
        
        if (strbeg == NULL)
                return (NULL);
        
        cur = buf->beg;

        while (cur != NULL) {
                memcpy (stroff, cur->data, sizeof (char) * cur->size);
                stroff += cur->size;
                cur = cur->next;
        }
        
        return (strbeg);
}

char *strbuf_cstr_r (strbuf_t *buf, char *str, size_t len)
{
        struct strblk *cur;
        char *stroff;

        if ((len - 1) < buf->size) {
                errno = ENOBUFS;
                return (NULL);
        }
        
        stroff = str;
        cur    = buf->beg;
        
        while (cur != NULL) {
                memcpy (stroff, cur->data, sizeof (char) * cur->size);
                stroff += cur->size;
                cur = cur->next;
        }
        
        return (str);
}
