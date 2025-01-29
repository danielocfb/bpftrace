#pragma once

#ifdef USE_BLAZESYM
#include <blazesym.h>

#define BLAZESYM_OPT                                                           \
  _Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")
#endif
