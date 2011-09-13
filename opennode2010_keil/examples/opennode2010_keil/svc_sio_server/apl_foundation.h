/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
 * 
 * OpenWSN is a free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple 
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 * 
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi 
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 * 
 ******************************************************************************/ 

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
