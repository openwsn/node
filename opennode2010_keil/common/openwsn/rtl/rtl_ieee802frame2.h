/**
 * IEEE802.2 Logic Link Layer Frame
 *
 * LLC PDU Format
 *  +-----------------+-----------------+------------------+----------------+
 *  | DSAP Address 1B | SSAP Address 1B | Control 1B or 2B | Information nB |
 *  +-----------------+-----------------+------------------+----------------+
 * 
 *  DSAP Address: destination service access point address field
 *  SSAP Address: source service access point address field
 *  Control: 16bits include sequence numbering, and 8bits for formats that do not
 *  Information: information field
 *  
 * reference
 *  IEEE 802.11 specification(version 1998), http://wenku.baidu.com/view/102219360b4c2e3f5727633b.html
 */
 

/**
 * IEEE 802.2 Frame Descriptor
 *
 * member variable
 *  total_length: real data length inside the buffer.
 *  datalen: value of the length byte in the frame. 
 */

// todo

typedef struct{
  char *    buf;
  uint8 *   capacity;
  uint8 *   dsap;
  uint8 *   ssap;
  uint8 *   control;
  uint8 *   payload;
}TiIEEE802Frame2Descriptor;

TiIEEE802Frame2Descriptor * ieee802frame2_open( TiIEEE802Frame2Descriptor * desc );
void ieee802frame2_close( TiIEEE802Frame2Descriptor * desc );
void ieee802frame2_clear( TiIEEE802Frame2Descriptor * desc );

TiIEEE802Frame2Descriptor * ieee802frame2_format( TiIEEE802Frame2Descriptor * desc, char * buf, uint8 capacity, uint16 ctrl );
char * ieee802frame2_assemble( TiIEEE802Frame2Descriptor * desc, uint8 * total_length );
bool ieee802frame2_parse( TiIEEE802Frame2Descriptor * desc, char * buf, uint8 len );

inline char * ieee802frame2_buffer( TiIEEE802Frame2Descriptor * desc ) 
{
	return ((desc)->buf);
}

inline char * ieee802frame2_dataptr( TiIEEE802Frame2Descriptor * desc ) 
{
	return ((desc->count == 0) ? NULL : desc->buf);
}
