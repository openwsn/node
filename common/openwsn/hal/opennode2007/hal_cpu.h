/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 *****************************************************************************/ 

#ifndef _CPU_H_1675_ 
#define _CPU_H_1675_ 

#include "hal_foundation.h"

#define CONFIG_CPU_CLOCK_MSEC_RATIO1 255
#define CONFIG_CPU_CLOCK_MSEC_RATIO2 16
#define CONFIG_CPU_CLOCK_MSEC_RATIO (CONFIG_CPU_CLOCK_MSEC_RATIO1*CONFIG_CPU_CLOCK_MSEC_RATIO2)

/******************************************************************************
 * Delay Functions:
 *
 * _cpu_pause()
 * not recommended to use directly.
 * make the program pause for a short time. we don't recomment you use this function
 * directly because the pause duration is determined by CPU and hardware. it's different
 * on different hardware platform. for current implmentation on ARM7 processor,
 * it's about 2 cycle @todo really ???
 *
 * cpu_delay( msec )
 * recommend to use comparing to _hal_pause(). the "delay" parameter is based on 
 * millseconds. for example, if you want to delay 10 millisecond, you can simply call:
 *     hal_delay( 10 );
 *
 * @attention:
 * before using this function, you should configure the following two macros to
 * adapter to you own hardware platform:
 *
 * #define CONFIG_CPU_CLOCK_MSEC_RATIO1 255
 * #define CONFIG_CPU_CLOCK_MSEC_RATIO2 16
 * 
 * the product of these two macros defines how many clock cycle equals to 1 milli-second.
 *****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"

#define hal_delay(msec) cpu_delay(msec)


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define _cpu_pause() NOP()

#elif defined __IAR_SYSTEMS_ICC__
#pragma optimize=none
#define _cpu_pause() NOP()

#else
void _cpu_pause( void );
#endif

void cpu_delay( uintx msec );

#define cpu_disable_interrupts() cli()
#define cpu_enable_interrupts() sti()

typedef uintx critical_t;

critical_t cpu_enter_critical( void );
void cpu_leave_critical( critical_t state );


/* cpu sleep and wakeup
 * the cpu is usually wakeup by external hardware interrupts such as
 * - UART interrupts
 * - cc2420 FIFOP interrupts
 * - RTC timer expire interrupts
 */
void cpu_sleep( void );
void cpu_reboot( void );
void cpu_sti( void );
void cpu_cli( void );
uintx cpu_state();
void cpu_setstate( uintx state );
void cpu_push( uintx value );
uintx cpu_pop();


#ifdef __cplusplus
}
#endif

#endif
