#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "xbase64.h"

int main (int argc, char *argv[])
{
        char  *buf;
        size_t len;
        
        if (argc != 3) {
                printf ("Usage: %s [bBxX] <string>\n", basename(argv[0]));
                exit (1);
        }

        switch (*argv[1]) {
        case 'x':
                len = xbase64_encode (argv[2], strlen (argv[2]), &buf);
                printf ("\"%.*s\"", len, buf);
                break;
        case 'b':
                len = base64_encode (argv[2], strlen (argv[2]), &buf);
                printf ("\"%.*s\"", len, buf);
                break;
        case 'X':
                len = xbase64_decode (argv[2], strlen (argv[2]), &buf);
                printf ("\"%.*s\"", len, buf);
                break;
        case 'B':
                len = base64_decode (argv[2], strlen (argv[2]), &buf);
                printf ("\"%.*s\"", len, buf);
                break;
        default:
                exit (1);
        }

        return 0;
}
