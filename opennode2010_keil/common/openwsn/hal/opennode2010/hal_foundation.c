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
 * @author zhangwei on 20070610
 * @description
 * "hal_foundation" module, contains some general utility functions used frequvently
 * in other modules.
 *  
 * @state
 *  - released. tested ok.
 * 
 * @modified by zhangwei on 20070610
 * 	- add hal_delay()
 * 	- add the interface functions on interrupt management
 * @modified by zhangwei on 20090520
 *	- remove most of the C source code in this file. this file should be as simple
 *    and short as possible.
 *	- add interrupt num-object-handler map table support in this module.
 * @modified by zhangwei on 20090528
 *	- revised.
 *  
 ******************************************************************************/ 
 
#include "hal_configall.h"
#include <string.h>
#include "hal_foundation.h"

/* @attention: You cannot include "hal_assert" module here for testing purpose, 
 * because this module is included by "hal_assert". This is a bottom layer module 
 * in the hal including tree.
 *
 * #include "../hal_assert.h"
 */
 
uint8 g_atomic_level = 0;
TiFunEventHandler m_listener = NULL;
void * m_listener_owner = NULL;

#ifdef CONFIG_INT2HANDLER_ENABLE
_TiIntHandlerItem m_int2handler[CONFIG_INT2HANDLER_CAPACITY];
#endif

/*******************************************************************************
 * software initialization of the hal layer
 ******************************************************************************/

void hal_init( TiFunEventHandler listener, void * object )
{
	g_atomic_level = 0;
	m_listener = listener;
	m_listener_owner = object;
	memset( &(m_int2handler[0]), 0x00, sizeof(m_int2handler) );
}

/******************************************************************************
 * interaction with upper layer using listener mechanism
 *****************************************************************************/

//inline void hal_setlistener( TiFunEventHandler listener, void * listener_owner )
void hal_setlistener( TiFunEventHandler listener, void * listener_owner )
{
	m_listener = listener;
	m_listener_owner = listener_owner;
}

//inline void hal_notifylistener( TiEvent * e )
void hal_notifylistener( TiEvent * e )
{
	if (m_listener != NULL)
		m_listener( m_listener_owner, e );
}

//inline void hal_notify_ex( TiEventId eid, void * objectfrom, void * objectto )
void hal_notify_ex( TiEventId eid, void * objectfrom, void * objectto )
{
	TiEvent e;

	/* you should guarantee the listener isn't NULL, but you cannot use assert here
     * directly, because this module is the foundation of module hal_assert, not 
     * vice versa */

	/* assert( m_listener != NULL ); */

	if (m_listener != NULL)
	{
		e.id = eid;
        e.handler = NULL;
		e.objectfrom = objectfrom;
		e.objectto = ((objectto == NULL) ? m_listener_owner : objectto);
		m_listener( m_listener_owner, &e );
	}
}
