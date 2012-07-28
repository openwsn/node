#ifndef _RTL_VARLIST_H_56YT_
#define _RTL_VARLIST_H_56YT_
/***********************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2004-2010 zhangwei(TongJi University)
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
#include <stdlib.h>
#include "rtl_foundation.h"
#include "rtl_variant.h"

#define CONFIG_VARLIST_CAPACITY 10

typedef struct{
    uint16 memsize;
    uint16 capacity;
    uint16 count;
    uint16 current;
}TiVarList;

#ifdef __cplusplus
extern "C" {
#endif

TiVarList * varlist_construct( void * mem, uint16 memsize );
void varlist_destroy( TiVarList * varlist );
TiVarList * varlist_create( uint8 capacity );
void varlist_free( TiVarList * varlist );
TiVariant * varlist_items( TiVarList * varlist, uint16 idx );
TiVariant ** varlist_table( TiVarList * varlist );
bool varlist_append( TiVarList * varlist, TiVariant * var );
TiVariant * varlist_first( TiVarList * varlist );
TiVariant * varlist_next( TiVarList * varlist );

uint16 varlist_capacity( TiVarList * varlist );
uint16 varlist_count( TiVarList * varlist );
uint16 varlist_current( TiVarList * varlist );

uint16 varlist_set_count( TiVarList * varlist );
uint16 varlist_set_current( TiVarList * varlist );

#ifdef __cplusplus
}
#endif

#endif /*_RTL_VARLIST_H_56YT_*/
