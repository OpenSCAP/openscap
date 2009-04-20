#include <stdio.h>
#include <stdint.h>

#if defined(WANT_XBASE64)
# ifndef XBASE64_H
# define XBASE64_H

# define XBASE64_PADDING_CHAR '_'

size_t xbase64_encode (const uint8_t *, size_t, char **);
size_t xbase64_decode (const char *, size_t, uint8_t **);

# endif /* XBASE64_H */
#endif /* WANT_XBASE64 */

#if defined(WANT_BASE64)
# ifndef BASE64_H
# define BASE64_H

# define BASE64_PADDING_CHAR '='

size_t base64_encode (const uint8_t *, size_t, char **);
size_t base64_decode (const char *, size_t, uint8_t **);

# endif /* BASE64_H */
#endif /* WANT_BASE64 */
