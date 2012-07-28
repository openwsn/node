#ifndef _RTL_IEEE802FRAME154_H_476G_
#define _RTL_IEEE802FRAME154_H_476G_

/**
 * rtl_ieee802frame154
 * this module helps to deal with the parsing and format/assemble of 802.15.4 frames.
 * if you have already received an frame inside buffer, you can call parse. if you 
 * want to assemble a frame inside specified buffer, you can first call format then 
 * aseemble.
 * 
 * @modified by zhangwei on 2011.08.26
 * - revision: ieee802frame154_check_crc()
 * @modified by Shi Zhirong on 2012.07.24
 * - revision: ieee802frame154_set_ack_request()
 */

#include "rtl_configall.h"
#include "rtl_foundation.h"

/*******************************************************************************
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
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][1 Command Type][n Command Payload][2 FCS]
 *
 * Frame Control
 * b2b1b0  	frame type 000 beacon, 001 data 010 ACK 011 command 100-111 reserved
 * b12b13 	reserved.
 * 
 ******************************************************************************/

/* the maximum frame length according to IEEE 802.15.4 specification 
 * frame size includes the length byte itself  */ 

/* FRAME154_PSDU_SIZE				     
 * the maximum mac layer PSDU size is 127. it doesn't include the length byte which
 * is part of the physical layer.
 *
 * FRAME154_ACK_FRAME_SIZE
 * The size of the ACK frame, including the frame length byte. 
 */
#define I802F154_PSDU_SIZE                   0x7F
#define I802F154_MAX_FRAME_LENGTH            (FRAME154_PSDU_SIZE+1) 
#define I802F154_MIN_FRAME_LENGTH            5
#define I802F154_ACK_FRAME_SIZE              6

#define I802F154_BROADCAST_PAN               0xFFFF
#define I802F154_BROADCAST_ADDRESS           0xFFFF

#define I802F154_MAKEWORD(highbyte,lowbyte)  ((((uint16)(highbyte))<<8) | ((uint16)(lowbyte)))
 
// todo: The following macros will be removed. pls use the above instead

#define FRAME154_PSDU_SIZE				     0x7F 
#define FRAME154_MAX_FRAME_LENGTH            (FRAME154_PSDU_SIZE+1) 
#define FRAME154_MIN_FRAME_LENGTH            5
#define FRAME154_ACK_FRAME_SIZE              6

#define FRAME154_BROADCAST_PAN               0xFFFF
#define FRAME154_BROADCAST_ADDRESS           0xFFFF

#define I802F154_MAKEWORD(highbyte,lowbyte)  ((((uint16)(highbyte))<<8) | ((uint16)(lowbyte)))
#define FRAME154_MAKEWORD(highbyte,lowbyte)  I802F154_MAKEWORD((highbyte),(lowbyte))


/* todo
 * FCF_xxx macros will be replaced by new FRAME154_xxx macros in the future. 2009.11
 */

/* Format of the frame control field (FCF)
 * according to IEEE 802.15.4 MAC standard
 *
 * bits 0-2		frame type
 *		3		security enabled
 *		4		frame pending
 *		5		acknowledge request
 *		6		intra pan
 *		7-9		reserved
 *		10-11	destination addressing mode
 *		12-13	reserved
 *		14-15	source addressing mode
 * 
 * the security bit, frame pending bit in our implementation is always 0. 
 * 
 * Intra-PAN Subfield 
 * The intra-PAN subfield is 1 bit in length and specifies whether the MAC frame 
 * is to be sent within the same PAN (intra-PAN) or to another PAN (inter-PAN). 
 * If this subfield is set to 1 and both destination and source addresses are 
 * present, the frame shall not contain the source PAN identifier field. If this 
 * subfield is set to 0 and both destination and source addresses are present, 
 * the frame shall contain both destination and source PAN identifier fields.
 *
 * Addressing mode 
 *		b1b0 = 00  pan identifier and address field are not present
 *		b1b0 = 01  reserved
 *		b1b0 = 10  address field contains 16 bit address
 *      b1b0 = 11  address field contains 64bit extended address
 */

#define FCF_FRAMETYPE_MASK          0x0007
#define FCF_SECURITY_MASK           0x0008
#define FCF_FRAME_PENDING_MASK      0x0010
#define FCF_ACK_REQUEST_MASK        0x0020
#define FCF_INTRA_PAN_MASK          0x0040
#define FCF_DEST_ADDRMODE_MASK      0x0C00
#define FCF_SOURCE_ADDRMODE_MASK    0xC000

#define FCF_FRAMETYPE(fcf)			(((fcf) & FCF_FRAMETYPE_MASK))
#define FCF_SECURITY(fcf)           (((fcf) & FCF_SECURITY_MASK) >> 3)
#define FCF_FRAME_PENDING(fcf)      (((fcf) & FCF_FRAME_PENDING_MASK) >> 4)
#define FCF_ACK_REQUEST(fcf)        (((fcf) & FCF_ACK_REQUEST_MASK) >> 5)
#define FCF_INTRA_PAN(fcf)          (((fcf) & FCF_INTRA_PAN_MASK) >> 6)
#define FCF_DEST_ADDRMODE(fcf)      (((fcf) & FCF_DEST_ADDRMODE_MASK) >> 10)
#define FCF_SOURCE_ADDRMODE(fcf)    (((fcf) & FCF_SOURCE_ADDRMODE_MASK) >> 14)

//#define FCF_GET_ACK_REQUEST(fcf) (((fcf) & FCF_ACK_REQUEST_MASK) >> 5)
//#define FCF_CLR_ACK_REQUEST(fcf) ((fcf) & (~FCF_ACK_REQUEST_MASK))
//#define FCF_SET_ACK_REQUEST(fcf) ((fcf) | FCF_ACK_REQUEST_MASK)


/* frame type definition in 802.15.4 protocol. the other values are reserved 
 * for future using */

#define FCF_FRAMETYPE_BEACON        0x00 
#define FCF_FRAMETYPE_DATA          0x01
#define FCF_FRAMETYPE_ACK           0x02
#define FCF_FRAMETYPE_COMMAND       0x03
#define FCF_FRAMETYPE_UNKNOWN       0x04



/* frame type used by openwsn itself. it's not part of the 802.15.4 frame specification */

// #define FCF_FRAMETYPE_RTS	        0x04
// #define FCF_FRAMETYPE_CTS	        0x05

// #define FRAME154_ACK                     0xF0
// #define FRAME154_RTS                     0xF2
// #define FRAME154_CTS                     0xF3

/* default frame control in IEEE 802.15.4 specification
 * frame control: frame type unknown; no security; no frame pending; ack request;
 *			   not intra pan. dest addressing mode uses 2B pan id and 2B short 
 *			   address.
 * 
 * bit 15-0:
 *  {b15/b14	= 10 using short address} 
 *  {b13/b12    = xxx reserved}
 *  {b11/b10    = 10 using short address} 
 *  {b9/b8/b7   = xxx reserved}
 *  {b6         = 1 intra pan. affects the address format}
 *  {b5         = 0 no ack request 1 ack request}
 *  {b4         = 0 frame pending}
 *	{b3         = 0 security disabled}
 *  {b2/b1/b0   = 000 frame type}
 * 
 * the following two macros are help to determine the frame control (FCF) value. 
 * we use the following address format:
 *   [2B Dest PAN] [2B Dest Short Address] [2B Source Short Address ]
 * 
 * OPF is the abbrviation of "TiOpenFrame"
 * DEF is the abbrviation of "default"
 * 
 * Reference
 *	- MAC Frame Format, in IEEE 802.15.4 Standard, Page 111.
 */

#define FRAME154_FCF_BEACON_TYPE_BITS            (0x0000)
#define FRAME154_FCF_DATA_TYPE_BITS              (0x0001)
#define FRAME154_FCF_ACK_TYPE_BITS               (0x0002)
#define FRAME154_FCF_COMMAND_TYPE_BITS           (0x0003)
#define FRAME154_FCF_SECURITY_ENABLED_BIT        (0x0001 << 3)
#define FRAME154_FCF_FRAME_PENDING_BIT           (0x0001 << 4)
#define FRAME154_FCF_ACK_REQUEST_BIT             (0x0001 << 5)
#define FRAME154_FCF_INTRA_PAN_BIT               (0x0001 << 6)
#define FRAME154_FCF_DESTADDR_NONE_BITS          (0x0000 << 10)
#define FRAME154_FCF_DESTADDR_SHORT16_BITS       (0x000A << 10)
#define FRAME154_FCF_DESTADDR_EXT64_BITS         (0x000B << 10)
#define FRAME154_FCF_SOURCEADDR_NONE_BITS        (0x0000 << 14)
#define FRAME154_FCF_SOURCEADDR_SHORT16_BITS     (0x000A << 14)  // todo: is it right? why compile complains?
#define FRAME154_FCF_SOURCEADDR_EXT64_BITS       (0x000B << 14)  // todo: is it right? why compile complains?

//#define FRAME154_DEF_FRAMECONTROL_BEACON         (FRAME154_FCF_SOURCEADDR_SHORT16_BITS | FRAME154_FCF_DESTADDR_SHORT16_BITS)
//#define FRAME154_DEF_FRAMECONTROL_DATA           (FRAME154_DEF_FRAMECONTROL_DATA_ACK_BITS)
//#define FRAME154_DEF_FRAMECONTROL_DATA_ACK       (FRAME154_FCF_SOURCEADDR_SHORT16_BITS | FRAME154_FCF_DESTADDR_SHORT16_BITS | FRAME154_FCF_ACK_REQUEST_BITS | FRAME154_FCF_DATA_TYPE_BITS)
//#define FRAME154_DEF_FRAMECONTROL_DATA_NOACK     (FRAME154_FCF_SOURCEADDR_SHORT16_BITS | FRAME154_FCF_DESTADDR_SHORT16_BITS | FRAME154_FCF_DATA_TYPE_BITS)
//#define FRAME154_DEF_FRAMECONTROL_COMMAND        (FRAME154_FCF_SOURCEADDR_SHORT16_BITS | FRAME154_FCF_DESTADDR_SHORT16_BITS | FRAME154_FCF_ACK_REQUEST_BITS | FRAME154_FCF_COMMAND_TYPE_BITS)
//#define FRAME154_DEF_FRAMECONTROL_ACK            (FRAME154_FCF_SOURCEADDR_SHORT16_BITS | FRAME154_FCF_DESTADDR_SHORT16_BITS | FRAME154_FCF_ACK_REQUEST_BITS | FRAME154_FCF_ACK_TYPE_BITS)

#define FRAME154_DEF_FRAMECONTROL_BEACON         0x8800
#define FRAME154_DEF_FRAMECONTROL_DATA			 FRAME154_DEF_FRAMECONTROL_DATA_ACK
#define FRAME154_DEF_FRAMECONTROL_DATA_NOACK     0x8801
#define FRAME154_DEF_FRAMECONTROL_DATA_ACK       0x8821
#define FRAME154_DEF_FRAMECONTROL_COMMAND        0x8803   
#define FRAME154_DEF_FRAMECONTROL_ACK            0x8802
#define FRAME154_FRAMECONTROL_UNKNOWN            0x0000

#define FRAME154_GET_ACK_REQUEST(fcf)            (((fcf) & 0x0020) >> 5)
#define FRAME154_CLR_ACK_REQUEST(fcf)            ((fcf) &= (~0x0020))
#define FRAME154_SET_ACK_REQUEST(fcf)            ((fcf) |= 0x0020)


//#define FRAME154_OPTION_CRC                    (0x01 << 0)
//#define FRAME154_DEF_OPTION                    (1 << FRAME154_OPTION_CRC)
#define FRAME154_DEF_OPTION                      0x00

/* TiOpenFrame 
 * denotes the network PHY or MAC layer frame. 
 *	
 *  - memsize                 size of the memory block allocated to TiOpenFrame object
 *  - buf                     frame buffer
 *	- size 			          size of the frame buffer. buffer is the memory block holding the frame.
 *  - datalen                 length of the data in the frame buffer. datalen and 
 *                            frame length are different. 
 *  - type                    frame type. If "ctrl" isn't empty, then the type can 
 *                            be extracted from the frame control field.
 *  - option                  option parameter controls some detail behavior of 
 *                            TiOpenFrame object.
 *                            bit 0 = 0 No ACK, 1 ACK 
 *                            bit 1 = 0 No CRC, 1 Automatic CRC
 *  - psdu                    start address of the PHY layer service data unit(SDU)
 *  - psdu_len                = buf[0]. It's the PHY service data unit.
 *  - msdu                    start address of the MAC layer service data unit(SDU)
 *  - msdu_len                length of the MSDU
 *
 * Difference among opf->size/opf_size(opf), opf_datalen() and opf_framelength():
 *  - opf_size(): how many bytes can be placed into the internal buffer
 *  - opf_datalen(): how many bytes really placed into the internal buffer
 *  - opf_framelength(): frame length. 
 *      opf_framelength() <= opf_datalen() <= opf_size()
 *  - psdu_len: length of PSDU. PSDU doesn't includes the first length byte in the buffer.
 *  - msdu_len: length of MSDU.
 *
 * On Frame Control Field (FCF)
 *  bits    [0-2]	frame type
 *			[3]		security enabled
 *			[4]		frame pending flag
 *			[5]		ack request
 *			[6]		intra pan
 *			[7-9]	reserved
 *			[10-11] dest. addressing mode
 *			[12-13] reserved
 *			[14-15]	source. addressing mode
 * 
 * On Frame Length
 *	According to cc2420 datasheet, the first byte in the RXFIFO is the frame length. 
 * It defines the number of bytes in MPDU. Note that the length field doesn't include
 * the length field itself. It does however include FCS (Frame Check Sequence), 
 * even if this is inserted automatically by cc2420 hardware. It also includes the
 * MIC if authentification is used.
 *
 * On TiOpenFrame->len
 *   TiOpenFrame->len means the length of the data inside the internal buffer. 
 * For 802.15.4 frames, the first byte is defined as the frame length (not include
 * the length byte itself), and TiOpenFrame->len should equal to TiOpenFrame->buf[0] + 1.
 * If the TiOpenFrame->len < TiOpenFrame->buf[0] + 1, then the frame inside the 
 * buffer isn't complete.
 *
 */

/* @attention
 * due to the __packed keyword, the source code can only be compiled with 
 * ARM compiler (provided in ADS)
 *
 * _packed is a ARMCC keyword. it indicates the compiler arrange the member variables
 * in the following structure byte by byte. you should attention this if you 
 * want to port OpenMAC to other platform and compiles.
 */

// #prgama align 1


/**
 * IEEE 802.15.4 Frame Descriptor. The variable of this type describe the IEEE 802.15.4
 * frame buffer organization. It helps the parsing and assembling of a standard 802.15.4
 * frame inside byte buffer. 
 *
 * member variable
 *  total_length: real data length inside the buffer.
 *  datalen: value of the length byte in the frame. 
 */
#pragma pack(1) 
typedef struct{
  char *    buf;
  uint8     capacity;
  uint8     count;                    
  uint8     option;                  
  char *    ctrl;					// 2B
  char *    sequence;				// 1B
  char *    panto;					// 2B
  char *    shortaddrto;			// 2B
  char *    longaddrto;				// 8B
  char *    panfrom;				// 2B
  char *    shortaddrfrom;			// 2B
  char *    longaddrfrom;			// 2B
  char *    superframe_spec;		// 2B
  char *    gts;                    // 1B
  uint8 *   gts_len;                // 1B
  char *    address_padding;        // 1B
  uint8     pad_len;                // 1B
  char *    cmdtype;                // 1B
  char *    msdu;
  uint8     msdu_len;
  char *    fcs;					// 2B
}TiIEEE802Frame154Descriptor;

TiIEEE802Frame154Descriptor * ieee802frame154_open( TiIEEE802Frame154Descriptor * desc );
void ieee802frame154_close( TiIEEE802Frame154Descriptor * desc );
void ieee802frame154_clear( TiIEEE802Frame154Descriptor * desc );

TiIEEE802Frame154Descriptor * ieee802frame154_format( TiIEEE802Frame154Descriptor * desc, char * buf, uint8 capacity, uint16 ctrl );
char * ieee802frame154_assemble( TiIEEE802Frame154Descriptor * desc, uint8 * total_length );
bool ieee802frame154_parse( TiIEEE802Frame154Descriptor * desc, char * buf, uint8 len );

inline char * ieee802frame154_buffer( TiIEEE802Frame154Descriptor * desc ) 
{
	return ((desc)->buf);
}

inline char * ieee802frame154_dataptr( TiIEEE802Frame154Descriptor * desc ) 
{
	return ((desc->count == 0) ? NULL : desc->buf);
}

inline uint8 ieee802frame154_size( TiIEEE802Frame154Descriptor * desc )
{
	return ((desc)->capacity);
}

inline uint8 ieee802frame154_length( TiIEEE802Frame154Descriptor * desc )
{
	return ((desc)->count);
}

inline void ieee802frame154_set_length( TiIEEE802Frame154Descriptor * desc, uint8 len ) 
{
	desc->count = len;
}

inline uint8 ieee802frame154_framelength( TiIEEE802Frame154Descriptor * desc ) 
{
	return (desc->buf[0]);
}

inline void ieee802frame154_set_framelength( TiIEEE802Frame154Descriptor * desc, uint8 len ) 
{
	desc->buf[0] = len;
}

inline bool ieee802frame154_full( TiIEEE802Frame154Descriptor * desc ) 
{
	return (desc->count == desc->capacity);
}

inline bool ieee802frame154_empty( TiIEEE802Frame154Descriptor * desc )
{
	return (desc->count==0);
}

inline uint16 ieee802frame154_control( TiIEEE802Frame154Descriptor * desc )
{
    return FRAME154_MAKEWORD( desc->buf[2], desc->buf[1] );
}

inline void ieee802frame154_set_control( TiIEEE802Frame154Descriptor * desc, uint16 ctrl )
{
	desc->buf[1] = (char)(ctrl & 0xFF);
	desc->buf[2] = (char)((ctrl >> 8) & 0xFF);
}

inline uint8 ieee802frame154_type( TiIEEE802Frame154Descriptor * desc )
{
	return FCF_FRAMETYPE( ieee802frame154_control(desc) );
}

inline void ieee802frame154_set_type( TiIEEE802Frame154Descriptor * desc, uint8 newtype ) 
{
	desc->buf[1] &= (~FCF_FRAMETYPE_MASK);
	desc->buf[1] |= (newtype & FCF_FRAMETYPE_MASK);
}

inline void ieee802frame154_set_ack_request( TiIEEE802Frame154Descriptor * desc, bool enabled )
{
    uint16 fcf = ieee802frame154_control(desc);
    if (enabled)
        fcf |= 0x0020;
    else
        fcf &= (~0x0020);
    ieee802frame154_set_control(desc, fcf);
}

inline void ieee802frame154_set_crc( TiIEEE802Frame154Descriptor * desc, bool enabled ) 
{
    // todo
    return;
}

inline uint8 ieee802frame154_sequence( TiIEEE802Frame154Descriptor * desc )
{
	rtl_assert( desc->sequence != NULL );
	return *(desc->sequence);
}

inline void ieee802frame154_set_sequence( TiIEEE802Frame154Descriptor * desc, uint8 seqid )
{
    if (desc->sequence)
        *(desc->sequence) = seqid;
}

inline uint16 ieee802frame154_panto( TiIEEE802Frame154Descriptor * desc )
{
	rtl_assert( desc->panto != NULL );
	return FRAME154_MAKEWORD( desc->panto[1], desc->panto[0] );
}

inline void ieee802frame154_set_panto( TiIEEE802Frame154Descriptor * desc, uint16 panid )
{
    if (desc->panto)
    {
	    desc->panto[0] = panid & 0xFF;
	    desc->panto[1] = (panid >> 8) & 0xFF;
    }
}

inline uint16 ieee802frame154_shortaddrto( TiIEEE802Frame154Descriptor * desc )
{
    if (desc->shortaddrto)
        return FRAME154_MAKEWORD( desc->shortaddrto[1], desc->shortaddrto[0] );
    else
        return 0;
}

inline void ieee802frame154_set_shortaddrto( TiIEEE802Frame154Descriptor * desc, uint16 addr )
{
    if (desc->shortaddrto)
    {
	    desc->shortaddrto[0] = addr & 0xFF;
        desc->shortaddrto[1] = (addr >> 8) & 0xFF;
    }
}

inline uint16 ieee802frame154_panfrom( TiIEEE802Frame154Descriptor * desc )
{
	if (desc->panfrom)
	    return FRAME154_MAKEWORD( desc->panfrom[1], desc->panfrom[0] );
    else
        return 0;
}

inline void ieee802frame154_set_panfrom( TiIEEE802Frame154Descriptor * desc, uint16 panid )
{
    if (desc->panfrom)
    {
	    desc->panfrom[0] = panid & 0xFF;
	    desc->panfrom[1] = (panid >> 8) & 0xFF;
    }
}

inline uint16 ieee802frame154_shortaddrfrom( TiIEEE802Frame154Descriptor * desc )
{
	if (desc->shortaddrfrom)
	    return FRAME154_MAKEWORD( desc->shortaddrfrom[1], desc->shortaddrfrom[0] );
    else
        return 0;
}

inline void ieee802frame154_set_shortaddrfrom( TiIEEE802Frame154Descriptor * desc, uint16 addr )
{
	if (desc->shortaddrfrom)
    {
	    desc->shortaddrfrom[0] = addr & 0xFF;
	    desc->shortaddrfrom[1] = (addr >> 8) & 0xFF;
    }
}

void ieee802frame154_swapaddress( TiIEEE802Frame154Descriptor * desc );

inline char * ieee802frame154_msdu( TiIEEE802Frame154Descriptor * desc )
{
	return desc->msdu;
}

inline bool ieee802frame154_msdu_len( TiIEEE802Frame154Descriptor * desc )
{
	return desc->msdu_len;
}

/* returns the RSSI byte in this frame */
inline uint8 ieee802frame154_rssi( TiIEEE802Frame154Descriptor * desc )
{
    if (desc->fcs)
        return desc->fcs[0];
    else
        return 0;
}

inline uint8 ieee802frame154_lqi( TiIEEE802Frame154Descriptor * desc )
{
    if (desc->fcs)
        return (desc->fcs[1] & 0x7F);
    else
        return 0;
}

/* Check the CRC result of the frame. 
 * For 802.15.4 Frame supported by cc2420 transceiver, the cc2420 hardware can compute
 * the CRC results. If (fcs[0] & 0x80) is 0x80, then it means CRC verification success. 
 */
inline bool ieee802frame154_check_crc( TiIEEE802Frame154Descriptor * desc )
{
    if (desc->fcs)
        return ((desc->fcs[1] & 0x80) > 0);
    else
        return 0;
}

#endif /* _RTL_IEEE802FRAME154_H_476G_ */

