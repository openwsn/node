#ifndef _RTL_DUMPFRAME_H_2442_
#define _RTL_DUMPFRAME_H_2442_

#include "../hal/hal_configall.h"
#include "../hal/hal_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../hal/hal_uart.h"

/**
 * @attention: before calling this function, you should already call rtl_init() to 
 * initialize the fundamental input/output interface 
 */

void ieee802frame154_dump( TiFrame * frame);

#endif
