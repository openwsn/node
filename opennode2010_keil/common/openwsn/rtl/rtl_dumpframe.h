#ifndef _RTL_DUMPFRAME_H_2442_
#define _RTL_DUMPFRAME_H_2442_

#include "../hal/opennode2010/hal_configall.h"
#include "../hal/opennode2010/hal_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../hal/opennode2010/hal_uart.h"

/**
 * @attention: before calling this function, you should already call rtl_init() to 
 * initialize the fundamental input/output interface 
 */

void ieee802frame154_dump( TiFrame * frame);

#endif
