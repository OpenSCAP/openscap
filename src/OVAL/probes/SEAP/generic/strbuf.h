#pragma once
#ifndef STRBUF_H
#define STRBUF_H

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

struct strblk {
        struct strblk *next;
        size_t         size;
        char           data[];
};

typedef struct {
        struct strblk *beg;
        struct strblk *lbo;
        size_t         blkmax;
        size_t         blkoff;
        size_t         size;
} strbuf_t;

strbuf_t *strbuf_new  (size_t max);
void      strbuf_free (strbuf_t *buf);

int strbuf_add   (strbuf_t *buf, const char *str, size_t len);
int strbuf_addf  (strbuf_t *buf, char *str, size_t len);
int strbuf_add0  (strbuf_t *buf, const char *str);
int strbuf_add0f (strbuf_t *buf, char *str);

int strbuf_trunc  (strbuf_t *buf, ssize_t len);
size_t strbuf_length (strbuf_t *buf);

char *strbuf_cstr   (strbuf_t *buf);
char *strbuf_cstr_r (strbuf_t *buf, char *str, size_t len);

size_t strbuf_fwrite (FILE *fp, strbuf_t *buf);
size_t strbuf_write  (strbuf_t *buf, int fd);

#endif /* STRBUF_H */
