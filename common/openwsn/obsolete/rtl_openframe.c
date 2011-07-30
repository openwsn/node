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
/*******************************************************************************
 * @history
 * @modified by zhangwei on 20091120
 *	- Corrected bugs in opf_copyfrom() and opf_copyto. An assert was added into 
 * these two functions. And the memsize was saved before memmove to avoid override 
 * it during the memory copy.
 ******************************************************************************/

#include "rtl_configall.h"
#include <string.h>
#include "../hal/hal_debugio.h"
#include "rtl_foundation.h"
#include "rtl_openframe.h"

static TiOpenFrame * _opf_cast_as( TiOpenFrame * opf, bool empty, uint8 len, uint16 ctrl, uint8 option );


/* Example
 *	opf = opf_open( opf, OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION ); 
 *	opf = opf_open( opf, OPF_DEF_FRAMECONTROL_DATA_ACK, OPF_DEF_OPTION ); 
 *
 */
TiOpenFrame * opf_open( void * buf, uint16 memsize, uint16 ctrl, uint8 option )
{
	TiOpenFrame * opf;

	rtl_assert((buf != NULL) && (sizeof(TiOpenFrame) < memsize));

	memset( buf, 0x00, memsize );
	opf = (TiOpenFrame *)buf;
	opf->memsize = memsize;
	opf->buf = (char*)buf + sizeof(TiOpenFrame);
	opf->size = memsize - sizeof(TiOpenFrame);
	opf->datalen = 0;
	opf->type = FCF_FRAMETYPE_UNKNOWN; 
    opf->option = option;

	if (ctrl != OPF_FRAMECONTROL_UNKNOWN)
	{
		opf->type = FCF_FRAMETYPE( ctrl );
		opf = _opf_cast_as( opf, true, opf->size, ctrl, option ); 
	}
	else
		opf->type = FCF_FRAMETYPE_UNKNOWN;

	return opf;
}

void opf_close( TiOpenFrame * opf )
{
	opf->buf[0] = 0;
	opf->datalen = 0;
	opf->type = FCF_FRAMETYPE_UNKNOWN; 
}
/*
void opf_attach( TiOpenFrame * opf, char * extern_buf, uint8 size )
{
	if (sizeof(TiOpenFrame) == opf->memsize)
	{
		opf->buf = extern_buf;
		opf->size = size;
	}
}

void opf_detach( TiOpenFrame * opf )
{
	if (sizeof(TiOpenFrame) < opf->memsize)
	{
		opf->buf = (char*)opf + sizeof(TiOpenFrame);
		opf->size = opf->memsize - sizeof(TiOpenFrame);
	}
	else
		opf->buf = NULL;
}
*/


/* opf_clear()
 * clear the data inside the openframe buffer. After clearing, you should re-call
 * opf_open() or opf_cast() to prepare the opf buffer before calling other access 
 * functions. But you can directly put data into it and call opf_parse() without 
 * opf_open() or opf_cast().
 * 
 * modified by shi miaojing and zhangwei in 200910
 * - corrected bug in opf_clear. Remember you shouldn't reset opf->size to 0. 
 *   opf->size should keep unchanged during the life time of this object.  
 *
 * The following member variables should keep unchanged when clearing the openframe:
 * - memsize
 * - buf
 * - size
 */
void opf_clear( TiOpenFrame * opf )
{
	opf->datalen = 0;
	opf->option = 0x00;
	opf->type = FCF_FRAMETYPE_UNKNOWN; 

	opf->ctrl = NULL;
	opf->sequence = NULL;
	opf->panto = NULL;
	opf->shortaddrto = NULL;
	opf->longaddrto = NULL;
	opf->panfrom = NULL;
	opf->shortaddrfrom = NULL;
	opf->longaddrfrom = NULL;
	opf->superframe_spec = NULL;
	opf->gts = NULL;
	opf->gts_len = 0;
	opf->address_padding = NULL;
	opf->pad_len = 0;
	opf->cmdtype = NULL;
	opf->msdu = NULL;
	opf->msdu_len = 0;
	opf->fcs = NULL;
}

bool opf_cast( TiOpenFrame * opf, uint8 len, uint16 ctrl )
{
	return (_opf_cast_as(opf, true, len, ctrl, opf->option) != NULL);
}

/* Assume the data has already been put into the internal buffer of TiOpenFrame
 * object. 
 *
 * @param
 *  len         data length in the buf. If it's 0, then the program will automatically
 *              use buf[0]+1 as the len's value.
 * @return
 *	true        when parse successfully. And the member pointers are valid in this case.
 *  false       parse failed.
 */
bool opf_parse( TiOpenFrame * opf, uint8 len )
{
	return (_opf_cast_as(opf, false, len, 0x00, opf->option) != NULL);
}

/* Example
 *	opf_construct
 *	opf_open
 *	write something by opf->buf
 *  opf_assemble
 */
char * opf_assemble( TiOpenFrame * opf, uint8 * total_length )
{
	opf->datalen = opf->buf[0] + 1;
	*total_length = opf->datalen;
	return opf->buf;
}

/* Cast the frame object into specified type according to the frame control parameter.
 * After casting successfully, you can use the member pointers to access the frame 
 * fields directly or else these pointers are undefined.
 *
 * Beacon frame format
 * [1B Frame Length] [2B Frame Control] [1B Sequence Number] [4 or 10 Address][2 Superframe Specification]
 * 		[k GTS fields][m Padding address fields] [n Beacon payload][2 FCS]
 * 
 * Data frame format
 * [1B Frame Length] [2B Frame Control] [1B Sequence Number] [4 or 20 Address][n Data Payload][2 FCS]
 * 
 * ACK Frame
 * [1B Frame Length] [2B Frame Control] [1B Sequence Number] [2 FCS]
 * 
 * MAC Control Frame
 * [1B Frame Length] [2B Frame Control] [1B Sequence Number] [4 or 20 Address][1 Command Type]
 *      [n Command Payload][2 FCS]
 * 
 * @param
 *	[in]buf                   where the data is placed.
 *  [in]len                   length of the data inside buffer. If len==0, this function 
 *                            assume there's already a frame placed inside the internal
 *                            buffer and will extract frame length/control information 
 *                            from the internal buffer. In this case, buf[0] is the frame 
 *                            length (not include itself). If len > 0, then this function 
 *                            will cast the TiOpenFrame according to the parameter values.
 *  [in]ctrl                  frame control.
 *  [in]option                0 cast empty buffer as expected frame structure.
 *                            1 cast buffer with data as expected frame structure
 */
TiOpenFrame * _opf_cast_as( TiOpenFrame * opf, bool empty, uint8 len, uint16 ctrl, uint8 option )
{
	TiOpenFrame * ret = opf;
	char * buf = opf->buf;
	uint8 count;

	if (empty)
	{
		/* If the frame buffer is empty, then this function will cast it into expected
		 * type specified by parameter "ctrl". In this case, the "len" parameter must
		 * larger than 0. */
		memset( buf, 0x00, len );
		buf[0] = len-1;
		buf[1] = ctrl & 0xFF;
		buf[2] = (uint8)(ctrl >> 8);


	}
	else{
		/* If there's already a frame placed inside the buffer, then opf->buf[0] should
		 * be the frame's data length according to IEEE 802.15.4 specification.
		 * if the frame is too short or too long, then simply ignore it */
		if (len == 0)
		{	
			len = opf->buf[0] + 1;
		}
		if ((len < OPF_MIN_FRAME_LENGTH) || (len >= OPF_MAX_FRAME_LENGTH))
		{
			opf->buf[0] = 0;
			opf->datalen = 0;
			ret = NULL;
		}

		/* @attention
		 * The following assertion checks whether the frame is complete. It assume 
		 * the opf->len has already been assigned a proper value. The incomplete 
		 * receiving shouldn't occur but there's no guarantee about this. So the 
		 * author suggests to eliminate this assertion in the release version.
		 */
		#ifdef CONFIG_DEBUG
		//rtl_assert( opf->datalen == opf->buf[0] + 1);
		#endif
//dbo_putchar(opf->buf[1]);
//dbo_putchar(opf->buf[2]);

		ctrl = OPF_MAKEWORD(opf->buf[2], opf->buf[1]);
				 //dbo_putchar(0x33);
//	dbo_putchar((uint8)(ctrl));
	//dbo_putchar((uint8)(ctrl>>8));

	}
		
	if (ret != NULL)
	{
		opf->datalen = len;
		opf->type = FCF_FRAMETYPE(ctrl);

		opf->ctrl = buf + 1;
		opf->sequence = buf + 3;
		count = 4;
		if (empty)
			*(opf->sequence) = 0;
	}

	// short address is 2 bytes. extended address is 8 bytes (64bit)
	// for ACK frames, there're no source PAN and source address.
	//
	if (ret != NULL)
	{
			//dbo_putchar(0x11);
		//dbo_putchar(FCF_DEST_ADDRMODE(ctrl));

		switch (FCF_DEST_ADDRMODE(ctrl))
		{

		case 0x00:
			opf->panto = buf + count;
			count += 2;
			opf->shortaddrto = 0;
			opf->longaddrto = 0;
			break;
		case 0x02:
			opf->panto = buf + count;
			count += 2;
			opf->shortaddrto = buf + count;
			count += 2;
			opf->longaddrto = 0;
			break;
		case 0x03:
			opf->panto = buf + count;
			count += 2;
			opf->shortaddrto = 0;
			opf->longaddrto = buf + count;
			count += 8;
			break;
		default:
			/* modified by zhangwei on in 200911
			 * In the previous version, the "opf" is cleared by calling 
			 * opf_clear(opf). However, we should NOT affect the orginal 
			 * data inside the buffer, because other source codes may still
			 * use the data. An return value of NULL is enough to indicate this.
			 */ 
			//dbo_putchar('1');
			//opf->panto = NULL;
			//opf->shortaddrto = NULL;
			//opf->longaddrto = NULL;
			//opf_clear( opf );
			//rtl_assert(false);
			ret = NULL;
			break;
		}
	}

	if (ret != NULL)
	{
//					dbo_putchar(0x22);
//		dbo_putchar(FCF_DEST_ADDRMODE(ctrl));

		switch (FCF_SOURCE_ADDRMODE(ctrl))
		{


		case 0x00:
			opf->panfrom = 0;
			opf->shortaddrfrom = 0;
			opf->longaddrfrom = 0;
			break;
		case 0x02:
			opf->panfrom = buf + count;
			count += 2;
			opf->shortaddrfrom = buf + count;
			count += 2;
			opf->longaddrfrom = 0;
			break;
		case 0x03:
			opf->panfrom = buf + count;
			count += 2;
			opf->shortaddrfrom = 0;
			opf->longaddrfrom = buf + count;
			count += 8;
			break;
		default:
			//dbo_putchar('2');
			//opf->panfrom = NULL;
			//opf->shortaddrfrom = NULL;
			//opf->longaddrfrom = NULL;
			//opf_clear( opf );
			ret = NULL;
			//rtl_assert(false);
			break;
		}
	}

	if (ret != NULL)
	{
		switch (FCF_FRAMETYPE(ctrl))
		{
		case FCF_FRAMETYPE_BEACON:
			opf->superframe_spec = buf + count;
			count += 2;
			opf->gts = NULL;
			opf->gts_len = 0;
			opf->address_padding = NULL;
			opf->pad_len = 0;
			opf->msdu = buf + count;
			break;

		case FCF_FRAMETYPE_DATA:
			opf->msdu = buf + count;
			break;

		case FCF_FRAMETYPE_ACK:
			opf->msdu = NULL;
			break;

		case FCF_FRAMETYPE_COMMAND:
			opf->cmdtype = buf + count;
			count ++;
			opf->msdu = buf + count;
			break;

		default:
			opf->type = FCF_FRAMETYPE_UNKNOWN;
			ret = NULL;
			break;
		}
	}

	if (ret != NULL)
	{
		// If this frame is designed to have the 2B CRC checksum, then the MSDU length
		// has 2 more decrease.
		//
		// attention: Currently, this section assume the frame always has the 2B checksum.

		//if (FCF_OPTION_CRC)
		{
			opf->msdu_len = len - count - 2;
			count = len - 2;
			opf->fcs = buf + count;
		}
		//else{ 
		//	opf->msdu_len = len - count;
		//	opf->fcs = NULL;
		//}

		/*
		if (FCF_ACK_REQUEST(ctrl))
			opf->option |= ((1 << OPF_OPTION_CRC) || (1 << OPF_OPTION_ACK));

		if (opf->option & (1 << OPF_OPTION_CRC))
		{
			opf->msdu_len = len - count - 2;
			count = len - 2;
			opf->fcs = buf + count;
		}
		else{
			opf->msdu_len = len - count;
			opf->fcs = NULL;
		}

		if (opf->option & (1 << OPF_OPTION_ACK))
		{
			ctrl |= FCF_ACK_REQUEST_MASK;
			opf_set_control( opf, ctrl );
		}
		*/
	}

	/* cast failure */
	if (ret == NULL)
	{
	//	opf->buf[0] = 0;
	//	opf->datalen = 0;
	}

	return ret;
}

/* opf_copyfrom()
 * Copy the data from source to destination. If there're data, then this function 
 * will continue call opf_parse() to analyze the data in order to initialize the 
 * member variables correctly.
 *
 * @return 
 *	the count of bytes successfully copied. attention these data maybe not an valid 
 * frame and the opf_parse() inside maybe failed.
 *
 * assume: no extern buffer used in these two TiOpenFrame objects. 
 * 
 * @modified by zhangwei in 200911
 * - correct bugs. The destination's "buf" pointer and "size" variable are overrided 
 *   when doing memmove. Now corrected.
 * 
 *   The following variable should keep unchanged during copying 
 *   - memsize
 *   - buf
 *   - size
 */
uint8 opf_copyfrom( TiOpenFrame * opf, TiOpenFrame * from )
{
	/* The following should keep unchanged:
	 * opf->memsize
	 * opf->buf 
	 * opf->size
	 */
	opf->datalen = from->datalen;
	opf->option = from->option;
	opf->type = from->type;
	opf->ctrl = NULL;
	opf->sequence = NULL;
	opf->panto = NULL;
	opf->shortaddrto = NULL;
	opf->longaddrto = NULL;
	opf->panfrom = NULL;
	opf->shortaddrfrom = NULL;
	opf->longaddrfrom = NULL;
	opf->superframe_spec = NULL;
	opf->gts = NULL;
	opf->gts_len = NULL;
	opf->address_padding = NULL;
	opf->pad_len = 0;
	opf->cmdtype = 0;
	opf->msdu = NULL;
	opf->msdu_len = 0;
	opf->fcs = NULL;

	rtl_assert( opf->size >= from->datalen );
	if (opf->datalen > 0)
	{
		memmove( opf->buf, from->buf, opf->datalen );
		opf_parse( opf, opf->datalen );
	}
	return opf->datalen;
}

/* opf_copyto()
 * Copy the data from source to destination. If there're data, then this function 
 * will continue call opf_parse() to analyze the data in order to initialize the 
 * member variables correctly.
 *
 * @return 
 *	the count of bytes successfully copied. attention these data maybe not an valid 
 * frame and the opf_parse() inside maybe failed.
 *
 * assume: no extern buffer used in these two TiOpenFrame objects. 
 * 
 * @modified by zhangwei in 200911
 * - correct bugs. The destination's "buf" pointer and "size" variable are overrided 
 *   when doing memmove. Now corrected.
 * 
 *   The following variable should keep unchanged during copying 
 *   - memsize
 *   - buf
 *   - size
 */
uint8 opf_copyto( TiOpenFrame * opf, TiOpenFrame * to )
{
	/* The following should keep unchanged:
	 * to->memsize
	 * to->buf 
	 * to->size
	 */
	to->datalen = opf->datalen;
	to->option = opf->option;
	to->type = opf->type;
	to->ctrl = NULL;
	to->sequence = NULL;
	to->panto = NULL;
	to->shortaddrto = NULL;
	to->longaddrto = NULL;
	to->panfrom = NULL;
	to->shortaddrfrom = NULL;
	to->longaddrfrom = NULL;
	to->superframe_spec = NULL;
	to->gts = NULL;
	to->gts_len = NULL;
	to->address_padding = NULL;
	to->pad_len = 0;
	to->cmdtype = 0;
	to->msdu = NULL;
	to->msdu_len = 0;
	to->fcs = NULL;

	rtl_assert( to->size >= opf->datalen );
	if (opf->datalen > 0)
	{
		memmove( to->buf, opf->buf, opf->datalen );
		opf_parse( to, to->datalen );
	}
	return opf->datalen;
}


/* Read the data out from TiOpenFrame's internal/attached memory. Attention the 
 * first byte in the frame (length byte) and the last two CRC checksum bytes are 
 * removed from the data returned.
 *
 * @return
 *  data length inside buf. 
 */
/*
uint8 opf_read( TiOpenFrame * opf, char * buf, uint8 size )
{
    int8 count;

	count = opf->buf[0];

	if (count > 0)
	{
		rtl_assert( count > OPF_MIN_FRAME_LENGTH );
		
		if (opf->option & (1 << OPF_OPTION_CRC))
			count -= 2;
		
		count --;
		rtl_assert( count <= size );
	    // copy data to the output buffer. ignore the first byte(length byte) and the 
		// last checksum (2 bytes)
	    memmove( buf, opf->buf + 1, count );
	}

	return count;
}
*/
/* attention: the TiOpenFrame's buffer must be large enough to hold the input data
 * and the possible checksum( 2 bytes, not in the parameter buf).
 */
/*
uint8 opf_write( TiOpenFrame * opf, char * buf, uint8 len )
{
    int8 size, count;

    size = opf->size;
	if (opf->option & (1 << OPF_OPTION_CRC)) 
        size -= 2;

    // attention there's an additional byte in the front of opf->buf. It represents
    // the frame's data length.
	count = ((len < opf->size) ? len : 0);
	if (count > 0)
	{
		memmove( opf->buf+1, buf, count );
        opf->buf[0] = count;
        opf->len = count+1;
		opf_parse( opf );
	}

	return count;
}
*/

/* @attention
 * @warning
 *	this function won't synchronize the opf's internal pointers. so generally, you 
 * should re-parse the frame after calling this function if you want to access the 
 * frame members.
 *
 * @return
 *	How many bytes successfully put into the frame. It should equal to "len" parameter. 
 * If the return value is less than "len", then it means the frame buffer is full.
 */
/*
uint8 opf_append( TiOpenFrame * opf, char * data, uint8 len )
{
	int8 count;

	hal_assert( opf->buf != NULL );

	count = min( opf->size - opf->len, len );
	if (opf->option & (1 << OPF_OPTION_CRC))
		count -= 2;

	if (count > 0)
	{
		memmove( opf->data + opf->buf[0], data, count );
		opf->len += count;
		opf->buf[0] += count;
	}

	return count;
}
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



/* Beacon frame format
 * [2B Frame Control] [1B Sequence Number][4 or 10 Address][2 Superframe Specification]
 * 		[k GTS fields][m Padding address fields] [n Beacon payload][2 FCS]
 */
/*
typedef struct{
  uint16 * ctrl;
  uint8  * sequence;
  uint16 * panto;
  uint16 * shortaddrto;
  uint8  * longaddrto;
  uint16 * panfrom;
  uint16 * shortaddrfrom;
  uint8  * longaddrfrom;
  uint32 * address;
  uint16 * superframe_spec;
  uint8  * gts;
  uint8    gts_len;
  uint8  * address_padding;
  uint8    pad_len;
  uint8  * data;
  uint8    datalen;
  uint16 * fcs;
}TiBeaconFrameIndex;

typedef struct{
  uint16 * ctrl;
  uint8  * sequence;
  uint16 * panto;
  uint16 * shortaddrto;
  uint8  * longaddrto;
  uint16 * panfrom;
  uint16 * shortaddrfrom;
  uint8  * longaddrfrom;
  uint8  * data;
  uint8    datalen;
  uint16 * fcs;
}TiDataFrameIndex;

typedef struct{
  uint16 * ctrl;
  uint8  * sequence;
  uint16 * fcs;
}TiAckFrameIndex;

typedef struct{
  uint16 * ctrl;
  uint8  * sequence;
  uint16 * panto;
  uint16 * shortaddrto;
  uint8  * longaddrto;
  uint16 * panfrom;
  uint16 * shortaddrfrom;
  uint8  * longaddrfrom;
  uint8  * cmdtype;
  uint8  * data;
  uint8    datalen;
  uint16 * fcs;
}TiCommandFrameIndex;
*/
