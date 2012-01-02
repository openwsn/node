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
#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_handle.h"

#ifdef CONFIG_DYNAMEM_ENABLE
  #include <stdlib.h>
  #include <string.h>
#endif

/******************************************************************************
 * TiHandleTable
 * Handle Management
 * handle is another representation of an object instead of pointer. you can 
 * retrieve the object if you know its handle. 
 * 
 * the TiHandles component maintains a 1:1 map between handle id and the object. 
 * the object's event handler function is also saved inside the table to facilitate
 * developing in the future. 
 *
 * the prefix "hdt_" means handle table. "hdl_" means "handle".
 * 
 * attention
 *	avoid using 0 and -1 as the id.  they're reserved by the system.
 *
 * @status
 *	- finished
 * 
 * @author zhangwei on 200905xx
 *	- first version
 *****************************************************************************/

static inline uintx _hdt_apply( TiHandleTable * hdt );
static inline void _hdt_release( TiHandleTable * hdt, uintx idx );
static inline uintx _hdt_search_handler( TiHandleTable * hdt, TiFunEventHandler handler, void * object );
static inline uintx _hdt_search_id( TiHandleTable * hdt, uintx id );

#define _hdt_items(hdt) ((TiHandleItem *)((char*)hdt + sizeof(TiHandleTable)))


#ifdef CONFIG_DYNAMEM_ENABLE
TiHandleTable * hdt_create( uintx capacity )
{
	uintx size = HANDLETABLE_HOPESIZE(capacity);
	void * buf = malloc( size );
	return hdt_construct( buf, size, capacity );
}
#endif

#ifdef CONFIG_DYNAMEM_ENABLE
void hdt_free( TiHandleTable * hdt )
{
	if (hdt != NULL)
	{
		hdt_destroy( hdt );
		free( hdt );
	}
}
#endif

inline TiHandleTable * hdt_construct( void * buf, uintx size, uintx capacity )
{
	TiHandleTable * hdt = (TiHandleTable *)buf;

	rtl_assert( buf != NULL );
	rtl_assert( HANDLETABLE_HOPESIZE(capacity) <= size );
	memset( buf, 0x00, size );

	return hdt;
}

inline void hdt_destroy( TiHandleTable * hdt )
{
	return;	
}

/* @warning
 *	- if there's already an handler registered with the input id, then the old setting
 * will be overrided by the new one. 
 *  - the above feature seems not safe. but it's quite simple and efficient. that's why
 * i still prefer this behavior in the hdt_register() function. 
 *  - since we had a % operatioin to get the index, this may lead to conflication. we 
 * you allocate a handle table with large enough capacity. 
 *    => i want to keep the id => idx map as simple and efficient as possible. that's 
 * why i still choose this unsafe solution.
 *	  => it's the developer's responsibility to guarantee there's no id conflication!
 * 
 * @attention
 *	- handle id should be 0 or postive. 
 *
 * @return
 *	0               failed
 *	>0              success. the value is the handle id itself.
 */
inline uintx hdt_attach( TiHandleTable * hdt, uintx id, TiFunEventHandler handler, void * object )
{
	uintx idx;
	TiHandleItem * items = _hdt_items(hdt);

	rtl_assert( id > 0 );
	idx = id % (hdt->capacity);
	if ((items[idx].id == id) || (items[idx].id == 0))
	{
		items[idx].id = id;
		items[idx].handler = handler;
		items[idx].object = object;
		return id;
	}
	else
		return 0;
}

inline void hdt_detach( TiHandleTable * hdt, uintx id )
{
	hdt_unregister( hdt, id );
}

/* similar to hdt_attach(), but this function will assign an handle id to the 
 * object handler automatically 
 *
 * @return
 *	0               failed
 *  >0              success. the value is the handle id
 */
inline uintx hdt_register( TiHandleTable * hdt, TiFunEventHandler handler, void * object )
{
	uintx idx;
	TiHandleItem * items = _hdt_items(hdt);

	idx = _hdt_search_handler( hdt, handler, object );
	if (idx > 0)
	{
		items[idx].handler = handler;
		items[idx].object = object;
		idx = items[idx].id;
	}
	else{
		idx = _hdt_apply( hdt );
		if (idx > 0)
		{
			items[idx].id = idx;
			items[idx].handler = handler;
			items[idx].object = object;
		}
	}

	return idx;
}

inline void hdt_unregister( TiHandleTable * hdt, uintx id )
{
	uintx idx;
	TiHandleItem * items = _hdt_items(hdt);

	rtl_assert( id > 0 );

	idx = _hdt_search_id( hdt, id );
	if (idx > 0)
	{
		items[idx].id = 0; 
		items[idx].handler = NULL;
		items[idx].object = NULL;
	}
}

/* return
 *	0               failed
 *	positive        success. the value is the id
 */
inline uintx hdt_getobjecthandler( TiHandleTable * hdt, uintx id, TiFunEventHandler * phandler, void ** pobject )
{
	uintx idx;
	TiHandleItem * items = _hdt_items(hdt);

	rtl_assert( id > 0 );

	idx = _hdt_search_id( hdt, id );
	if (idx > 0)
	{
		*phandler = items[idx].handler;
		*pobject = items[idx].object;
		idx = items[idx].id;
	}
	else
		idx = 0;

	return idx;
}

/* return
 *	0               failed
 *	positive        success
 */
inline uintx hdt_getid( TiHandleTable * hdt, TiFunEventHandler handler, void * object )
{
	uintx idx;
	TiHandleItem * items = _hdt_items(hdt);

	idx = _hdt_search_handler( hdt, handler, object );
	if (idx > 0)
		return items[idx].id;
	else
		return 0;
}

/* return
 *  0		failed
 *  >0      success. the value is the index.
 */
inline uintx _hdt_search_handler( TiHandleTable * hdt, TiFunEventHandler handler, void * object )
{
	bool found = false;
	TiHandleItem * items = _hdt_items(hdt);
	uintx i;

	for (i=1; i<hdt->capacity; i++)
	{
		if ((items[i].handler == handler) && (items[i].object == object))
		{
			found = true;
			break;
		}
	}

	return (found ? i : 0);
}

/* return
 *  0		failed
 *  >0      success
 */
inline uintx _hdt_search_id( TiHandleTable * hdt, uintx id )
{
	bool found = false;
	TiHandleItem * items = _hdt_items(hdt);
	uintx i;

	rtl_assert( id > 0 );

	i = id % hdt->capacity;
	if (items[i].id == id)
	{
		found = true;
	}
	else{
		found = false;
		for (i=1; i<hdt->capacity; i++)
		{
			if (items[i].id == id)
			{
				found = true;
				break;
			}
		}
	}

	return (found ? i : 0);
}

/* return
 *	0				failed
 *	>0              success
 */
inline uintx _hdt_apply( TiHandleTable * hdt )
{
	bool found = false;
	uintx i;
	TiHandleItem * items = _hdt_items(hdt);

	for (i=1; i<hdt->capacity; i++)
	{
		if (items[i].id == 0)
		{
			found = true;
			break;
		}
	}

	return (found ? i : 0);
}

void _hdt_release( TiHandleTable * hdt, uintx idx )
{
	TiHandleItem * items = _hdt_items(hdt);
	items[idx].id = 0;
	items[idx].handler = NULL;
	items[idx].object = NULL;
}
