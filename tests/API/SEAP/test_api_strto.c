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

#define TEST_strto_uint16_hex(input, expected_result) do { \
    if (strto_uint16_hex(input, strlen(input), NULL) != (expected_result)) {\
      fprintf(stderr, "strto_uint16_hex(%s) != %"PRIu16"\n", input, expected_result);\
	return 1;\
    }\
  } while (0)

  TEST_strto_uint16_hex("0", 0);
  TEST_strto_uint16_hex("1", 1);
  TEST_strto_uint16_hex("ff", 255);
  TEST_strto_uint16_hex("FF", 255);
  TEST_strto_uint16_hex("Ff", 255);
  TEST_strto_uint16_hex("fF", 255);
  TEST_strto_uint16_hex("10", 16);
  TEST_strto_uint16_hex("11", 17);
  TEST_strto_uint16_hex("a", 10);
  TEST_strto_uint16_hex("0a", 10);
  TEST_strto_uint16_hex("00", 0);
  TEST_strto_uint16_hex("01", 1);

  TEST_strto_uint16_hex("fFfF", 0xffff);
  TEST_strto_uint16_hex("FFFF", 0xffff);
  TEST_strto_uint16_hex("ffff", 0xffff);
  TEST_strto_uint16_hex("aaaa", 0xaaaa);
  TEST_strto_uint16_hex("0101", 0x0101);
  TEST_strto_uint16_hex("1010", 0x1010);
  TEST_strto_uint16_hex("1111", 0x1111);
  TEST_strto_uint16_hex("0000", 0x0000);
  TEST_strto_uint16_hex("000", 0x000);
  TEST_strto_uint16_hex("8000", 0x8000);
  TEST_strto_uint16_hex("0", 0x0);
  TEST_strto_uint16_hex("1234", 0x1234);

  return 0;
}
