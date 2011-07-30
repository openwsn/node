/*****************************************************************************
* This file is part of OpenWSN, the Open Wireless Sensor Network System.
*
* Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
* 
* OpenWSN is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 or (at your option) any later version.
* 
* OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
* 
* You should have received a copy of the GNU General Public License along
* with eCos; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
* 
* As a special exception, if other files instantiate templates or use macros
* or inline functions from this file, or you compile this file and link it
* with other works to produce a work based on this file, this file does not
* by itself cause the resulting work to be covered by the GNU General Public
* License. However the source code for this file must still be made available
* in accordance with section (3) of the GNU General Public License.
* 
* This exception does not invalidate any other reasons why a work based on
* this file might be covered by the GNU General Public License.
* 
****************************************************************************/ 

#include "hal_flash.h"

/*
typedef struct{
  uint32 addr;
  uint32 size;
  uint16 blocksize;
  uint32 index;
}TFlash;

TFlash * flash_construct( char * buf, uint32 size, uint32 flashaddr, uint32 flashsize,
	uint16 blocksize )
{
	TFlash * flash;
	
	if (sizeof(TFlash) < size)
		flash = (TFlash *)buf;
	else
		flash = NULL;
		
	if (flash != NULL)
	{
		memset( buf, 0x00, sizeof(TFlash) );
		flash->addr = flashaddr;
		flash->size = flashsize;
		flash->blocksize = blocksize;
		index = 0;
	}
	
	return flash;
}

void flash_destroy( TFlash * flash )
{
	NULL;
}

void flash_reset( TFlash * flash )
{
	flash->index = 0;
}

boolean flash_seek( TFlash * flash, int32 index )
{
	flash->index = index;
}

uint32 flash_read( TFlash * flash, char * buf, uint32 size )
{
	//flash_rawread()
}

uint32 flash_write( TFlash * flash, char * buf, uint32 len )
{
	//flash_rawwrite
}
*/
void flash_rawread( uint32 flashaddr, uint32 memaddr, uint32 size )
{
	// TODO
}

void flash_rawwrite( uint32 flashaddr, uint32 memaddr, uint32 size )
{
	// TODO
}

