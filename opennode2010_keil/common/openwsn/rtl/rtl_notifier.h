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
#ifndef _RTL_NOTIFIER_H_8496_
#define _RTL_NOTIFIER_H_8496_

/* rtl_notifier
 * this is an C language version of notifier object. notifier is used to connect
 * multiple objects and transmit events between sender and receiver.
 *
 * @state
 *	finished. not fully tested	
 *
 * @author zhangwei on 200812
 * @modified by zhangwei on 200903
 *	- revision 
 */ 

#include "rtl_configall.h"
#include "rtl_foundation.h"

/* control whether the notif_post() function can be used in the project 
 * you should include "os_scheduler.h" in your project because this module
 * will use some functions in that module, but you cannot include it directly
 * in this one. 
 */
#define CONFIG_NOTIF_POST_ENABLE

#define TiFunNotifyHandler TiFunEventHandler
#define NOTIFIER_HOPESIZE(capacity) (sizeof(TiNotifier) - sizeof(TiVector) + VECTOR_HOPESIZE(capacity))

typedef struct{
	uint16       memsize;
	void        owner;
	TiVector    handlers;
}TiNotifier;

TiNotifier *    notif_construct( void * buf, uint16 size, uint8 capacity );
void            notif_destrit( TiNotifier * notif );
void            notif_addlistener( TiNotifier * notif, TxFunNotifyHandler handler, void * object );
void            notif_removelistener( TiNotifier * notif, TxFunNotifyHandler handler, void * object );
void            notif_send( TiNotifier * notif, TiEvent * e );

/* void notif_post( TiNotifier * notif, TxEvent e );
 * notif_post() is similar to notif_send(), but it's an asynchrous call. it will
 * place the event into the scheduler, and let the scheduler to make the function
 * calls.
 */
#ifdef CONFIG_NOTIF_POST_ENABLE
	#define notif_post(notif,e,sche) sche_put(sche,notif_send,notif,e)
#endif

#endif
