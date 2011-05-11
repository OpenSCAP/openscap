#include <stdio.h>
#include <strbuf.h>
#include <string.h>
#include <sexp.h>

static int print_sexp (SEXP_t *s_exp)
{
        strbuf_t *sb;

        /*
         * print the S-exp in advanced format
         */
        printf ("a> ");
        SEXP_fprintfa (stdout, s_exp);
        printf ("\n");
        
        /*
         * print the S-exp in transport format
         */
        printf ("t> ");
        sb = strbuf_new (8);
        SEXP_sbprintf_t (s_exp, sb);
        strbuf_fwrite (stdout, sb);
        strbuf_free (sb);
        printf ("\n");

        return (0);
}

#if defined(__SVR4) && defined(__sun)

#define _GETLINE_BUFLEN 255

ssize_t mygetline(char **lineptr, size_t *n, FILE *stream) {
	int c;
	size_t alloced = 0;
	char *linebuf;

	if (*lineptr == NULL) {
		linebuf = malloc(sizeof(char) * (_GETLINE_BUFLEN + 1));
		alloced = _GETLINE_BUFLEN + 1;
	} else {
		linebuf = *lineptr;
		alloced = *n;
	}
	ssize_t linelen = 0;

	do {
		c = fgetc(stream);
		if (c == EOF) {
			break;
		}
		if (linelen >= alloced) {
			linebuf = realloc(linebuf, sizeof(char) * (alloced + _GETLINE_BUFLEN + 1));
			alloced += (_GETLINE_BUFLEN + 1);
		}
		*(linebuf + linelen) = (unsigned char)c;
		linelen++;
	} while (c != '\n');

	/* empty line means EOF or some other error */
	if (linelen == 0) {
		if (linebuf != NULL && *lineptr == NULL) {
			free(linebuf);
			linebuf = NULL;
		}
		linelen = -1;
		*n = alloced;
	} else {
		if (linebuf != NULL) {
			linebuf[linelen] = '\0';
		}
		*n = alloced;
		*lineptr = linebuf;
	}

	return linelen;
}
#endif

int main (int argc, char *argv[])
{
        char  *input;
        size_t inlen;
        
        SEXP_psetup_t *psetup;
        SEXP_pstate_t *pstate;
        SEXP_t *s_exp;

        setbuf (stdout, NULL);
        setbuf (stdin,  NULL);
        
        psetup = SEXP_psetup_new ();
        pstate = NULL;

        if (argc == 1) {
                while (!feof (stdin)) {
                        input = NULL;
                        inlen = 0;

#if defined(__FreeBSD__)
                        input = fgetln (stdin, &inlen);
#elif defined(__linux__) || defined(__GLIBC__)
                        getline (&input, &inlen, stdin);
#elif defined(__SVR4) && defined(__sun)
                        mygetline (&input, &inlen, stdin);
#else
# error "FIXME"
#endif
                        s_exp = SEXP_parse (psetup, input, inlen, &pstate);
                        
                        if (s_exp != NULL) {
                                _A(pstate == NULL);
                                print_sexp (s_exp);
                                SEXP_free (s_exp);
                        }
                        
                        /* FIXME: getline/fgetln leak */
                }
        } else {
                int i;
                
                for (i = 0; i < (argc - 1); ++i) {
                        s_exp = SEXP_parse (psetup, argv[i + 1], strlen (argv[i + 1]), &pstate);
                        
                        if (s_exp != NULL) {
                                _A(pstate == NULL);
                                print_sexp (s_exp);
                                
                                SEXP_free (s_exp);
                        }
                }
                
                if (pstate != NULL)
                        return (1);
        }
        
        SEXP_psetup_free (psetup);

        return (0);
}
