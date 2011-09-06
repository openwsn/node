#ifndef _APL_FOUNDATION_H_5678_
#define _APL_FOUNDATION_H_5678_

/**
 * @attention: You can include "rtl_ascii.h" before "hal_mcu.h". Because "rtl_ascii"
 * will define CR as an macro. However, hal_mcu (which includes stm32f10x.h) in the 
 * STM32 version will regard CR as an register. In order to solve this confliction, 
 * I had to move "rtl_ascii" after "hal_mcu".
 */ 

#include "openwsn/configall.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_assert.h"
#include "openwsn/rtl/rtl_debugio.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/rtl/rtl_framequeue.h"
#include "openwsn/rtl/rtl_iobuf.h"
#include "openwsn/rtl/rtl_slipfilter.h"
#include "openwsn/rtl/rtl_ieee802frame154.h"
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/hal/hal_targetboard.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_mcu.h"
#include "openwsn/rtl/rtl_ascii.h"
#include "openwsn/hal/hal_interrupt.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_debugio.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/svc/svc_sio_acceptor.h"

//#include "openwsn/hal/opennode2010/util.h"
//#include "openwsn/hal/opennode2010/basic_rf.h"

#endif /* _APL_FOUNDATION_H_5678_ */
