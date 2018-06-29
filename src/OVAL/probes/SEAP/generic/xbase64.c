/*
 * Copyright 2009 Red Hat Inc., Durham, North Carolina.
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>

#include "xbase64.h"

#if defined(WANT_XBASE64)
static const char xb64_enc_alphabet[64] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ:_";
static const char xb64_dec_alphabet[75] = {
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 62,  0,  0,  0,  0,  0, 0, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
        59, 60, 61,  0,  0,  0,  0, 63,  0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
};

#define B(n) (*(in+(n)))
static inline void __xb64_enc3 (const uint8_t in[3], char out[4]) {
        out[0] = xb64_enc_alphabet[ B(0) & 0x3f];
        out[1] = xb64_enc_alphabet[(B(0) & 0xc0) >> 2 | (B(1) & 0x0f)];
        out[2] = xb64_enc_alphabet[(B(1) & 0xf0) >> 2 | (B(2) & 0x03)];
        out[3] = xb64_enc_alphabet[(B(2) & 0xfc) >> 2];
        return;
}

static inline void __xb64_enc2 (const uint8_t in[2], char out[4]) {
        out[0] = xb64_enc_alphabet[ B(0) & 0x3f];
        out[1] = xb64_enc_alphabet[(B(0) & 0xc0) >> 2 | (B(1) & 0x0f)];
        out[2] = xb64_enc_alphabet[(B(1) & 0xf0) >> 2];
        out[3] = XBASE64_PADDING_CHAR;
        return;
}

static inline void __xb64_enc1 (const uint8_t in, char out[4]) {
        out[0] = xb64_enc_alphabet[ in & 0x3f];
        out[1] = xb64_enc_alphabet[(in & 0xc0) >> 2];
        out[2] = XBASE64_PADDING_CHAR;
        out[3] = XBASE64_PADDING_CHAR;
        return;
}
#undef  B

#define B(n) ((*(in+(n)) - 48) % 75)
static inline void __xb64_dec4 (const char in[4], uint8_t out[3]) {
        out[0] =  xb64_dec_alphabet[B(0)]         | ((xb64_dec_alphabet[B(1)] & 0x30) << 2);
        out[1] = (xb64_dec_alphabet[B(1)] & 0x0f) | ((xb64_dec_alphabet[B(2)] & 0x3c) << 2);
        out[2] = (xb64_dec_alphabet[B(2)] & 0x03) | ((xb64_dec_alphabet[B(3)] & 0x3f) << 2);
        return;
}

static inline void __xb64_dec3 (const char in[3], uint8_t out[2]) {
        out[0] =  xb64_dec_alphabet[B(0)]         | ((xb64_dec_alphabet[B(1)] & 0x30) << 2);
        out[1] = (xb64_dec_alphabet[B(1)] & 0x0f) | ((xb64_dec_alphabet[B(2)] & 0x3c) << 2);
        return;
}

static inline void __xb64_dec2 (const char in[2], uint8_t out[1]) {
        out[0] =  xb64_dec_alphabet[B(0)]         | ((xb64_dec_alphabet[B(1)] & 0x30) << 2);
        return;
}
#undef B

size_t xbase64_encode (const uint8_t *data, size_t size, char **buffer) {
        register uint8_t    d = size % 3;
        register size_t  i, s = size - d;

	*buffer = malloc(sizeof(char) * (1 + (s/3 * 4) + (d != 0 ? 4 : 0)));

        for (i = 0; i < s/3; ++i) {
                __xb64_enc3 (data + (i * 3), (*buffer) + (i * 4));
        }

        switch (d) {
        case 2:
                __xb64_enc2 (data + (i * 3), (*buffer) + (i * 4));
                ++i;
                break;
        case 1:
                __xb64_enc1 (*(data + (i * 3)), (*buffer) + (i * 4));
                ++i;
                break;
        }

        (*buffer)[(i * 4)] = '\0';
        return (i * 4);
}

size_t xbase64_decode (const char *data, size_t size, uint8_t **buffer) {
        register size_t  i, s = size;
        register uint8_t d = 0;

        d = 4 - (size % 4);

        if (d == 4) {
                if (*(data + s - 1) == XBASE64_PADDING_CHAR) {
                        if (*(data + s - 2) == XBASE64_PADDING_CHAR) {
                                d = 2;
                        } else {
                                d = 1;
                        }

                        s -= 4;
                }
        } else {
                if (d == 3) {
                        *buffer = NULL;
                        return 0;
                }
        }

        fprintf(stderr, "d = %u\n", d);

	*buffer = malloc(sizeof(uint8_t) * ((s / 4 * 3) + d));

        for (i = 0; i < s/4; ++i) {
                __xb64_dec4 (data + (i * 4), (*buffer) + (i * 3));
        }

        switch (d) {
        case 2:
                __xb64_dec2 (data + (i * 4), (*buffer) + (i * 3));
                return (i * 3) + 1;
        case 1:
                __xb64_dec3 (data + (i * 4), (*buffer) + (i * 3));
                return (i * 3) + 2;
        default:
                return (i * 3);
        }
}
#endif /* WANT_XBASE64 */

#if defined(WANT_BASE64)
static const char b64_enc_alphabet[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char b64_dec_alphabet[75] = {
        /* 0 */ 52,
        /* 1 */ 53,
        /* 2 */ 54,
        /* 3 */ 55,
        /* 4 */ 56,
        /* 5 */ 57,
        /* 6 */ 58,
        /* 7 */ 59,
        /* 8 */ 60,
        /* 9 */ 61,
        /* : */ 0,
        /* ; */ 0,
        /* < */ 0,
        /* = */ 0,
        /* > */ 0,
        /* ? */ 0,
        /* @ */ 0,
        /* A */ 0,
        /* B */ 1,
        /* C */ 2,
        /* D */ 3,
        /* E */ 4,
        /* F */ 5,
        /* G */ 6,
        /* H */ 7,
        /* I */ 8,
        /* J */ 9,
        /* K */ 10,
        /* L */ 11,
        /* M */ 12,
        /* N */ 13,
        /* O */ 14,
        /* P */ 15,
        /* Q */ 16,
        /* R */ 17,
        /* S */ 18,
        /* T */ 19,
        /* U */ 20,
        /* V */ 21,
        /* W */ 22,
        /* X */ 23,
        /* Y */ 24,
        /* Z */ 25,
        /* [ */ 0,
        /* \ */ 0,
        /* ] */ 0,
        /* ^ */ 0,
        /* _ */ 0,
        /* ` */ 0,
        /* a */ 26,
        /* b */ 27,
        /* c */ 28,
        /* d */ 29,
        /* e */ 30,
        /* f */ 31,
        /* g */ 32,
        /* h */ 33,
        /* i */ 34,
        /* j */ 35,
        /* k */ 36,
        /* l */ 37,
        /* m */ 38,
        /* n */ 39,
        /* o */ 40,
        /* p */ 41,
        /* q */ 42,
        /* r */ 43,
        /* s */ 44,
        /* t */ 45,
        /* u */ 46,
        /* v */ 47,
        /* w */ 48,
        /* x */ 49,
        /* y */ 50,
        /* z */ 51
};

#define B(n) (*(in+(n)))
static inline void __b64_enc3 (const uint8_t in[3], char out[4]) {
        out[0] = b64_enc_alphabet[(B(0) & 0xfc) >> 2];
        out[1] = b64_enc_alphabet[(B(0) & 0x03) << 4 | (B(1) & 0xf0) >> 4];
        out[2] = b64_enc_alphabet[(B(1) & 0x0f) << 2 | (B(2) & 0xc0) >> 6];
        out[3] = b64_enc_alphabet[(B(2) & 0x3f)];
        return;
}

static inline void __b64_enc2 (const uint8_t in[2], char out[4]) {
        out[0] = b64_enc_alphabet[(B(0) & 0xfc) >> 2];
        out[1] = b64_enc_alphabet[(B(0) & 0x03) << 4 | (B(1) & 0xf0) >> 4];
        out[2] = b64_enc_alphabet[(B(1) & 0x0f) << 2];
        out[3] = BASE64_PADDING_CHAR;
        return;
}

static inline void __b64_enc1 (const uint8_t in, char out[4]) {
        out[0] = b64_enc_alphabet[(in & 0xfc) >> 2];
        out[1] = b64_enc_alphabet[(in & 0x03) << 4];
        out[2] = BASE64_PADDING_CHAR;
        out[3] = BASE64_PADDING_CHAR;
        return;
}
#undef  B

#define B(n) ((*(in+(n)) - 48) % 75)
static inline void __b64_dec4 (const char in[4], uint8_t out[3]) {
        out[0] =   b64_dec_alphabet[B(0)]         << 2  | ((b64_dec_alphabet[B(1)] & 0x30) >> 4);
        out[1] = ((b64_dec_alphabet[B(1)] & 0x0f) << 4) | ((b64_dec_alphabet[B(2)] & 0x3c) >> 2);
        out[2] = ((b64_dec_alphabet[B(2)] & 0x03) << 6) |   b64_dec_alphabet[B(3)];
        return;
}

static inline void __b64_dec3 (const char in[3], uint8_t out[2]) {
        out[0] =   b64_dec_alphabet[B(0)]         << 2  | ((b64_dec_alphabet[B(1)] & 0x30) >> 4);
        out[1] = ((b64_dec_alphabet[B(1)] & 0x0f) << 4) | ((b64_dec_alphabet[B(2)] & 0x3c) >> 2);
        return;
}

static inline void __b64_dec2 (const char in[2], uint8_t out[1]) {
        out[0] = ((b64_dec_alphabet[B(0)] & 0x03) << 6) |   b64_dec_alphabet[B(1)];
        return;
}
#undef B

size_t base64_encode (const uint8_t *data, size_t size, char **buffer) {
        register uint8_t    d = size % 3;
        register size_t  i, s = size - d;

	*buffer = malloc(sizeof(char) * (1 + (s / 3 * 4) + (d != 0 ? 4 : 0)));

        for (i = 0; i < s/3; ++i) {
                __b64_enc3 (data + (i * 3), (*buffer) + (i * 4));
        }

        switch (d) {
        case 2:
                __b64_enc2 (data + (i * 3), (*buffer) + (i * 4));
                ++i;
                break;
        case 1:
                __b64_enc1 (*(data + (i * 3)), (*buffer) + (i * 4));
                ++i;
                break;
        }

        (*buffer)[(i * 4)] = '\0';
        return (i * 4);
}

size_t base64_decode (const char *data, size_t size, uint8_t **buffer) {
        register size_t  i, s = size;
        register uint8_t d = 0;

        d = size % 4;

        if (d == 0) {
                if (*(data + s - 1) == BASE64_PADDING_CHAR) {
                        if (*(data + s - 2) == BASE64_PADDING_CHAR) {
                                d = 2;
                        } else {
                                d = 1;
                        }

                        s -= 4;
                }
        } else {
                if (d == 1) {
                        *buffer = NULL;
                        return 0;
                }
        }

	*buffer = malloc(sizeof(uint8_t) * ((s / 4 * 3) + d));

        for (i = 0; i < s/4; ++i) {
                __b64_dec4 (data + (i * 4), (*buffer) + (i * 3));
        }

        switch (d) {
        case 2:
                __b64_dec2 (data + (i * 4), (*buffer) + (i * 3));
                return (i * 3) + 1;
         case 1:
                __b64_dec3 (data + (i * 4), (*buffer) + (i * 3));
                return (i * 3) + 2;
         default:
                return (i * 3);
        }
}
#endif /* WANT_BASE64 */
