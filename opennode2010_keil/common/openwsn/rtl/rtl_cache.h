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
#ifndef _CACHE_H_4638_
#define _CACHE_H_4638_

/*******************************************************************************
 * rtl_cache
 * This module implements a simple but very useful object -- TiCache. The eldest
 * item will be replaced according to the current algorithm.
 *
 * @todo
 * performance tuning in the future
 *
 * @author zhangwei(TongJi University) in 200911
 *	- first created.
 ******************************************************************************/ 

#ifndef CONFIG_CACHE_CAPACITY 
#define CONFIG_CACHE_CAPACITY 8
#endif

#ifndef CONFIG_CACHE_MAX_LIFETIME 
#define CONFIG_CACHE_MAX_LIFETIME 0xFF
#endif

#define CACHE_HOPESIZE(itemsize,capacity) (sizeof(TiCache) + itemsize*capacity)

/* TiCache
 * An simple cache. The eldest item will be displaced first. 
 *
 * lifetime = 0 means this is an empty item in the cache. The bigger the lifetime, 
 * the newer the lifetime.
 */
typedef struct{
	uint16 itemsize;
	uintx  capacity;
	uint8  lifetime[CONFIG_CACHE_CAPACITY];				
} TiCache;

TiCache * cache_open( void * mem, uint16 memsize, uint16 itemsize, uintx capacity );
void cache_close( TiCache * cache );
bool cache_hit( TiCache * cache, char * item, uint16 itemlen, uintx * idx );
uintx cache_visit( TiCache * cache, char * item, uint16 itemlen );

#endif
