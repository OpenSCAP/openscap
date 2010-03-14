#include <stdio.h>
#include <string.h>
#include <spb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define BUFNUM 1024
#define BUFLEN_MIN 1
#define BUFLEN_MAX 1024

int main (int argc, char *argv[])
{
        struct iovec iov[BUFNUM];
        void  *r_buf;
        size_t r_len;
        unsigned long seed;
       
        spb_t *spb;
                
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
                                memset (r_buf + r_len,   (int)(i % 256), iov[i].iov_len);
                                r_len += iov[i].iov_len;
                        }
        }
        
        spb = spb_new (NULL, 0, 0);
        spb_free (spb);
        
        spb = spb_new (NULL, 0, 1);
        spb_free (spb);

        spb = spb_new (NULL, 0, 10);
        spb_free (spb);
        
        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 0);
        spb_free (spb);
        
        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 1);
        spb_free (spb);
        
        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 10);
        spb_free (spb);

        spb = spb_new (NULL, 0, 0);
        for (int add = 0; add < BUFNUM; ++add) {
                if (spb_add (spb, iov[add].iov_base, iov[add].iov_len) != 0) {
                        abort ();
                }
        }
        spb_free (spb);

        spb = spb_new (iov[0].iov_base, iov[0].iov_len, 0);
        for (int add = 1; add < BUFNUM; ++add) {
                if (spb_add (spb, iov[add].iov_base, iov[add].iov_len) != 0) {
                        abort ();
                }                
        }
        spb_free (spb);
        
        spb = spb_new (NULL, 0, 0);
        for (int add = 0; add < BUFNUM; ++add) {
                if (spb_add (spb, iov[add].iov_base, iov[add].iov_len) != 0) {
                        abort ();
                }
        }
        
        fprintf (stdout, "spb size= %zu\n", spb_size (spb));
        
        {
                size_t  i = 0;
                uint8_t b;
                
                spb_iterate (spb, 0, b,
                             if (b != *(((uint8_t *)r_buf) + i)) {
                                     fprintf (stderr,
                                              "iterate: difference at position %zu: %hhu != %hhu",
                                              b, *(((uint8_t *)r_buf) + i));
                                     abort ();
                             } else {
                                     fprintf (stdout, "%c", i&1?((i&2)?'_':'^'):(i&2?'\\':'/'));
                             }
                             ++i;
                        );

                fprintf (stdout, "\n");
                
                if (i != spb_size(spb)) {
                        fprintf (stderr,
                                 "iterate: iteration ended before the actual buffer end: i=%zu, size=%zu\n",
                                 i, spb_size (spb));
                        abort ();
                }
        }
        
        spb_free (spb);

        spb = spb_new (NULL, 0, 10);
        {
                uint8_t b;
                spb_iterate (spb, 0, b, abort ());
        }
        spb_free (spb);
                
        return (0);
}
