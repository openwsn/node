#ifndef _RTL_FOUNDATION_H_7689_
#define _RTL_FOUNDATION_H_7689_
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
 * rtl_foundation
 *
 * system wide definitions for runtime library (RTL)
 *
 * @history
 * @author zhangwei on 2003
 * @modified by zhangwei on 200610
 * @modified by xxx on 200704
 * @modified by zhangwei on 20100709
 *  - revision. add rtl_init() and rtl_assert() 
 *
 * @modified by zhangwei on 200901
 * 	- bug fix: you should include <tchar.h> all the time no matter the macro
 * 	  CONFIG_UNICODE defined or not, or else the system cannot recognize the
 * 	  TCHAR macro.
 *  - tested ok.
 *  - review the source code.
 *  - format the source code.
 *
 * @modified by zhangwei on 2011.08.06
 *	- Enable the definition of TiHandle(TiObjectHandle) and TiHandleId(TiObjectId).
 *
 ******************************************************************************/

#include "rtl_configall.h"

/*****************************************************************************
 * Q: how to enable unicode programming?
 * R:
 * for windows developing, _UNICODE for C runtime library, and UNICODE for windows.
 * furthermore, you need to include <windows.h>.
 *
 * reference
 * - Windows环境下Unicode编程总结 (ZZ), http://yangwei.blogbus.com/logs/3192116.html
 *
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define min(x,y) (((x)<(y))?(x):(y))

/*******************************************************************************
 * TiEvent and TiFunEventHandler
 ******************************************************************************/

struct _TiEvent;

struct _TiEvent{
  uintx id;
  void (* handler)(void * object, struct _TiEvent * e);
  void * objectfrom;
  void * objectto;
};
typedef struct _TiEvent TiEvent;

#define TiEventId uintx

typedef void (* TiFunEventHandler)(void * object, TiEvent * e);


/*******************************************************************************
 * rtl_assert(...)
 * this function provide assert() like function which can be used in any modules.
 * however, you should call rtl_init() first to enable the report functionalitiy. 
 * because the reporting progress needs to operate real hardware. so we can implement
 * assert report in another function (usually in hal layer) and pass it to the 
 * run time library through an function pointer.
 ******************************************************************************/

typedef void (* TiFunAssertReport)( bool cond, char * file, uint16 line );

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_DEBUG
  #define rtl_assert(cond) _rtl_assert_report((cond), __FILE__, __LINE__)
#else
  #define rtl_assert(cond) 
#endif

extern TiFunAssertReport g_assert_report;
void _rtl_assert_report( bool cond, char * file, uint16 line );

#ifdef __cplusplus
}
#endif

/*******************************************************************************
 * debug input/output (debugio) support by the rtl layer
 * you must call rtl_init(...) first to register the hardware input/output function.
 ******************************************************************************/

typedef intx (* TiFunDebugIoPutChar)( void * io, char c );
typedef char (* TiFunDebugIoGetChar)( void * io );

extern void * g_dbc_io_provider;
extern TiFunDebugIoPutChar g_dbc_putchar;
extern TiFunDebugIoGetChar g_dbc_getchar;

/**
 * Initialize the runtime library(rtl). generally, you needn't call this function.
 * however, if you want to activate the rtl_assert() and debug input/output functionalities
 * in the runtime library, you should call rtl_init() when your application startup.
 * or else the rtl_assert() and debug functions don't know how to really perform 
 * input/output operations with the hardware modules.
 * 
 * Return
 *  none
 * 
 * Example
 *  rtl_init( hal_assert_report, g_uart0, uart_putchar, uart_getchar_wait );
 * 
 *  If you include hal_debug module in your project, then there're already the putchar() 
 *  and getchar() function for you. You can initialize rtl layer as the following:
 * 
 *  dbio = (void *)dbio_open( 38400 );
 *  rtl_init( dbio, (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
 */
void rtl_init( void * io_provider, TiFunDebugIoPutChar debugio_putchar, TiFunDebugIoGetChar debugio_getchar, 
    TiFunAssertReport assert_report );



/*
#define STEP_CHECK(cond,retvar,retvalue) \
	if ((retvar >= 0) && (cond)) retvar = retvalue;
*/

/*
 * obsolete definitions
 */

/*******************************************************************
  DEBUG macros
  These functions can be used in debug mode of the application.
*/

#ifdef CONFIG_DEBUG
	#define DEBUG_RUN(x) x
	#define NDEBUG_RUN(x)
    #define trace(x) printf("%s",(x))
    #define tracen(x,n) printf((x),(n))
    #define tracen2(x,n1,n2) printf((x),(n1),(n2))
    #define tracen3(x,n1,n2,n3) printf((x),(n1),(n2),(n3))
#else
	#define DEBUG_RUN(x) NULL
	#define NDENUG_RUN(x) x
    #define trace(x) NULL
    #define tracen(x,n) NULL
    #define tracen2(x,n1,n2) NULL
    #define tracen3(x,n1,n2,n3) NULL
    #define tracen4(x,n1,n2,n3,n4) NULL
    #define tracen5(x,n1,n2,n3,n4,n5) NULL
    #define tracen6(x,n1,n2,n3,n4,n5,n6) NULL
#endif


#define addrof(arr) (&(arr[0]))
#define ADDR(arr) (&((arr)[0]))
#define ADDRAT(arr,n) (&(arr[0])+(n))

#ifdef CONFIG_COMPILER_BORLAND
//#define max(a,b) (((a)>(b)) ? (a) : (b))
//#define min(a,b) (((a)<(b)) ? (a) : (b))
#endif

/**
 * TiHandle is used to save an object handle. The handle can be a unique identifier 
 * of the object or an pointer to the object. 
 * 
 * @attention:
 * You MUST guarantee sizeof(TiHandleId) == sizeof(void*). Currently, TiHandleId
 * is defined as "unsigned int". In most systems, sizeof(unsigned int) equals sizeof(void*),
 * but it's not always the truth.
 * 
 * This is checked in rtl_init() by an assertion. You may need to adjust the definition
 * here to adapt to your own system.
 */

typedef union{
  int id;
  void * ptr;
  int value;
}TiHandle;

#define TiHandleId unsigned int

#define TiObjectHandle TiHandle
#define TiObjectId TiHandleId

/******************************************************************/
#ifdef __cplusplus
}
#endif

#endif
