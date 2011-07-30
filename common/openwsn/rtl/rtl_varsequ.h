#ifndef _RTL_VARSEQUENCE_H_AA62_
#define _RTL_VARSEQUENCE_H_AA62_
/*******************************************************************************
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
#include "rtl_foundation.h"
#include "rtl_variant.h"
#include "rtl_varlist.h"

/*******************************************************************************
 * rtl_xtpvar
 *
 * @modified by zhangwei on 2010.05.01
 *	- first created
 *
 ******************************************************************************/
#define CONFIG_XTPSEQU_CAPACITY 10

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * TiXtpVariant 
 ******************************************************************************/
#define TiXtpVariant TiVariant
#define xtp_var_construct(mem,memsize) var_construct(mem,memsize)
#define xtp_var_destroy(var) var_destroy(var)
#define xtp_var_create(size) var_create(size)
#define xtp_var_free(var) var_free(var)

#define xtp_var_isnull(var) var_isnull(var)
#define xtp_var_clear(var) var_clear(var)
#define xtp_var_type(var) var_type(var)
#define xtp_var_settype(var, type) var_set_type(var, type)

#define xtp_var_size(var) var_size(var)
#define xtp_var_length(var) var_length(var)
#define xtp_var_setlength(var,len) var_set_length(var,len)
#define xtp_var_dataptr(var) var_dataptr(var)

#define xtp_var_equal(var1,var2) var_equal(var1,var2)
#define xtp_var_duplicate(var) var_duplicate(var)
#define xtp_var_assignfrom(var,varfrom) var_assignfrom(var,varfrom)
#define xtp_var_assignto(var,varto) var_assignto(var,varto)

#define xtp_var_setvalue(var,type,buf,len) var_set_value(var,type,buf,len)
#define xtp_var_getvalue(var,ptype,buf,size) var_value(var,ptype,buf,size)

#define xtp_var_bitvalue(var) var_bitvalue(var)
#define xtp_var_set_bitvalue(var,bit) var_set_bitvalue(var,bit)
#define xtp_var_boolvalue(var) var_boolvalue(var)
#define xtp_var_set_boolvalue(var,value) var_set_boolvalue(var,value);
#define xtp_var_get_char(var) var_get_char(var);
#define xtp_var_set_charvalue(var,value) var_get_char(var,value);
#define xtp_var_int8value(var) var_get_char(var);
#define xtp_var_set_int8(var,value) var_get_char(var,value);
#define xtp_var_uint8value(var) var_get_char(var);
#define xtp_var_set_uint8(var,value) var_get_char(var,value);
#define xtp_var_int16value(var) var_get_char(var);
#define xtp_var_set_int16(var,value) var_get_char(var,value);
#define xtp_var_uint16value(var) var_get_char(var);
#define xtp_var_set_uint16(var,value) var_get_char(var,value);
#define xtp_var_string(var,buf,size) var_get_char(var,size);
#define xtp_var_set_string(var,str) var_get_char(var,str);
#define xtp_var_binary(var,buf,size) var_get_char(var,size);
#define xtp_var_set_binary(var,buf,len) var_get_char(var,buf,len);
#define xtp_var_dump(var) var_dump(var)

/*******************************************************************************
 * TiXtpVarSequence
 * 该类协助将变量的值写入内存缓冲区,通常该内存缓冲区就是TiXtpPacket的Payload部分.
 * 
 * attention: 
 * the var sequence isn't part of the xtp. actually, it should on top of xtp.
 ******************************************************************************/


#define TiXtpVarSequence TiVarList

#define xtp_varsequ_construct(buf,memsize) varlist_construct(buf,memsize)
#define xtp_varsequ_destroy(varsequ) varlist_destroy(varsequ)
#define xtp_varsequ_create(capacity) varlist_create(capacity)
#define xtp_varsequ_free(varsequ) varlist_free(varsequ)

#define xtp_varsequ_append(varsequ,var) varlist_append(varsequ,var)
#define xtp_varsequ_first(varsequ) varlist_first(varsequ)
#define xtp_varsequ_next(varsequ) varlist_next(varsequ)

#define xtp_varsequ_capacity(varlist) varlist_capacity(varlist)
#define xtp_varsequ_count(varlist) varlist_count(varlist)
#define xtp_varsequ_current(varlist) varlist_current(varlist)

#define xtp_varsequ_set_count(varlist) varlist_set_count(varlist)
#define xtp_varsequ_set_current(varlist) varlist_set_current(varlist)

uint16 xtp_varsequ_pack(TiXtpVarSequence * varsequ, char * buf, uint16 size );
uint16 xtp_varsequ_unpack( TiXtpVarSequence * varsequ, char * buf, uint16 len );

#ifdef CONFIG_DEBUG
void xtp_varsequ_test();
void xtp_varsequ_dump( TiXtpVarSequence * varsequ );
#endif


#ifdef __cplusplus
}
#endif

#endif /*_RTL_VARSEQUENCE_H_AA62_*/
