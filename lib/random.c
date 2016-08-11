/* Family of functions for random integers.
   Copyright (C) 1995-1997, 2000-2002, 2012-2016 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/*
 * This is derived from the Berkeley source:
 *      @(#)random.c    5.5 (Berkeley) 7/6/88
 * It was reworked for the GNU C Library by Roland McGrath.
 * Rewritten to use reentrant functions by Ulrich Drepper, 1995.
 */

/*
   Copyright (C) 1983 Regents of the University of California.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   4. Neither the name of the University nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
   SUCH DAMAGE.*/

#include <config.h>

/* Specification.  */
#include <stdlib.h>

#include <stdint.h>
#include <time.h>

/* This file can assume the 'struct random_data' and associated *_r functions
   from gnulib.  */

/* Linear congruential.  */
#define TYPE_0          0
#define BREAK_0         8
#define DEG_0           0
#define SEP_0           0

/* x**7 + x**3 + 1.  */
#define TYPE_1          1
#define BREAK_1         32
#define DEG_1           7
#define SEP_1           3

/* x**15 + x + 1.  */
#define TYPE_2          2
#define BREAK_2         64
#define DEG_2           15
#define SEP_2           1

/* x**31 + x**3 + 1.  */
#define TYPE_3          3
#define BREAK_3         128
#define DEG_3           31
#define SEP_3           3

/* x**63 + x + 1.  */
#define TYPE_4          4
#define BREAK_4         256
#define DEG_4           63
#define SEP_4           1

/* Array versions of the above information to make code run faster.
   Relies on fact that TYPE_i == i.  */

#define MAX_TYPES       5       /* Max number of types above.  */

/* Initially, everything is set up as if from:
        initstate(1, randtbl, 128);
   Note that this initialization takes advantage of the fact that srandom
   advances the front and rear pointers 10*rand_deg times, and hence the
   rear pointer which starts at 0 will also end up at zero; thus the zeroth
   element of the state information, which contains info about the current
   position of the rear pointer is just
        (MAX_TYPES * (rptr - state)) + TYPE_3 == TYPE_3.  */

static int32_t randtbl[DEG_3 + 1] =
  {
    TYPE_3,

    -1726662223, 379960547, 1735697613, 1040273694, 1313901226,
    1627687941, -179304937, -2073333483, 1780058412, -1989503057,
    -615974602, 344556628, 939512070, -1249116260, 1507946756,
    -812545463, 154635395, 1388815473, -1926676823, 525320961,
    -1009028674, 968117788, -123449607, 1284210865, 435012392,
    -2017506339, -911064859, -370259173, 1132637927, 1398500161,
    -205601318,
  };

static struct random_data generator =
  {
/* FPTR and RPTR are two pointers into the state info, a front and a rear
   pointer.  These two pointers are always rand_sep places aparts, as they
   cycle through the state information.  (Yes, this does mean we could get
   away with just one pointer, but the code for random is more efficient
   this way).  The pointers are left positioned as they would be from the call:
        initstate(1, randtbl, 128);
   (The position of the rear pointer, rptr, is really 0 (as explained above
   in the initialization of randtbl) because the state table pointer is set
   to point to randtbl[1] (as explained below).)  */

    /* fptr */ &randtbl[SEP_3 + 1],
    /* rptr */ &randtbl[1],

/* The following things are the pointer to the state information table,
   the type of the current generator, the degree of the current polynomial
   being used, and the separation between the two pointers.
   Note that for efficiency of random, we remember the first location of
   the state information, not the zeroth.  Hence it is valid to access
   state[-1], which is used to store the type of the R.N.G.
   Also, we remember the last location, since this is more efficient than
   indexing every time to find the address of the last element to see if
   the front and rear pointers have wrapped.  */

    /* state */ &randtbl[1],

    /* rand_type */ TYPE_3,
    /* rand_deg */ DEG_3,
    /* rand_sep */ SEP_3,

    /* end_ptr */ &randtbl[sizeof (randtbl) / sizeof (randtbl[0])]
};

long
random (void)
{
  int32_t val;

  if (random_r (&generator, &val) < 0)
    abort (); /* should not happen */
  return val;
}

void
srandom (unsigned int seed)
{
  (void) srandom_r (seed, &generator); /* may fail! */
}

char *
initstate (unsigned int seed, char *buf, size_t buf_size)
{
  char *old_state = (char *) ((int32_t *) generator.state - 1);
  if (initstate_r (seed, buf, buf_size, &generator) < 0)
    return NULL; /* failed */
  return old_state;
}

char *
setstate (char *arg_state)
{
  char *old_state = (char *) ((int32_t *) generator.state - 1);
  if (setstate_r (arg_state, &generator) < 0)
    return NULL; /* failed */
  return old_state;
}
