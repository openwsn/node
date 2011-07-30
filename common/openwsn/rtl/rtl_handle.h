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

#ifndef _RTL_HANDLE_H_7857_
#define _RTL_HANDLE_H_7857_
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

#include "rtl_configall.h"
#include "rtl_foundation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_DYNAMEM_ENABLE
#define HANDLETABLE_HOPESIZE(capacity) (sizeof(TiHandleTable)+sizeof(TiHandleItem)*(capacity))
#define HDT_INVALID_HANDLE ((unsigned uintx)(-1))

/******************************************************************************
 * Handle Management
 * handle is another representation of an object instead of pointer. you can 
 * retrieve the object if you know its handle. 
 *****************************************************************************/

#define TiHandle intx

typedef struct{
	uintx id;
	TiFunEventHandler handler;
	void * object;
}TiHandleItem;

typedef struct{
	uintx capacity; 
}TiHandleTable;

#ifdef CONFIG_DYNAMEM_ENABLE
TiHandleTable * hdt_create( uintx capacity );
void            hdt_free( TiHandleTable * hdt );
#endif

TiHandleTable * hdt_construct( void * buf, uintx size, uintx capacity );
void            hdt_destroy( TiHandleTable * hdt );
uintx           hdt_attach( TiHandleTable * hdt, uintx id, TiFunEventHandler handler, void * object );
void            hdt_detach( TiHandleTable * hdt, uintx id );
uintx           hdt_register( TiHandleTable * hdt, TiFunEventHandler handler, void * object );
void            hdt_unregister( TiHandleTable * hdt, uintx id );
uintx           hdt_getobjecthandler( TiHandleTable * hdt, uintx id, TiFunEventHandler * phandler, void ** pobject );
uintx           hdt_getid( TiHandleTable * hdt, TiFunEventHandler handler, void * object );

#ifdef __cplusplus
}
#endif

#endif
