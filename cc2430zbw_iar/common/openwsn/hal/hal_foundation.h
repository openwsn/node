#ifndef _HAL_FOUNDATION_H_3721_
#define _HAL_FOUNDATION_H_3721_
/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
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

/*******************************************************************************
 * @author zhangwei on 20060906
 * foundation.h
 * this is the base foundation file of all the files in the application.
 * 
 * @status 
 * - Released. Tested ok.
 *
 * @modified by zhangwei on 20060906
 * add "signed" in the typedef of int8
 * this is because some compile will assume char as unsigned type, while here
 * we need a signed char.
 ******************************************************************************/

/**
 * CONFIG_INT2HANDLER_ENABLE
 * Enable the interrupt-handler map table. This is the default settings on most
 * of the microcontrollers.
 * 
 * CONFIG_INT2HANDLER_CAPACITY
 * Configure the interrupt-handler mapping table capacity. Suggested value is from 
 * 8~32. It depends on how many hardware interrupts you used in your system. 
 */


#define CONFIG_INT2HANDLER_ENABLE

#ifndef CONFIG_INT2HANDLER_CAPACITY
#define CONFIG_INT2HANDLER_CAPACITY 32
#endif

/* @attention
 * in WinAVR(avr-gcc), <stdint.h> contains the basic types used in WinAVR such as:
 * int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t,
 * intptr_t, uintptr_t, size_t, wchar_t 
 * 
 * so if you don't use WinAVR, you can comment the line about including stdint.h.
 */

#include "hal_configall.h"
#ifndef CONFIG_COMPILER_IAR
#include <stdint.h>
#endif
#include "../rtl/rtl_foundation.h"

extern uint8 g_atomic_level ;

/* System wide event identifier */

#define EVENT_RESET                     1
#define EVENT_RESTART                   2
#define EVENT_WAKEUP                    3
#define EVENT_SLEEP                     4
#define EVENT_TIMER_EXPIRED             5
#define EVENT_UART_DATA_ARRIVAL         7
#define EVENT_DATA_ARRIVAL              7
#define EVENT_DATA_COMPLETE             8
#define EVENT_REQUEST                   9
#define EVENT_REPLY                     10
#define EVENT_ACTION_DONE               11

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Function typedef for hardware interrupt handlers. Such function can be called
 * by hardware directly if attached to some interrupt.
 */
typedef void (* TiFunInterruptHandler)(void);


/**
 * interrupt number to object's handler mapping table.
 * this table is used to save the relationship between interrupt number and object's
 * event handler. when the hardware interrupt occurs, the kernel can use this
 * table to map the hardware interrupt call to the object's handler call.
 *
 * 	{interrupt number, objects handler, object}
 */
typedef struct{
  uint8 num;
  TiFunEventHandler handler;
  void * owner;
}_TiIntHandlerItem;

/* "iht_" denotes "interrupt number - handler table" */

#ifdef CONFIG_INT2HANDLER_ENABLE
extern _TiIntHandlerItem m_int2handler[CONFIG_INT2HANDLER_CAPACITY];
#endif

/**
 * hal layer initialization. It will initialize the global variables such as the 
 * interrupt-object handler mapping table and possible listener functions. 
 */
void hal_init( TiFunEventHandler listener, void * object );

/*******************************************************************************
 * interaction with the upper layer such as the osx kernel or other listeners
 *
 * osx is a ultralight OS kernel running on top of the hal layer. the following 
 * types and functions are used to help interfacing the hal layer objects to 
 * the osx kernel. 
 * 
 * though the following is used to help using osx kernel, this module doesn't 
 * require to include osx modules when compiling. 
 * 
 * @attention
 *	- you must call hal_swinit() some where before you can call the following 
 * functions successfully. 
 ******************************************************************************/

void hal_setlistener( TiFunEventHandler listener, void * listener_owner );
void hal_notifylistener( TiEvent * e );
void hal_notify_ex( TiEventId eid, void * objectfrom, void * objectto );










// @todo 
// for MSP430 and STM32. will be eliminated soon

/***********************************************************************************
* MACROS
*/

#ifndef BV
#define BV(n)      (1 << (n))
#endif

#ifndef BM
#define BM(n)      (1 << (n))
#endif

#ifndef BF
#define BF(x,b,s)  (((x) & (b)) >> (s))
#endif

#ifndef MIN
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif

#ifndef WIN32
#define UPPER_WORD(a) ((WORD) (((DWORD)(a)) >> 16))
#define HIWORD(a)     UPPER_WORD(a)

#define LOWER_WORD(a) ((WORD) ((DWORD)(a)))
#define LOWORD(a)     LOWER_WORD(a)

#define UPPER_BYTE(a) ((BYTE) (((WORD)(a)) >> 8))
#define HIBYTE(a)     UPPER_BYTE(a)

#define LOWER_BYTE(a) ((BYTE) ( (WORD)(a))      )
#define LOBYTE(a)     LOWER_BYTE(a)
#endif

/* takes a byte out of a uint32 : var - uint32,  ByteNum - byte to take out (0 - 3) */
#define BREAK_UINT32( var, ByteNum ) \
    (byte)((uint32)(((var) >>((ByteNum) * 8)) & 0x00FF))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
    ((uint32)((uint32)((Byte0) & 0x00FF) \
        + ((uint32)((Byte1) & 0x00FF) << 8) \
            + ((uint32)((Byte2) & 0x00FF) << 16) \
                + ((uint32)((Byte3) & 0x00FF) << 24)))

#define BUILD_UINT16(loByte, hiByte) \
    ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define BUILD_UINT8(hiByte, loByte) \
    ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)

/*
*  This macro is for use by other macros to form a fully valid C statement.
*  Without this, the if/else conditionals could show unexpected behavior.
*
*  For example, use...
*    #define SET_REGS()  st( ioreg1 = 0; ioreg2 = 0; )
*  instead of ...
*    #define SET_REGS()  { ioreg1 = 0; ioreg2 = 0; }
*  or
*    #define  SET_REGS()    ioreg1 = 0; ioreg2 = 0;
*  The last macro would not behave as expected in the if/else construct.
*  The second to last macro will cause a compiler error in certain uses
*  of if/else construct
*
*  It is not necessary, or recommended, to use this macro where there is
*  already a valid C statement.  For example, the following is redundant...
*    #define CALL_FUNC()   st(  func();  )
*  This should simply be...
*    #define CALL_FUNC()   func()
*
* (The while condition below evaluates false without generating a
*  constant-controlling-loop type of warning on most compilers.)
*/
#define st(x)      do { x } while (__LINE__ == -1)

// typedef void (*ISR_FUNC_PTR)(void);
// typedef void (*VFPTR)(void);



/***********************************************************************************
* Memory Attributes
*/

#ifdef __IAR_SYSTEMS_ICC__
	#if defined __ICC430__	
		#define  CODE
		#define  XDATA	
	#elif defined __ICC8051__
		#define  CODE   __code
		#define  XDATA  __xdata
	#else
		#error "Unsupported architecture"
	#endif

#elif defined __KEIL__
	#define  CODE   code
	#define  XDATA  xdata

#elif defined WIN32
	#define CODE
	#define XDATA
	#include "windows.h"
	#define FAR far

#else
	//#error "Unsupported compiler"
#endif


#ifdef __cplusplus
}
#endif

#endif /* _HAL_FOUNDATION_H_3721_ */
