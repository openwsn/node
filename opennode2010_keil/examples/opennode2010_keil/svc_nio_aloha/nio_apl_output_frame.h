#ifndef _OUTPUT_FRAME_H_4832_
#define _OUTPUT_FRAME_H_4832_

#include "../../common/openwsn/hal/hal_configall.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/hal/hal_uart.h"

void _output_frame( TiFrame * frame, TiUartAdapter * uart );

#endif
