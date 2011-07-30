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

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_assert.h"

/******************************************************************************
 * @author zhangwei on 20070610
 * @description
 * "hal_foundation" module, contains some general utility functions used frequvently
 * in other modules.
 *  
 * @state
 *  developing
 * 
 * @modified by zhangwei on 20070610
 * 	- add hal_delay()
 * 	- add the interface functions on interrupt management
 * @modified by zhangwei on 20090520
 *	- remove most of the C source code in this file. this file should be as simple
 *    and short as possible.
 *	- add interrupt num-object-handler map table support in this module.
 *  
 *****************************************************************************/ 





/*****************************************************************************
 * TiIntTable
 * Interrupt Handler Table (iht)
 * interrupt handler table is quite like the vector interrupt table implemented
 * by the vector interrupt controller (VIC). it's used to help encapsulate the 
 * standard interrupt service routine (ISR) as standard event handlers. 
 *
 * standard interrupt service routine's prototype
 *	void isr_name()
 *
 * standard event handlers
 *	void eventhandler( void * object, TiEvent * evt )
 *
 * after connecting the ISR and eventhandler, then we can let the hardware event
 * fire the eventhandler directly. 
 * 
 * @modified by zhangwei on 20090407
 *  - added
 ****************************************************************************/

/* 完成ISR和num之间转换，完成ISR和handler之间转换 */

typedef struct{
	uint8 num;
	TiFunEventHandler handler;
	void * object;
	TiEvent evt;
}TiIntTableItem;


/* number to object's handler mapping table.
 * this table is used to save the relationship between interrupt number and object's
 * event handler. when the hardware interrupt occurs, the kernel can use this
 * table to map the hardware interrupt call to the object's handler call.
 *
 * 	{interrupt number, objects handler, object}
 */

typedef struct{
  uint8 num;
  TiFunEventHandler handler;
  void * object;
}_TiNum2ObjectItem;

#ifdef CONFIG_NUM2OBJECT_ENABLE
static _TiNum2ObjectItem m_num2object[ CONFIG_NUM2OBJECT_CAPACITY ];
#endif

TiFunOsxPostEvent m_osxpost = NULL;

void hal_init()
{
}



/******************************************************************************
 * interaction with osx kernel
 *****************************************************************************/

void hal_setosxpost( TiFunOsxPostEvent osx_post )
{
	m_osxpost = osx_post;
}

void hal_osxpost( TiEventId eid, void * objectfrom, void * objectto )
{
	static TiEvent e;
	assert( m_osxpost != NULL );
	e.id = eid;
	e.objectfrom = objectfrom;
	e.objectto = objectto;
	m_osxpost( &e );
}

/******************************************************************************
 * interrupt number - object's handler mapping table  (abbr. as iht)
 * this table records the relationships between interrupt number and related object's
 * event handler. it's usually used to find the related object by interrupt number
 * only in interrupt service routines (isr).
 *
 * 	{interrupt number, objects handler, object}
 *
 *****************************************************************************/

#ifdef CONFIG_INT_HANDLER_TABLE_ENABLE 
static TiHandleTable * m_inttable = NULL;
#endif

#ifdef CONFIG_INT_HANDLER_TABLE_ENABLE 
int hal_register( uint8 num, TiFunEventHandler handler, void * object )
{
	hdt_register( m_inttable, num, handler, object );
	return 0;
}
#endif

#ifdef CONFIG_INT_HANDLER_TABLE_ENABLE 
int hal_unregister( uint8 num )
{
	hdt_unregister( m_inttable, num );
	return 0;
}
#endif

#ifdef CONFIG_INT_HANDLER_TABLE_ENABLE 
int hal_getobjecthandler( uint8 num, TiFunEventHandler * phandler, void ** pobject )
{
	return hdt_getobjecthandler( m_inttable, num, phandler, pobject );
}
#endif



/******************************************************************************
 * interrupt number - object's handler mapping table  (abbr. as iht)
 * this table records the relationships between interrupt number and related object's
 * event handler. it's usually used to find the related object by interrupt number
 * only in interrupt service routines (isr).
 *
 * 	{interrupt number, objects handler, object}
 *
 *****************************************************************************/

#ifdef CONFIG_NUM2OBJECT_ENABLE
int hal_register( uint8 num, TiFunEventHandler handler, void * object )
{
	int idx;

	idx = num % CONFIG_NUM2OBJECT_CAPACITY;
	m_num2object[idx].num = num;
	m_num2object[idx].handler = handler;
	m_num2object[idx].object = object;

	return idx;
}
#endif

#ifdef CONFIG_NUM2OBJECT_ENABLE
int hal_unregister( uint8 num )
{
	int idx;

	idx = num % CONFIG_NUM2OBJECT_CAPACITY;
	m_num2object[idx].num = num;
	m_num2object[idx].handler = NULL;
	m_num2object[idx].object = NULL;

	return idx;
}
#endif

#ifdef CONFIG_NUM2OBJECT_ENABLE
int hal_getobjecthandler( uint8 num, TiFunEventHandler * phandler, void ** pobject )
{
	int idx;

	idx = num % CONFIG_NUM2OBJECT_CAPACITY;
	*phandler = m_num2object[idx].handler;
	*pobject = m_num2object[idx].object;

	return (m_num2object[idx].handler == NULL) ? -1 : idx;
}
#endif


