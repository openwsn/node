#include "rtl_configall.h"
#include <stdlib.h>
#include "rtl_foundation.h"
#include "rtl_xtpaddress.h"
#include "rtl_assert.h"
/*******************************************************************************
 * rtl_xtpvar
 *
 * @modified by zhangwei on 2010.05.01
 *	- first created
 * @modified by yanshixing on 2010.05.09
 *  - finish the implement, not test.
 ******************************************************************************/

TiXtpAddress * xtp_addr_create( uintx size )
{
	TiXtpAddress * addr = (TiXtpAddress *)malloc( size );
	xtp_addr_construct( addr, size );
	return addr;
}

void xtp_addr_free( TiXtpAddress * addr )
{
	xtp_addr_destroy( addr );
	free( addr );
}

TiXtpAddress * xtp_addr_construct( void * mem, uintx memsize )
{
	
	rtl_assert( sizeof(TiXtpAddress) <= memsize );
	memset( mem, 0x00, memsize );
	TiXtpAddress * addr = (TiXtpAddress *)mem;
	addr->desc = 0;
	return addr;
}

void xtp_addr_destroy(TiXtpAddress * addr)
{
	
}

//
uint8 xtp_addr_type( TiXtpAddress * addr )
{
	return addr->desc;
}

void xtp_addr_set_type( TiXtpAddress * addr, uint8 type)
{
	addr->desc = type;
}

uint8 xtp_addr_nodelength( uint8 addrdesc )
{
	uint8 len;
	switch( addrdesc&0x07 )
	{
		case 1:
			len = 4;
 			break;
		case 2:
			len = 16;
			break;
		case 3:
			len = 8;
			break;
		case 4:
			len = 8;
			break;
		default:
			len = 0;
	}
	return len;

}
char * xtp_addr_value( TiXtpAddress * addr, uint8 * len )
{
	*len = xtp_addr_nodelength(addr->desc);

	if( (addr->desc&0x80) == 1)//if there is the net addr, it should be added.
	{
		*len =  *len + 4;//Q: what is the default length of the net addr. I assume it 4.
	}
	return &addr->addrbuf[0];
}

// len must less than 16 byte and be one of 4, 8, 16.
// and the order in the value must correspond to document.
void xtp_addr_setvalue( TiXtpAddress * addr, char * value, uint8 len ){
	memmove(addr->addrbuf, value, len);
}

// Question: how to arrange address byte? 
// I have negotiate with Zhu Jie. addrbuf[0-3] should be device/ipv4 addr, and addrbuf[4-7] should be net addr.
// And for "192.168.1.1", byte[0] is "192" and byte[2] is 168
void xtp_addr_set_ipv4( TiXtpAddress * addr, uint32 net, uint32 ipv4 )
{
	addr->addrbuf[0] = (ipv4&0xFF000000)>>24;
	addr->addrbuf[1] = (ipv4&0x00FF0000)>>16;
	addr->addrbuf[2] = (ipv4&0x0000FF00)>>8;
	addr->addrbuf[3] = (ipv4&0x000000FF);

	addr->addrbuf[4] = (net&0xFF000000)>>24;
	addr->addrbuf[5] = (net&0x00FF0000)>>16;
	addr->addrbuf[6] = (net&0x0000FF00)>>8;
	addr->addrbuf[7] = (net&0x000000FF);

}


char * xtp_addr_get_nodeptr( TiXtpAddress * addr, uint8 * len )
{
	*len = xtp_addr_nodelength(addr);
	return &addr->addrbuf[0];
}

char * xtp_addr_get_netptr( TiXtpAddress * addr, uint8 * len )
{
	*len = 4;//Q: what is the default length of the net addr. I assume it 4.
	return &addr->addrbuf[4];
}
