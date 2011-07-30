/******************************************************************************
 * @author zhangwei on 20070130
 * this module implements the openframe interface
 * the structure of openframe is the same as 802.15.4 frame format. thus it can 
 * be easlily ported to other platforms that support TI/Chipcon cc2420 transceivers.
 * 
 * though this module is not related to hardware. it is still placed into HAL
 * because "hal_cc2420" will use it.
 *  
 * @history
 * @modified by zhangwei on 200905
 *	- almost revise all. now with new interface and TiIoBuffer
 *****************************************************************************/ 

/******************************************************************************
 * IEEE 802.15.4 PPDU format
 * [4B Preamble][1B SFD][7b Framelength, 1b Reserved][nB PSDU/Payload]
 * 
 * IEEE 802.15.4 MAC DATA format (the payload of PHY frame)
 * Beacon Frame
 * [2B Frame Control] [1B Sequence Number][4 or 10 Address][2 Superframe Specification]
 * 		[k GTS fields][m Padding address fields] [n Beacon payload][2 FCS]
 * 
 * Data Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][n Data Payload][2 FCS]
 * 
 * ACK Frame
 * [2B Frame Control] [1B Sequence Number][2 FCS]
 * 
 * MAC Control Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 ADdress][1 Command Type][n Command Payload][2 FCS]
 *
 * Frame Control
 * b2b1b0  	frame type 000 beacon, 001 data 010 ACK 011 command 100-111 reserved
 * b12b13 	reserved.
 * 
 *****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "../rtl/rtl_iobuf.h"

#define TiFrameBuffer TiIoBuffer

#define phy_frame_init(iob) buf_setskiplen(iob,0)
#define phy_frame_length(iob) buf_datalen(iob)
#define phy_frame_body(iob) buf_body(iob)
#define phy_frame_full(iob) buf_full(iob)
#define phy_frame_empty(iob) buf_empty(iobuf)
#define phy_frame_read(iob,mem,size) buf_read(iob,mem,size)
#define phy_frame_write(iob,mem,len) buf_write(iob,mem,len)
#define phy_frame_append(iob,mem,len) buf_append(iob,mem,len)
#define phy_frame_clear(iob) buf_clear(iob)

#define mac_frame_init(iob) buf_setskiplen(iob,0)
#define mac_frame_length(iob) buf_datalen(iob)
#define mac_frame_data(iob) buf_data(iob)
#define mac_frame_header(iob) buf_header(iob)
#define mac_frame_body(iob) buf_body(iob)
#define mac_frame_full(iob) buf_full(iob)
#define mac_frame_empty(iob) buf_empty(iobuf)
#define mac_frame_read(iob,mem,size) buf_read(iob,mem,size)
#define mac_frame_write(iob,mem,len) buf_write(iob,mem,len)
#define mac_frame_append(iob,mem,len) buf_append(iob,mem,len)
#define mac_frame_clear(iob) buf_clear(iob)

/* frame type definition in 802.15.4 protocol */

#define MAC_FRAME_BEACON        0x00 
#define MAC_FRAME_DATA          0x01
#define MAC_FRAME_ACK           0x02
#define MAC_FRAME_COMMAND       0x03

uint8 mac_frame_type( TiFrameBuffer * iob );
void mac_frame_set_type( TiFrameBuffer * iob, uint8 newtype );
uint16 mac_frame_control( TiFrameBuffer * iob );
char * mac_frame_control_ptr( TiFrameBuffer * iob );
void mac_frame_set_control( TiFrameBuffer * iob, uint16 control );
uint8 mac_frame_sequence( TiFrameBuffer * iob );
void mac_frame_set_sequence( TiFrameBuffer * iob );
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






/* the following is obsolete source codes */

/* the header size of a TiOpenFrame
 * note that this header is start from the length byte to nodeid byte
 * it is 8  byte. = sizeof(TiOpenFrame) - payload size
 */
#define OPF_HEADER_SIZE 8

/* you cannot change following defination. they are defined according to 
 * IEEE 802.15.4 specification. RTS and CTS are exception!
 */  
#define OPF_TYPE_BEACON 0x00
#define OPF_TYPE_DATA 	0x01
#define OPF_TYPE_ACK	0x02
#define OPF_TYPE_MACCMD 0x03
#define OPF_TYPE_RTS	0x04
#define OPF_TYPE_CTS	0x05
#define OPF_TYPE_RESERVED 0x06	

typedef struct{
  uint8 len;
  char value[20];
}TiOpenAddress;

uint16 mac_getshortpan( TiOpenAddress * addr );
uint16 mac_getshortid( TiOpenAddress * addr );
void   mac_setshortpan( TiOpenAddress * addr, uint16 pan );
void   mac_setshortid( TiOpenAddress * addr, uint16 id );

/* the maximum frame length according to IEEE 802.15.4 specification
 */  
#define OPF_MAX_PSDU_SIZE 0x7F 
#define OPF_FRAME_SIZE 0x7E
#define OPF_PAYLOAD_SIZE (OPF_FRAME_SIZE-8)

/* @attention
 * due to the __packed keyword, the source code can only be compiled with 
 * ARM compiler (provided in ADS)
 *
 * _packed is a ARMCC keyword. it indicates the compiler arrange the member variables
 * in the following structure byte by byte. you should attention this if you 
 * want to port OpenMAC to other platform and compiles.
 */
typedef struct{
    uint8 	length;
    uint16 	control;
    uint8  	seqid;
    uint16 	panid;
    uint16  nodeto;
    uint16 	nodefrom;
    uint8  	payload[OPF_PAYLOAD_SIZE];
    uint16  footer;
}TiOpenFrame;

TiOpenFrame * opf_init( char * buf, uint8 size );
uint8  opf_type( char * buf );
uint8  opf_length( char * buf );
uint16 opf_control( char * buf );
uint8  opf_seqid( char * buf );
uint16 opf_addrfrom( char * buf );
uint16 opf_addrto( char * buf );
uint8  opf_command( char * buf );

void   opf_settype( char * buf, uint8 type );
void   opf_setlength( char * buf, uint8 length );
void   opf_setpanid( char * buf, uint16 panid );
void   opf_setaddrfrom( char * buf, uint16 addrfrom );
void   opf_setaddrto( char * buf, uint16 addrto );

//char * opf_length_addr( char * buf );
//char * opf_control_addr( char * buf );
//char * opf_seqid_addr( char * buf );
char * opf_panid( char * buf);
char * opf_addrfrom_addr( char * buf );
char * opf_addrto_addr( char * buf );
//char * opf_command_addr( char * buf );

#define PSDU(openframe) ((char*)openframe + 1)
#define MSDU(openframe) ((char*)openframe+10)
//#define NSDU(framebuf) ((char*)framebuf+?)

//char * opf_psdu( char * buf );
char * opf_msdu( char * buf );
// char * opf_nsdu( char * buf );
char * opf_packet( char * buf );
