#ifndef _RTL_VARIANT_H_56YT_
#define _RTL_VARIANT_H_56YT_
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

/*******************************************************************************
 * rtl_variant
 * an variant implementation which can hold different type of values. it supports
 * NULL value, string or binary block. 
 * 
 * @todo
 * 	array support in the future
 *
 * @history
 * @modified by zhangwei on 2004.09.09
 *	- first created
 * @modified by zhangwei on 2010.05.02
 *  - revision. finished. compile passed
 * @modified by yanshixing on 2010.5.9
 ******************************************************************************/

#include "rtl_configall.h"
#include "rtl_foundation.h"

#define VAR_NULL_BIT    0x80
//#define VAR_ARRAY_BIT   0x40//?:not in document

typedef enum {
  VAR_TYPE_NULL = 0,
  VAR_TYPE_BIT,
  VAR_TYPE_BOOL,
  VAR_TYPE_CHAR,
  VAR_TYPE_INT8,
  VAR_TYPE_UINT8,
  VAR_TYPE_INT16,
  VAR_TYPE_UINT16,
  VAR_TYPE_INT32,
  VAR_TYPE_UINT32,
  VAR_TYPE_POINTER,
  VAR_TYPE_F32,
  VAR_TYPE_F64,
  VAR_TYPE_STRING,
  VAR_TYPE_BINARY,
}TiVarType;

#define VAR_ATOM_TYPE(type) ((type) <= VAR_TYPE_F64)

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (* TiFunOnValueChanged)( void * var, void * e );

/* TiVariant
 * 
 * attention
 *  - due to the memory alignment assumptions, the "space" variable must be the last
 * one in the field list.
 */
typedef struct{
    uint16      memsize;
    TiVarType   type;
    // char *   name;
	// TiFunOnValueChanged onvaluechanged;
    uint16      length;
    union{
        uint8   bitvalue;
        bool    boolvalue;
        char    cvalue;
        int8    int8value;
        uint8   uint8value;
        int16   int16value;
        uint16  uint16value;
        int32   int32value;
        uint32  uint32value;
        void *  ptrvalue;
        //FP32    fp32value;
        //FP64    fp64value;
        unsigned char    bytes[8];
    }mem;
}TiVariant;

TiVariant * var_construct( void * mem, uint16 memsize );
void var_destroy( TiVariant * var );
TiVariant * var_create( uint16 size );
void var_free( TiVariant * var );

void var_clear( TiVariant * var );
bool var_isnull( TiVariant * var );
TiVarType var_type( TiVariant * var );
void var_set_type( TiVariant * var, TiVarType type );
uint16 var_size( TiVariant * var );
uint16 var_length( TiVariant * var );
void var_set_length( TiVariant * var, uint16 len );
char * var_dataptr( TiVariant * var );

bool var_equal( TiVariant * var1, TiVariant * var2 );
TiVariant * var_duplicate( TiVariant * var );
TiVariant * var_assignfrom( TiVariant * var, TiVariant * varfrom );
TiVariant * var_assignto( TiVariant * var, TiVariant * varto );

uint16 var_set_value( TiVariant * var, TiVarType type, char * buf, uint16 len );
uint16 var_value( TiVariant * var, uint8 * type, char * buf, uint16 size );

uint8 var_bitvalue( TiVariant * var );
void var_set_bitvalue( TiVariant * var, uint8 bit );
bool var_boolvalue( TiVariant * var );
void var_set_boolvalue( TiVariant * var, bool value );
char var_get_char( TiVariant * var );
void var_set_charvalue( TiVariant * var, char value );
int8 var_int8value( TiVariant * var );
void var_set_int8( TiVariant * var, int8 value );
uint8 var_uint8value( TiVariant * var );
void var_set_uint8( TiVariant * var, uint8 value );
int16 var_int16value( TiVariant * var );
void var_set_int16( TiVariant * var, int16 value );
uint16 var_uint16value( TiVariant * var );
void var_set_uint16( TiVariant * var, char value );
uint16 var_string( TiVariant * var, char * buf, uint16 size );
uint16 var_set_string( TiVariant * var, char * str );
unsigned char * var_binary( TiVariant * var, char * buf, uint16 size );
uint16 var_set_binary( TiVariant * var, char * buf, uint16 len );

// int32 var_attach( TiVariant * var, TiFunOnValueChanged handler );
// int32 var_detach( TiVariant * var );

uint32 var_dump( TiVariant * var, char * buf, uint32 size );

#ifdef __cplusplus
}
#endif

#endif /* _RTL_VARIANT_H_56YT_ */


