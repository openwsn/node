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

#ifndef _HAL_FOUNDATION_H_5342_
#define _HAL_FOUNDATION_H_5342_

/******************************************************************************
 * Hardware Abstraction Layer (hal) Foundation
 * HAL中的对象及其方法要保持最大程度的简明．每一个接口变量和函数，能不增加就不增加．
 * 这样做是为了最大限度简化HAL在不同硬件平台上的移植．
 * 
 * @author zhangwei on 2006-07-24
 * 	this is the foundation header of HAL. every file in "hal" directory should
 * include this one or "hal_configall.h" as their first header file in order
 * to make the definition and declarations in this file effective.
 *
 * @history
 * @modified by zhangwei on 2006-07-24
 * 	- this file replace the old "include.h".
 * @modified by zhangwei on 2006-09-01
 * 	- add type TiFunInterruptHandler and TiFunEventHandler. these two are function
 * 	  pointer types. they are often used when setting callback functions.
 * @modified by zhangwei on 20090520
 *	- great revision
 *****************************************************************************/

#include "hal_configall.h"
#include "arch_configall.h"
#include "arch_foundation.h"
#include <string.h>

/* CONFIG_NUM2OBJECT_ENABLE
 * controls whether to use the interrupt number-object-handler mapping table.
 * the default setting is enabled. if you undef this macro, you'll save memory
 * space allocated to the table before.
 */
#define CONFIG_NUM2OBJECT_ENABLE
#define CONFIG_NUM2OBJECT_CAPACITY 8

#define min(x,y) (((x)<(y))?(x):(y))
#define max(x,y) (((x)>(y))?(x):(y))

//#define MIN(n,m) (((n) < (m)) ? (n) : (m))
#define MAX(n,m) (((n) < (m)) ? (m) : (n))
#define ABS(n) ((n < 0) ? -(n) : (n))

#define EVENT_SLEEP 		0x03
#define EVENT_WAKEUP        0x04


/******************************************************************************
 * osx kernel related functions
 *****************************************************************************/

/* TiEvent
 * represent the event in the system. "event" drives the state transfer inside
 * an object.
 * the member variable "handler" here is actually TiFunEventHandler type.
 */
#define TiEventId uintx


#ifdef __cplusplus
extern "C" {
#endif

struct _TiEvent;

struct _TiEvent{
	TiEventId id;
	intx (* handler)( void * object, struct _TiEvent * e );
	void * objectfrom;
	void * objectto;
};

typedef struct _TiEvent TiEvent;

/* TiFunInterruptHandler
 * represents the hardware's interrupt service routine (ISR)
 *
 * TiFunEventHandler
 * represents the event handler of some object
 */
typedef void (* TiFunInterruptHandler)( void );
typedef intx (* TiFunEventHandler)( void * object, TiEvent * e );

extern void hal_init();
 
/* the osx kernel is build on top of the "hal" layer. we only allow the high layer 
 * functions to invoke the lower level function. in order to post an event to the 
 * scheduler, we use the tip of function pointer.  the lower hal layer can now
 * post event into scheduler's queue by the function pointer variable here.
 * 
 * the post event function is usually used by the interrupt service routine to 
 * inidcate the upper layer some things happened, such as the data ready in ADC
 * or the packet arrival from tranceiver chip. 
 * 
 * @attention
 *	when booting the device, the osx kernel should call hal_setosxpost() to
 * initialize the hal layer first before calling hal_osxpost().
 */ 
typedef void (* TiFunOsxPostEvent)( TiEvent * e );

extern void hal_setosxpost( TiFunOsxPostEvent osx_post );
extern void hal_osxpost( TiEventId eid, void * objectfrom, void * objectto );

/* hal_register()
 * add a {interrupt number, handler, object} mapping item into the internal
 * mapping table.
 *
 * hal_unregister()
 * remove an item from the internal mapping table
 *
 * hal_getobjecthandler()
 * searching for the object/handler information by interrupt number
 */

#ifdef CONFIG_NUM2OBJECT_ENABLE
extern int hal_register( uint8 num, TiFunEventHandler handler, void * object );
extern int hal_unregister( uint8 num );
extern int hal_getobjecthandler( uint8 num, TiFunEventHandler * phandler, void ** pobject );
#endif

#ifdef __cplusplus
}
#endif

#endif
