#ifndef _HAL_ASSERT_H_5748_
#define _HAL_ASSERT_H_5748_

#include "hal_configall.h"

#ifdef CONFIG_DEBUG
  #define assert(v) hal_assert(v)
#else
  #define assert(v) NULL
#endif

void hal_assert(int v);

#endif