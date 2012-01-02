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

/* rtl_notifier
 * this is an C language version of notifier object. notifier is used to connect
 * multiple objects and transmit events between sender and receiver.
 *
 * @state
 *	finished. not fully tested	
 *
 * @author zhangwei on 200812
 */ 

typedef struct { 
	TiFunNotifyHandler handler; 
	void * object; 
}_TiNotifierItem;


TiNotifier * notif_construct( void * buf, uint16 size )
{
	TiNotifier * notif = (TiNotifier *)buf;
	notif->memsize = size;
	notif->owner = NULL;
	vec_construct( &notif->handlers, (size - sizeof(TiNotifier) + sizeof(TiVector)), 
		sizeof(_TiNotifierItem) );
	return notif;
}

void notif_destroy( TiNotifier * notif )
{
	return;
}

void notif_addlistener( TiNotifier * notif, TxFunNotifyHandler handler, void * object )
{
	_TiNotifierItem item;
	item.handler = handler;
	item.object = object;
	vec_append( &notif->handlers, &item );
}

void notif_removelistener( TiNotifier * notif, TxFunNotifyHandler handler, void * object )
{
	/* TODO
	 * find the listener in the vector and delete it from the vector.
	 */
}

void notif_send( TiNotifier * notif, TxEvent * e )
{
	_TiNotifierItem * item;
	uint8 i;

	for (i=0; i<vec_count(notif->handlers); i++)
	{
		item = vec_items(notif->handlers, i);
		item->handler( item->object, e );
	}
}
