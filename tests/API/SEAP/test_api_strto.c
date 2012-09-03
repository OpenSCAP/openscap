#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include "strto.h"
#include "strto.c"

int main(void)
{
  uint8_t num_out, num_exp;
  char *input;
  size_t input_len;

#define TEST_strto_uint8_hex(input, expected_result) do { \
    if (strto_uint8_hex(input, strlen(input), NULL) != (expected_result)) {\
      fprintf(stderr, "strto_uint8_hex(%s) != %"PRIu8"\n", input, expected_result);\
      return 1;\
    }\
  } while (0)

  TEST_strto_uint8_hex("0", 0);
  TEST_strto_uint8_hex("1", 1);
  TEST_strto_uint8_hex("ff", 255);
  TEST_strto_uint8_hex("FF", 255);
  TEST_strto_uint8_hex("Ff", 255);
  TEST_strto_uint8_hex("fF", 255);
  TEST_strto_uint8_hex("10", 16);
  TEST_strto_uint8_hex("11", 17);
  TEST_strto_uint8_hex("a", 10);
  TEST_strto_uint8_hex("0a", 10);
  TEST_strto_uint8_hex("00", 0);
  TEST_strto_uint8_hex("01", 1);

  return 0;
}
