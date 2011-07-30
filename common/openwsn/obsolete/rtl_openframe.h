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
#ifndef _RTL_OPENFRAME_H_7824_ 
#define _RTL_OPENFRAME_H_7824_

/******************************************************************************
 * rtl_openframe
 * this module implements an openframe object to help manipulating the IEEE 802.15.4 frame 
 * buffer. 
 * 
 * architecture
 * char * buf => TiOpenFrame => TiOpenPacket => TiOpenData (which includes serveral TiOpenSection)
 *  
 * @author zhangwei on 20070130
 *	- first created.
 * 
 * @history
 * @modified by zhangwei on 20090719
 *	- revision
 * @modified by zhangwei on 20090821
 *	- revision. Change the TiOpenFrame->len to TiOpenFrame->datalen. The meaning 
 *    of it is the length of the data in the buffer. It's often equal to (framelength+1).
 *  - add member function opf_datalen(...) and opf_framelength().
 *	- not to suggest using opf->len directly.
 * @modified by zhangwei on 20100613
 *  - correct bugs in ACK bit operation
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
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][1 Command Type][n Command Payload][2 FCS]
 *
 * Frame Control
 * b2b1b0  	frame type 000 beacon, 001 data 010 ACK 011 command 100-111 reserved
 * b12b13 	reserved.
 * 
 *****************************************************************************/

#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "../hal/hal_debugio.h"

/* the maximum frame length according to IEEE 802.15.4 specification 
 * frame size includes the length byte itself  */ 

#define OPF_PSDU_SIZE				    0x7F 
#define OPF_MAX_FRAME_LENGTH            OPF_PSDU_SIZE 
#define OPF_MIN_FRAME_LENGTH            5
#define OPF_SUGGEST_SIZE                (sizeof(TiOpenFrame)+OPF_MAX_FRAME_LENGTH+1)
#define OPF_HOPESIZE(framelength)       (sizeof(TiOpenFrame)+(framelength)+1)

#define OPF_BROADCAST_PAN               0xFFFF
#define OPF_BROADCAST_ADDRESS           0xFFFF

#define OPF_MAKEWORD(highbyte,lowbyte)  ((((uint16)(highbyte))<<8) | ((uint16)(lowbyte)))


/* todo
 * FCF_xxx macros will be replaced by new OPF_xxx macros in the future. 2009.11
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

// #define OPF_ACK                     0xF0
// #define OPF_RTS                     0xF2
// #define OPF_CTS                     0xF3

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

#define OPF_FCF_BEACON_TYPE_BITS            (0x0000)
#define OPF_FCF_DATA_TYPE_BITS              (0x0001)
#define OPF_FCF_ACK_TYPE_BITS               (0x0002)
#define OPF_FCF_COMMAND_TYPE_BITS           (0x0003)
#define OPF_FCF_SECURITY_ENABLED_BIT        (0x0001 << 3)
#define OPF_FCF_FRAME_PENDING_BIT           (0x0001 << 4)
#define OPF_FCF_ACK_REQUEST_BIT             (0x0001 << 5)
#define OPF_FCF_INTRA_PAN_BIT               (0x0001 << 6)
#define OPF_FCF_DESTADDR_NONE_BITS          (0x0000 << 10)
#define OPF_FCF_DESTADDR_SHORT16_BITS       (0x000A << 10)
#define OPF_FCF_DESTADDR_EXT64_BITS         (0x000B << 10)
#define OPF_FCF_SOURCEADDR_NONE_BITS        (0x0000 << 14)
#define OPF_FCF_SOURCEADDR_SHORT16_BITS     (0x000A << 14)
#define OPF_FCF_SOURCEADDR_EXT64_BITS       (0x000B << 14)

//#define OPF_DEF_FRAMECONTROL_BEACON         (OPF_FCF_SOURCEADDR_SHORT16_BITS | OPF_FCF_DESTADDR_SHORT16_BITS)
//#define OPF_DEF_FRAMECONTROL_DATA           (OPF_DEF_FRAMECONTROL_DATA_ACK_BITS)
//#define OPF_DEF_FRAMECONTROL_DATA_ACK       (OPF_FCF_SOURCEADDR_SHORT16_BITS | OPF_FCF_DESTADDR_SHORT16_BITS | OPF_FCF_ACK_REQUEST_BITS | OPF_FCF_DATA_TYPE_BITS)
//#define OPF_DEF_FRAMECONTROL_DATA_NOACK     (OPF_FCF_SOURCEADDR_SHORT16_BITS | OPF_FCF_DESTADDR_SHORT16_BITS | OPF_FCF_DATA_TYPE_BITS)
//#define OPF_DEF_FRAMECONTROL_COMMAND        (OPF_FCF_SOURCEADDR_SHORT16_BITS | OPF_FCF_DESTADDR_SHORT16_BITS | OPF_FCF_ACK_REQUEST_BITS | OPF_FCF_COMMAND_TYPE_BITS)
//#define OPF_DEF_FRAMECONTROL_ACK            (OPF_FCF_SOURCEADDR_SHORT16_BITS | OPF_FCF_DESTADDR_SHORT16_BITS | OPF_FCF_ACK_REQUEST_BITS | OPF_FCF_ACK_TYPE_BITS)

#define OPF_DEF_FRAMECONTROL_BEACON         0x8800
#define OPF_DEF_FRAMECONTROL_DATA			OPF_DEF_FRAMECONTROL_DATA_ACK
#define OPF_DEF_FRAMECONTROL_DATA_NOACK     0x8801
#define OPF_DEF_FRAMECONTROL_DATA_ACK       0x8821
#define OPF_DEF_FRAMECONTROL_COMMAND        0x8803   
#define OPF_DEF_FRAMECONTROL_ACK            0x8802
#define OPF_FRAMECONTROL_UNKNOWN            0x0000

#define OPF_GET_ACK_REQUEST(fcf)            (((fcf) & 0x0020) >> 5)
#define OPF_CLR_ACK_REQUEST(fcf)            ((fcf) &= (~0x0020))
#define OPF_SET_ACK_REQUEST(fcf)            ((fcf) |= 0x0020)


//#define OPF_OPTION_CRC                    (0x01 << 0)
//#define OPF_DEF_OPTION                    (1 << OPF_OPTION_CRC)
#define OPF_DEF_OPTION                      0x00

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
typedef struct{
  uint8	    memsize;
  char *    buf;
  uint8     size;
  uint8     datalen;                // 1B
  uint8     option;                  
  uint8     type;
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
}TiOpenFrame;

#define opf_construct(buf,memsize,ctrl,option) opf_open(buf,memsize,ctrl,option)

TiOpenFrame *   opf_open( void * buf, uint16 memsize, uint16 ctrl, uint8 option );
void            opf_close( TiOpenFrame * opf );

void			opf_clear( TiOpenFrame * opf );
bool			opf_cast( TiOpenFrame * opf, uint8 len, uint16 ctrl );

inline bool		opf_format( TiOpenFrame * opf, uint8 len, uint16 ctrl )
{
	return opf_cast( opf, len, ctrl );
}

bool			opf_parse( TiOpenFrame * opf, uint8 len );
char *			opf_assemble( TiOpenFrame * opf, uint8 * total_length );
uint8           opf_copyfrom( TiOpenFrame * opf, TiOpenFrame * from );
uint8           opf_copyto( TiOpenFrame * opf, TiOpenFrame * to );

uint8			opf_read( TiOpenFrame * opf, char * buf, uint8 size );
uint8			opf_write( TiOpenFrame * opf, char * buf, uint8 len );
uint8			opf_append( TiOpenFrame * opf, char * data, uint8 len );

inline uint8 opf_size( TiOpenFrame * opf )
{
	return ((opf)->size);
}

inline char * opf_buffer( TiOpenFrame * opf ) 
{
	return ((opf)->buf);
}

inline char * opf_data( TiOpenFrame * opf ) 
{
	return ((opf->datalen == 0) ? NULL : opf->buf);
}

inline uint8 opf_datalen( TiOpenFrame * opf ) 
{
	return ((opf)->datalen);
}

inline void opf_set_datalen( TiOpenFrame * opf, uint8 len ) 
{
	(opf)->datalen = len;
}

inline uint8 opf_framelength( TiOpenFrame * opf ) 
{
	return (opf->buf[0]);
}

inline bool opf_full( TiOpenFrame * opf ) 
{
	return (opf->datalen == opf->size);
}

inline bool opf_empty( TiOpenFrame * opf )
{
	bool r = (opf->datalen==0);
	if (!r) 
		r = (opf->buf[0] == 0);
	return r;
}

inline uint16 opf_control( TiOpenFrame * opf )
{
    return OPF_MAKEWORD( opf->buf[2], opf->buf[1] );
}

inline void opf_set_control( TiOpenFrame * opf, uint16 ctrl )
{
	opf->buf[1] = ctrl & 0xFF;
	opf->buf[2] = ((ctrl >> 8) & 0xFF);
}

inline uint8 opf_type( TiOpenFrame * opf )
{
	return FCF_FRAMETYPE( opf_control(opf) );
}

inline void opf_set_type( TiOpenFrame * opf, uint8 newtype ) 
{
	opf->buf[1] &= (~FCF_FRAMETYPE_MASK);
	opf->buf[1] |= (newtype & FCF_FRAMETYPE_MASK);
}

inline uint8 opf_sequence( TiOpenFrame * opf )
{
	rtl_assert( opf->sequence != NULL );
	return *(opf->sequence);
}

inline void opf_set_sequence( TiOpenFrame * opf, uint8 seqid )
{
    if (opf->sequence)
        *(opf->sequence) = seqid;
}

inline uint16 opf_panto( TiOpenFrame * opf )
{
	rtl_assert( opf->panto != NULL );
	return OPF_MAKEWORD( opf->panto[1], opf->panto[0] );
}

inline void opf_set_panto( TiOpenFrame * opf, uint16 panid )
{
    if (opf->panto)
    {
	    opf->panto[0] = panid & 0xFF;
	    opf->panto[1] = (panid >> 8) & 0xFF;
    }
}

inline uint16 opf_shortaddrto( TiOpenFrame * opf )
{
    if (opf->shortaddrto)
        return OPF_MAKEWORD( opf->shortaddrto[1], opf->shortaddrto[0] );
    else
        return 0;
}

inline void opf_set_shortaddrto( TiOpenFrame * opf, uint16 addr )
{
    if (opf->shortaddrto)
    {
	    opf->shortaddrto[0] = addr & 0xFF;
        opf->shortaddrto[1] = (addr >> 8) & 0xFF;
    }
}

inline uint16 opf_panfrom( TiOpenFrame * opf )
{
	if (opf->panfrom)
	    return OPF_MAKEWORD( opf->panfrom[1], opf->panfrom[0] );
    else
        return 0;
}

inline void opf_set_panfrom( TiOpenFrame * opf, uint16 panid )
{
    if (opf->panfrom)
    {
	    opf->panfrom[0] = panid & 0xFF;
	    opf->panfrom[1] = (panid >> 8) & 0xFF;
    }
}

inline uint16 opf_shortaddrfrom( TiOpenFrame * opf )
{
	if (opf->shortaddrfrom)
	    return OPF_MAKEWORD( opf->shortaddrfrom[1], opf->shortaddrfrom[0] );
    else
        return 0;
}

inline void opf_set_shortaddrfrom( TiOpenFrame * opf, uint16 addr )
{
	if (opf->shortaddrfrom)
    {
	    opf->shortaddrfrom[0] = addr & 0xFF;
	    opf->shortaddrfrom[1] = (addr >> 8) & 0xFF;
    }
}

inline void _opf_swapbyte( char * b1, char * b2 )
{
	char tmp = *b1;
	*b1 = *b2;
	*b2 = tmp;
}

inline void opf_swapaddress( TiOpenFrame * opf )
{
	if ((opf->panto != NULL) && (opf->panfrom != NULL))
	{
		_opf_swapbyte( opf->panto, opf->panfrom );
		_opf_swapbyte( opf->panto+1, opf->panfrom+1 );
	}

	if ((opf->shortaddrto != NULL) && (opf->shortaddrfrom != NULL))
	{
		_opf_swapbyte( opf->shortaddrto, opf->shortaddrfrom );
		_opf_swapbyte( opf->shortaddrto+1, opf->shortaddrfrom+1 );

	}

	if ((opf->longaddrto != NULL) && (opf->longaddrfrom != NULL))
	{
		_opf_swapbyte( opf->longaddrto+0, opf->longaddrfrom+0 );
		_opf_swapbyte( opf->longaddrto+1, opf->longaddrfrom+1 );
		_opf_swapbyte( opf->longaddrto+2, opf->longaddrfrom+2 );
		_opf_swapbyte( opf->longaddrto+3, opf->longaddrfrom+3 );
		_opf_swapbyte( opf->longaddrto+4, opf->longaddrfrom+4 );
		_opf_swapbyte( opf->longaddrto+5, opf->longaddrfrom+5 );
		_opf_swapbyte( opf->longaddrto+6, opf->longaddrfrom+6 );
		_opf_swapbyte( opf->longaddrto+7, opf->longaddrfrom+7 );

	}
}

inline char * opf_msdu( TiOpenFrame * opf )
{
	return opf->msdu;
}

inline bool opf_msdu_len( TiOpenFrame * opf )
{
	return opf->msdu_len;
}

/* Returns the RSSI value in this frame */
inline uint8 opf_rssi( TiOpenFrame * opf )
{
    if (opf->fcs)
        return opf->fcs[0];
    else
        return 0;
}

inline uint8 opf_lqi( TiOpenFrame * opf )
{
    if (opf->fcs)
        return (opf->fcs[1] & 0x7F);
    else
        return 0;
}

/* Check the CRC result of the frame. 
 * For 802.15.4 Frame supported by cc2420 transceiver, the cc2420 hardware can compute
 * the CRC results. If (fcs[0] & 0x80) is 0x80, then it means CRC verification success. 
 * QQQ: ??? 0 or 1 means success?
 */
inline bool opf_check_crc( TiOpenFrame * opf )
{
    if (opf->fcs)
        return ((opf->fcs[1] && 0x80) == 1);
    else
        return 0;
}

#endif
