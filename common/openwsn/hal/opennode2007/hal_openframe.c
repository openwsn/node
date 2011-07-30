
uint8 mac_frame_type( TiFrameBuffer * iob )
{
	char * header = mac_frame_header(iob);
	return (header[1] &= 0xF30);
}

void mac_frame_set_type( TiFrameBuffer * iob, uint8 newtype ) 
{
	char * header = mac_frame_header(iob);
	header[1] &= 0xF30;
	header[1] |= (newtype & 0xF30);
}

uint16 mac_frame_control( TiFrameBuffer * iob )
{
	char * header = mac_frame_header(iob);
	return (((uint16)header[0]) << 8) | (header[1]);
}

char * mac_frame_control_ptr( TiFrameBuffer * iob )
{
	return mac_frame_header(iob) + 0;
}

void mac_frame_set_control( TiFrameBuffer * iob, uint16 control )
{
	char * header = mac_frame_header(iob);
	header[0] = (uint8)(control >> 8);
	header[1] = (uint8)(control & 0x00FF);
}

uint8 mac_frame_sequence( TiFrameBuffer * iob )
{
	// todo
}

void mac_frame_set_sequence( TiFrameBuffer * iob )
{
	// todo
}

uint16 mac_frame_panid( TiFrameBuffer * iob )
{
	// todo
}

void mac_frame_set_panid( TiFrameBuffer * iob )
{
	// todo
}

char * mac_frame_addrfrom_ptr( TiFrameBuffer * iob )
{
	char * header = mac_frame_header(iob);
	char * addr;

	switch (mac_frame_type(iob))
	{
	case MAC_FRAME_DATA: 
		addr = header + 3;
		break;
	case MAC_FRAME_BEACON:
		addr = header + 3;
		break;
	case MAC_FRAME_ACK:
		addr = NULL;
		break;
	case MAC_FRAME_COMMAND:
		addr = header + 3;
		break;
	}

	return addr;
}

char * mac_frame_addrto_ptr( TiFrameBuffer * iob )
{
	// todo
}

char * mac_frame_longaddrfrom_ptr( TiFrameBuffer * iob )
{
	// todo
}

char * mac_frame_longaddrto_ptr( TiFrameBuffer * iob )
{
	// todo
}




















/* the following are obsolete source codes */

char * opt_ownerframe( char * packet )
{
	return packet-1;
}

char * opt_data( char * packet )
{
	return packet+8;
	
}



/*****************************************************************************
 * Address Manipulations
 * assume 802.15.4 specification address.
 * 
 * @TODO
 * the 802.15.4 address contains 20 byte at most. this is too complicated for 
 * research using. however, we still adopt this design. attention that we only
 * use 4 byte address in current implementation (20070130)  
 ****************************************************************************/ 

uint16 mac_getshortpan( TiOpenAddress * addr )
{
	uint16 * mem = (uint16 *)addr;
	return (* mem);
}

uint16 mac_getshortid( TiOpenAddress * addr )
{
	uint16 * mem = (uint16 *)((char*)addr+2);
	return (* mem);
}

void  mac_setshortpan( TiOpenAddress * addr, uint16 pan )
{
	uint16 * mem = (uint16 *)addr;
	*mem = pan;
}

void  mac_setshortid( TiOpenAddress * addr, uint16 id )
{
	uint16 * mem = (uint16 *)((char*)addr+2);
	*mem = id;
}

/*****************************************************************************
 * Frame Manipulations
 * assume 802.15.4 specification frame.
 ****************************************************************************/
 
TiOpenFrame * opf_init( char * buf, uint8 size )
{
	TiOpenFrame * opf;
	//assert( size <= OPF_FRAME_SIZE );
	opf = (TiOpenFrame *)buf;
	//opf->length = 0x7F & size;
	return opf;
}

/* refer to the specification of 802.15.4, page 112 */
uint8 opf_type( char * buf )
{
	uint8 type = (opf_control(buf) & 0x03);
	return type;
}

uint8 opf_length( char * buf )
{
	return buf[0] & 0x7F;
}

uint16 opf_control( char * buf )
{  
	uint16 * addr = (uint16 *)(buf);
	return *addr;
}

uint8 opf_seqid( char * buf )
{
	return buf[3];
}

uint16 opf_addrfrom( char * buf )
{
	uint16 * addr = (uint16 *)(buf + 8);
	return *addr;
}

uint16 opf_addrto( char * buf )
{
	uint16 * addr = (uint16 *)(buf + 6);
	return *addr;
}

uint8 opf_command( char * buf )
{
	return buf[8];
}

void opf_settype( char * buf, uint8 type )
{
	buf[2] |= 0x03;
}

void opf_setlength( char * buf, uint8 length )
{
	buf[0] = length & 0x7F;
}

void opf_setpanid( char * buf, uint16 panid )
{
	buf = opf_panid(buf); 
	*buf = (uint8)panid;
	*(buf+1) = (uint8)(panid >> 8);
}

void opf_setaddrfrom( char * buf, uint16 addrfrom )
{
	buf = opf_addrfrom_addr(buf); 
	*buf = (uint8)addrfrom;
	*(buf+1) = (uint8)(addrfrom >> 8);
}

void opf_setaddrto( char * buf, uint16 addrto )
{
	buf = opf_addrto_addr(buf); 
	*buf = (uint8)addrto;
	*(buf+1) = (uint8)(addrto >> 8);
}
/*
char * opf_length_addr( char * buf )
{
	return buf;
}

char * opf_control_addr( char * buf )
{
	return buf + 1;
}

char * opf_seqid_addr( char * buf )
{
	return buf + 3;
}
*/
char * opf_panid(char * buf)
{
	return buf + 4;
}

char * opf_addrfrom_addr( char * buf )
{
	return buf + 8;
}

char * opf_addrto_addr( char * buf )
{
	return buf + 6;
}

/*
char * opf_command_addr( char * buf )
{
	return buf + 8;
}
*/

/* payload address of PHY data unit
 * the payload of PHY frame is essentially the MAC layer frame. 
 */
 
 /*
char * opf_psdu( char * buf )
{
	return buf + 1; 
}
*/


/* payload address of MAC frame 
 * the payload of MAC frame is essentially the NET layer packet.
 */
char * opf_msdu( char * buf )
{
	return buf + 8;
}

/* returns the pointer to the TOpenPacket
 * @modified by zhangwei on 20070207 
 */
char * opf_packet( char * buf )
{
	return buf + 1;
}

