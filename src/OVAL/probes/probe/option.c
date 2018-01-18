#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdarg.h>
#include "option.h"

size_t probe_optdef_count = 0;
probe_option_t *probe_optdef = NULL;

static int __probe_option_op(int option, int op, va_list ap)
{
  probe_option_t* optdef = probe_optdef;
  size_t i, optdef_count = probe_optdef_count;
  int ret = -1;

  for (i = 0; i < optdef_count; ++i) {
	if (option == optdef[i].option) {
	  ret = optdef[i].handler(option, op, ap);
	}
  }

  return ret;
}

int probe_setoption(int option, ...)
{
  int ret;
  va_list ap;
  va_start(ap, option);
  ret = __probe_option_op(option, PROBE_OPTION_SET, ap);
  va_end(ap);
  return ret;
}

int probe_getoption(int option, ...)
{
  int ret;
  va_list ap;
  va_start(ap, option);
  ret = __probe_option_op(option, PROBE_OPTION_GET, ap);
  va_end(ap);
  return ret;
}
