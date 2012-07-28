/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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

#ifndef _RTL_VERSION_H_4252_
#define _RTL_VERSION_H_4252_

/*****************************************************************************
 * @name rtl_version.h
 * @author zhangwei on 20070331
 * 
 * version manipulation macros in runtime library(RTL)
 * 
 * @history
 * @modified by xxx on 200704
 * 
 ****************************************************************************/ 

typedef uint32 version_t;
#define TiVersion version_t

#define MAKE_VERSION(major,minor,micro) (((major&0x0FF)<<16) | ((minor&0xFF)<<8) | (micro&0xFF))
#define MAJOR_VERSION(ver) ((ver>>16) & 0xFF)
#define MINOR_VERSION(ver) ((ver>>8) & 0xFF)
#define MICRO_VERSION(ver) (ver&0xFF)

#endif
