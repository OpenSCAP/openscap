/*
 * Copyright 2010 Red Hat Inc., Durham, North Carolina.
 * All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *      "Daniel Kopecek" <dkopecek@redhat.com>
 */
#ifndef SEXP_TEMPLATE
#define SEXP_TEMPLATE

SEXP_t *SEXP_template_new(const char *tplstr);
void    SEXP_template_free(SEXP_t *tpl);
SEXP_t *SEXP_template_fill(SEXP_t *tpl, ...);
SEXP_t *SEXP_template_safefill(SEXP_t *tpl, int argc, ...);

#endif /* SEXP_TEMPLATE */
