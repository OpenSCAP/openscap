
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <spb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "../../assume.h"

#define BUFNUM 256
#define BUFLEN_MIN 1
#define BUFLEN_MAX 253

int main (int argc, char *argv[])
{
        struct iovec iov[BUFNUM];
        void  *r_buf;
        size_t r_len;
        unsigned long seed;
       
        spb_t *spb;

        setbuf (stdout, NULL);
        setbuf (stderr, NULL);
                
        switch (argc) {
        case 1:
                seed = ((unsigned long) time (NULL)) ^ ((unsigned long) getpid ());
                break;
        case 2:
                seed = strtoul (argv[1], NULL, 10);
                switch (errno) {
                case ERANGE:
                case EINVAL:
                        fprintf (stderr, "Invalid seed: Not a number\n");
                        return (-1);
                }
                break;
        default:
                fprintf (stderr, "Usage: %s [<seed>]\n", argv[0]);
                return (-1);
        }
        
        fprintf (stdout, "Seed = %lu\n", seed);
        srandom (seed);
        
        r_buf = malloc (BUFNUM * BUFLEN_MAX);
        r_len = 0;
        
        if (r_buf == NULL) {
                perror ("r_buf alloc");
                return (1);
        }

        { size_t i;
                        for (i = 0; i < BUFNUM; ++i) {
                                iov[i].iov_len  = BUFLEN_MIN + (random () % BUFLEN_MAX);
                                iov[i].iov_base = malloc (iov[i].iov_len);
                
                                if (iov[i].iov_base == NULL) {
                                        perror ("iov alloc");
                                        return (2);
                                }
                
                                memset (iov[i].iov_base, (int)(i % 256), iov[i].iov_len);
                                memset (((char *)r_buf) + r_len,   (int)(i % 256), iov[i].iov_len);
                                r_len += iov[i].iov_len;
                        }
        }
        
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (NULL, 0, 0);
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (NULL, 0, 1);
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (NULL, 0, 10);
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 0);
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 1);
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 10);
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (NULL, 0, 0);
        for (int add = 0; add < BUFNUM; ++add) {
                if (spb_add (spb, iov[add].iov_base, iov[add].iov_len) != 0) {
                        abort ();
                }
        }
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 0);
        for (int add = 1; add < BUFNUM; ++add) {
                if (spb_add (spb, iov[add].iov_base, iov[add].iov_len) != 0) {
                        abort ();
                }                
        }
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////
        spb = spb_new (NULL, 0, 0);
        for (int add = 0; add < BUFNUM; ++add) {
                if (spb_add (spb, iov[add].iov_base, iov[add].iov_len) != 0) {
                        abort ();
                }
        }
        
        {
                spb_size_t s_idx, b_size;
                uint32_t   b_idx, b_idx2;
                
                b_idx  = 0;
                b_size = spb_size (spb);

                for (s_idx = 0; s_idx < b_size; ++s_idx) {
                        b_idx2 = spb_bindex (spb, s_idx);
                        
                        if (b_idx2 < b_idx) abort ();
                        else b_idx = b_idx2;
                }

                if (b_idx != BUFNUM - 1) abort ();
                /* out of range s_idx */
                if (spb_bindex (spb, s_idx) < BUFNUM) abort ();
        }
        
        fprintf (stdout, "spb size= "SPB_SZ_FMT"\n", spb_size (spb));
        {
                size_t  i = 0;
                uint8_t b;
                
                spb_iterate (spb, 0, b,
                             if (b != *(((uint8_t *)r_buf) + i)) {
                                     fprintf (stderr,
                                              "iterate: difference at position %zu: %hhu != %hhu",
                                              i, b, *(((uint8_t *)r_buf) + i));
                                     abort ();
                             } else {
                                     fprintf (stdout, "%c", i&1?((i&2)?'_':'^'):(i&2?'\\':'/'));
                             }
                             ++i;
                        );

                fprintf (stdout, "\n");
                
                if (i != spb_size(spb)) {
                        fprintf (stderr,
                                 "iterate: iteration ended before the actual buffer end: i=%zu, size="SPB_SZ_FMT"\n",
                                 i, spb_size (spb));
                        abort ();
                }
        }        
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 0);
        for (int add = 1; add < BUFNUM; ++add) {
                if (spb_add (spb, iov[add].iov_base, iov[add].iov_len) != 0) {
                        abort ();
                }                
        }

        fprintf (stdout, "spb size= "SPB_SZ_FMT"\n", spb_size (spb));
        {
                size_t  i = 0;
                uint8_t b;
                
                spb_iterate (spb, 0, b,
                             if (b != *(((uint8_t *)r_buf) + i)) {
                                     fprintf (stderr,
                                              "iterate: difference at position %zu: %hhu != %hhu",
                                              i, b, *(((uint8_t *)r_buf) + i));
                                     abort ();
                             } else {
                                     fprintf (stdout, "%c", i&1?((i&2)?'_':'^'):(i&2?'\\':'/'));
                             }
                             ++i;
                        );

                fprintf (stdout, "\n");
                
                if (i != spb_size(spb)) {
                        fprintf (stderr,
                                 "iterate: iteration ended before the actual buffer end: i=%zu, size="SPB_SZ_FMT"\n",
                                 i, spb_size (spb));
                        abort ();
                }
        }        
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (NULL, 0, 10);
        {
                uint8_t b;
                spb_iterate (spb, 0, b, abort ());
        }
        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////        
        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 0);
        for (int add = 1; add < BUFNUM; ++add) {
                if (spb_add (spb, iov[add].iov_base, iov[add].iov_len) != 0) {
                        abort ();
                }                
        }
                
        {
                spb_size_t pick_size;
                spb_size_t pick_start;
                spb_size_t buf_size;
                void *p_buf;
                int p_ret, i;
                spb_size_t o;
                
                buf_size = spb_size (spb);
                p_buf = malloc (buf_size);
                
                for (pick_start = 0; pick_start < (buf_size - 1); ++pick_start) {
                        for (i = 0; i < 8; ++i) {
                                pick_size = 1 + (random () % (buf_size - pick_start));
                                
                                p_ret = spb_pick (spb, pick_start, pick_size, p_buf);
                                if (p_ret != 0) abort ();
                                
                                o = 0;
                                for (o = 0; o < pick_size; ++o) {
                                        if (*(uint8_t *)(((char *)p_buf) + o) != *(uint8_t *)(((char *)r_buf) + pick_start + o))
                                                abort ();
                                }
                        }
                        fprintf (stdout, "\r%c", pick_start&1?(pick_start&2?'/':'\\'):(pick_start&2?'|':'-'));
                }
                free (p_buf);
        }
        spb_free (spb, 0);
        fprintf (stdout, "\n");

        //////////////////////////////////////////////////////////////////////////
        { size_t i;
                for (i = 0; i < BUFNUM; ++i) {
                        free (iov[i].iov_base);
                }
        }
        free (r_buf);
        //////////////////////////////////////////////////////////////////////////
        uint8_t b1[8], b2[16], b3[32], b4[64];

        spb = spb_new (NULL, 0, 0);
        spb_add (spb, b1, 8);
        spb_add (spb, b2, 16);
        spb_add (spb, b3, 32);
        spb_add (spb, b4, 64);        

        assume (spb_size (spb) == 8+16+32+64);
        assume (spb_drop_head (spb, 4, 0) == 0);
        assume (spb_size (spb) == 8+16+32+64);
        assume (spb_drop_head (spb, 8, 0) == 8);
        assume (spb_size (spb) == 16+32+64);
        assume (spb_drop_head (spb, 17, 0) == 16);
        assume (spb_size (spb) == 32+64);
        assume (spb_drop_head (spb, 31, 0) == 0);
        assume (spb_size (spb) == 32+64);
        assume (spb_drop_head (spb, 32+64, 0) == 32+64);
        assume (spb_size (spb) == 0);

        spb_free (spb, 0);
        //////////////////////////////////////////////////////////////////////////

        return (0);
}
