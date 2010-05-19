#ifndef SEXP_TEMPLATE
#define SEXP_TEMPLATE

SEXP_t *SEXP_template_new(const char *tplstr);
void    SEXP_template_free(SEXP_t *tpl);
SEXP_t *SEXP_template_fill(SEXP_t *tpl, ...);
SEXP_t *SEXP_template_safefill(SEXP_t *tpl, int argc, ...);

#endif /* SEXP_TEMPLATE */
