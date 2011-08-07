#include "rtl_configall.h"
#include <string.h>
#include "rtl_foundation.h"
#include "rtl_ieee802frame154.h"
#include "rtl_debugio.h"

// static void _write_n16( char * buf, uint16 value );
// static uint16 _read_n16( char * buf );
static inline void _ieee802frame154_swapbyte( char * b1, char * b2 );
static TiIEEE802Frame154Descriptor * _ieee802frame154_cast_as( TiIEEE802Frame154Descriptor * desc, 
    bool empty, char * buf, uint8 len, uint16 ctrl, uint8 option );

TiIEEE802Frame154Descriptor * ieee802frame154_open( TiIEEE802Frame154Descriptor * desc )
{
    memset( desc, 0x00, sizeof(desc) );
    return desc;
}

void ieee802frame154_close( TiIEEE802Frame154Descriptor * desc )
{
    return;
}

/* ieee802frame154_clear()
 * clear the data inside the openframe buffer. 
 * 
 * modified by shi miaojing and zhangwei in 200910
 * - corrected bug in desc_clear. Remember you shouldn't reset desc->capacity to 0. 
 *   desc->capacity should keep unchanged during the life time of this object.  
 *
 * The following member variables should keep unchanged when clearing the openframe:
 * - memsize
 * - buf
 * - capacity
 */
void ieee802frame154_clear( TiIEEE802Frame154Descriptor * desc )
{
    uint8 capacity = desc->capacity;

    memset( desc, 0x00, sizeof(desc) );
    desc->buf = NULL;
    desc->capacity = capacity;

    /*
    desc->buf = NULL;
    desc->capacity = 0;
    desc->count = 0;

	//desc->datalen = 0;
	//desc->option = 0x00;
	//desc->type = FCF_FRAMETYPE_UNKNOWN; 

    desc->plen = NULL;
	desc->ctrl = NULL;
	desc->sequence = NULL;
	desc->panto = NULL;
	desc->shortaddrto = NULL;
	desc->longaddrto = NULL;
	desc->panfrom = NULL;
	desc->shortaddrfrom = NULL;
	desc->longaddrfrom = NULL;
	desc->superframe_spec = NULL;
	desc->gts = NULL;
	desc->gts_len = 0;
	desc->address_padding = NULL;
	desc->pad_len = 0;
	desc->cmdtype = NULL;
	desc->msdu = NULL;
	desc->msdu_len = 0;
	desc->fcs = NULL;
    */
}

/* Example
 *	desc = desc_format( desc, FRAME154_DEF_FRAMECONTROL_DATA_ACK, FRAME154_DEF_OPTION ); 
 *
 */
TiIEEE802Frame154Descriptor * ieee802frame154_format( TiIEEE802Frame154Descriptor * desc, 
    char * buf, uint8 capacity, uint16 ctrl )
{
	return _ieee802frame154_cast_as(desc, true, buf, capacity, ctrl, FRAME154_DEF_OPTION );
}

/* Example
 *	desc_construct
 *	desc_open
 *	write something by desc->buf
 *  desc_assemble
 */
char * ieee802frame154_assemble( TiIEEE802Frame154Descriptor * desc, uint8 * total_length )
{
    /* todo
    if (desc->option & ACK BIT)
    {
    }

    if (desc->option & CRC bit)
    {
    }

    */
    rtl_assert( desc->buf != NULL );
	desc->count = desc->buf[0] + 1;
	*total_length = desc->count;
	return desc->buf;
}

/* Assume the data has already been put into the buffer.
 *
 * @param
 *  len         data length in the buf. If it's 0, then the program will automatically
 *              use buf[0]+1 as the len's value.
 * @return
 *	true        when parse successfully. And the member pointers are valid in this case.
 *  false       parse failed.
 */
bool ieee802frame154_parse( TiIEEE802Frame154Descriptor * desc, char * buf, uint8 len )
{
    // the last two parameters, control and option, are no use when parsing a frame,
    // because such information will be extracted from the buffer.

	return (_ieee802frame154_cast_as(desc, false, buf, len, 0x00, 0x00) != NULL);
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
 *                            will cast the TiIEEE802Frame154Descriptor according to the parameter values.
 *  [in]ctrl                  frame control.
 *  [in]option                0 
 *                            1 
 * 
 * @param len: if empty is true, then this parameter indicates the size of the buffer.
 *      if empty is false, then this parameter indicates the length of the data in the buffer.
 */
TiIEEE802Frame154Descriptor * _ieee802frame154_cast_as( TiIEEE802Frame154Descriptor * desc, 
    bool empty, char * buf, uint8 len, uint16 ctrl, uint8 option )
{
	TiIEEE802Frame154Descriptor * ret = desc;
	uint8 count;

    rtl_assert( buf != NULL );

	if (empty)
	{
        // @modified by zhangwei on 2010.08.15
        // - bug fix. the buf cannot be cleared because the application may put some
        // data into this buffer. you should keep them in the buffer. 
        
		// If the frame buffer is empty, then this function will cast it into expected
		// type specified by parameter "ctrl". In this case, the "len" parameter must
		// larger than 0.

        // #ifdef CONFIG_DEBUG
		// memset( buf, 0x00, len );
        // #endif

        // an valid frame's length should not be smaller than 9 because the smallest 
        // header has more than 9 byte
       
        rtl_assert( len > FRAME154_MIN_FRAME_LENGTH );
		
        rtl_assert( len > 9 );

		buf[0] = len-1;
		buf[1] = (char)(ctrl & 0xFF);
		buf[2] = (char)(ctrl >> 8);
        desc->buf = buf;
        desc->capacity = len;
        desc->count = 0;
	}
	else{
		// If there's already a frame placed inside the buffer, then buf[0] should
		// be the frame's data length according to IEEE 802.15.4 specification.
		// if the frame is too short or too long, then simply ignore it.

        // if len is 0, then we use buf[0] + 1 as the new data length. attention
        // in this case, you must guarantee the frame is complete, namely, buf[0]+1
        // must be smaller than the buffer capacity.
        // 
        // nonzero len value indicates the data length in the buffer. it should 
        // equal to buf[0]+1, however, this maybe not the truth if the frame isn't
        // complete. we suggest you always input nonzero len value and let the program
        // to check whether the frame is valid or not.
        //
		if (len == 0)
		{	
			len = buf[0] + 1;
		}
        else if (len != buf[0]+1)
        {
            len = 0;
        }

        // if the frame is too short or too long, then we ignore this frame
        //
        if ((len < FRAME154_MIN_FRAME_LENGTH) || (len >= FRAME154_MAX_FRAME_LENGTH))
		{
			buf[0] = 0;
            desc->buf = buf;
            desc->capacity = len;
            desc->count = 0;
			ret = NULL;
		}
        else{
            desc->buf = buf;
            desc->capacity = len;
            desc->count = len;
    		ctrl = FRAME154_MAKEWORD( buf[2], buf[1]);
            option = 0x00;
        }
	}

	if (ret != NULL)
	{
		desc->ctrl = buf + 1;
		desc->sequence = buf + 3;
		count = 4;
		if (empty)
        {
			*(desc->sequence) = 0;
        }
	}

	// short address is 2 bytes. extended address is 8 bytes (64bit)
	// for ACK frames, there're no source PAN and source address.
	//
	if (ret != NULL)
	{
        // dbo_putchar( 0x11 );
		// dbo_putchar( FCF_DEST_ADDRMODE(ctrl) );

		switch (FCF_DEST_ADDRMODE(ctrl))
		{
		case 0x00:
			desc->panto = buf + count;
			count += 2;
			desc->shortaddrto = 0;
			desc->longaddrto = 0;
			break;
		case 0x02:
			desc->panto = buf + count;
			count += 2;
			desc->shortaddrto = buf + count;
			count += 2;
			desc->longaddrto = 0;
			break;
		case 0x03:
			desc->panto = buf + count;
			count += 2;
			desc->shortaddrto = 0;
			desc->longaddrto = buf + count;
			count += 8;
			break;
		default:
            // modified by zhangwei on in 200911
            // the 802.15.4 protocol doesn't support other values. so the program 
            // should never enter this default branch. however, if the current node
            // received an corrupt frame in the confliction, the value of each byte
            // is undetermined. so in real case, the program may enter this default
            // case. 
            //
            // in the past, we had use assertion here. based on the above analysis,
            // we shouldn't use assertion here.
            
            // the following is for debugging only. you should delete them in released version.
			// dbc_putchar(0xb3);
			// dbc_putchar(0xb3);
			// dbc_putchar(0xb3);
			// dbc_putchar(0xb3);
			ret = NULL;
			break;
		}
	}

	if (ret != NULL)
	{
        // dbo_putchar(0x22);
        // dbo_putchar(FCF_DEST_ADDRMODE(ctrl));

		switch (FCF_SOURCE_ADDRMODE(ctrl))
		{
		case 0x00:
			desc->panfrom = 0;
			desc->shortaddrfrom = 0;
			desc->longaddrfrom = 0;
			break;
		case 0x02:
			desc->panfrom = buf + count;
			count += 2;
			desc->shortaddrfrom = buf + count;
			count += 2;
			desc->longaddrfrom = 0;
			break;
		case 0x03:
			desc->panfrom = buf + count;
			count += 2;
			desc->shortaddrfrom = 0;
			desc->longaddrfrom = buf + count;
			count += 8;
			break;
		default:
            // modified by zhangwei on in 200911
            // the 802.15.4 protocol doesn't support other values. so the program 
            // should never enter this default branch. however, if the current node
            // received an corrupt frame in the confliction, the value of each byte
            // is undetermined. so in real case, the program may enter this default
            // case. 
            //
            // in the past, we had use assertion here. based on the above analysis,
            // we shouldn't use assertion here.
            //
			ret = NULL;
			break;
		}
	}

	if (ret != NULL)
	{
		switch (FCF_FRAMETYPE(ctrl))
		{
		case FCF_FRAMETYPE_BEACON:
			desc->superframe_spec = buf + count;
			count += 2;
			desc->gts = NULL;
			desc->gts_len = 0;
			desc->address_padding = NULL;
			desc->pad_len = 0;
			desc->msdu = buf + count;
			break;

		case FCF_FRAMETYPE_DATA:
			desc->msdu = buf + count;
			break;

		case FCF_FRAMETYPE_ACK:
			desc->msdu = NULL;
			break;

		case FCF_FRAMETYPE_COMMAND:
			desc->cmdtype = buf + count;
			count ++;
			desc->msdu = buf + count;
			break;

		default:
            // modified by zhangwei on in 201006
            // the 802.15.4 protocol doesn't support other values. so the program 
            // should never enter this default branch. however, if the current node
            // received an corrupt frame in the confliction, the value of each byte
            // is undetermined. so in real case, the program may enter this default
            // case. 
            //
            // in the past, we had use assertion here. based on the above analysis,
            // we shouldn't use assertion here.
            //
			ret = NULL;
			break;
		}
	}

	if (ret != NULL)
	{
		// If this frame is designed to have the 2B CRC checksum, then the MSDU length
		// has 2 more decrease.
		//
		// attention: currently, this section assume the frame always has the 2B checksum.

		//if (FCF_OPTION_CRC)
		{
			desc->msdu_len = len - count - 2;
			count = len - 2;
			desc->fcs = buf + count;
		}
		//else{ 
		//	desc->msdu_len = len - count;
		//	desc->fcs = NULL;
		//}

		/*
		if (FCF_ACK_REQUEST(ctrl))
			desc->option |= ((1 << FRAME154_OPTION_CRC) || (1 << FRAME154_OPTION_ACK));

		if (desc->option & (1 << FRAME154_OPTION_CRC))
		{
			desc->msdu_len = len - count - 2;
			count = len - 2;
			desc->fcs = buf + count;
		}
		else{
			desc->msdu_len = len - count;
			desc->fcs = NULL;
		}

		if (desc->option & (1 << FRAME154_OPTION_ACK))
		{
			ctrl |= FCF_ACK_REQUEST_MASK;
			desc_set_control( desc, ctrl );
		}
		*/
	}

	// cast failure 
    // return NULL is really failed format or parsing.

	if (ret == NULL)
	{
	    // desc->buf[0] = 0;
        // desc->len = 0;
	}

	return ret;
}
/*
inline void _write_n16( char * buf, uint16 value )
{
	buf[0] = (char)(value & 0xFF);
	buf[1] = (char)((value >> 8) & 0xFF);
}

inline uint16 _read_n16( char * buf )
{
    return (((uint16)buf[1]) << 8) | ((uint16)buf[0]);
}
*/
inline void _ieee802frame154_swapbyte( char * b1, char * b2 )
{
	char tmp = *b1;
	*b1 = *b2;
	*b2 = tmp;
}

void ieee802frame154_swapaddress( TiIEEE802Frame154Descriptor * desc )
{
	if ((desc->panto != NULL) && (desc->panfrom != NULL))
	{
		_ieee802frame154_swapbyte( desc->panto, desc->panfrom );
		_ieee802frame154_swapbyte( desc->panto+1, desc->panfrom+1 );
	}

	if ((desc->shortaddrto != NULL) && (desc->shortaddrfrom != NULL))
	{
		_ieee802frame154_swapbyte( desc->shortaddrto, desc->shortaddrfrom );
		_ieee802frame154_swapbyte( desc->shortaddrto+1, desc->shortaddrfrom+1 );

	}

	if ((desc->longaddrto != NULL) && (desc->longaddrfrom != NULL))
	{
		_ieee802frame154_swapbyte( desc->longaddrto+0, desc->longaddrfrom+0 );
		_ieee802frame154_swapbyte( desc->longaddrto+1, desc->longaddrfrom+1 );
		_ieee802frame154_swapbyte( desc->longaddrto+2, desc->longaddrfrom+2 );
		_ieee802frame154_swapbyte( desc->longaddrto+3, desc->longaddrfrom+3 );
		_ieee802frame154_swapbyte( desc->longaddrto+4, desc->longaddrfrom+4 );
		_ieee802frame154_swapbyte( desc->longaddrto+5, desc->longaddrfrom+5 );
		_ieee802frame154_swapbyte( desc->longaddrto+6, desc->longaddrfrom+6 );
		_ieee802frame154_swapbyte( desc->longaddrto+7, desc->longaddrfrom+7 );

	}
}

/* desc_copyfrom()
 * Copy the data from source to destination. If there're data, then this function 
 * will continue call desc_parse() to analyze the data in order to initialize the 
 * member variables correctly.
 *
 * @return 
 *	the count of bytes successfully copied. attention these data maybe not an valid 
 * frame and the desc_parse() inside maybe failed.
 *
 * assume: no extern buffer used in these two TiIEEE802Frame154Descriptor objects. 
 * 
 * @modified by zhangwei in 200911
 * - correct bugs. The destination's "buf" pointer and "capacity" variable are overrided 
 *   when doing memmove. Now corrected.
 * 
 *   The following variable should keep unchanged during copying 
 *   - memsize
 *   - buf
 *   - capacity
 */
uint8 desc_copyfrom( TiIEEE802Frame154Descriptor * desc, TiIEEE802Frame154Descriptor * from )
{
	/* The following should keep unchanged:
	 * desc->memsize
	 * desc->buf 
	 * desc->capacity
	 */

    /*
	desc->datalen = from->datalen;
	desc->option = from->option;
	desc->type = from->type;
	desc->ctrl = NULL;
	desc->sequence = NULL;
	desc->panto = NULL;
	desc->shortaddrto = NULL;
	desc->longaddrto = NULL;
	desc->panfrom = NULL;
	desc->shortaddrfrom = NULL;
	desc->longaddrfrom = NULL;
	desc->superframe_spec = NULL;
	desc->gts = NULL;
	desc->gts_len = NULL;
	desc->address_padding = NULL;
	desc->pad_len = 0;
	desc->cmdtype = 0;
	desc->msdu = NULL;
	desc->msdu_len = 0;
	desc->fcs = NULL;

	rtl_assert( desc->capacity >= from->datalen );
	if (desc->datalen > 0)
	{
		memmove( desc->buf, from->buf, desc->datalen );
		desc_parse( desc, desc->datalen );
	}
	return desc->datalen;
    */
    return 0;
}

/* desc_copyto()
 * Copy the data from source to destination. If there're data, then this function 
 * will continue call desc_parse() to analyze the data in order to initialize the 
 * member variables correctly.
 *
 * @return 
 *	the count of bytes successfully copied. attention these data maybe not an valid 
 * frame and the desc_parse() inside maybe failed.
 *
 * assume: no extern buffer used in these two TiIEEE802Frame154Descriptor objects. 
 * 
 * @modified by zhangwei in 200911
 * - correct bugs. The destination's "buf" pointer and "capacity" variable are overrided 
 *   when doing memmove. Now corrected.
 * 
 *   The following variable should keep unchanged during copying 
 *   - memsize
 *   - buf
 *   - capacity
 */
uint8 desc_copyto( TiIEEE802Frame154Descriptor * desc, TiIEEE802Frame154Descriptor * to )
{
	/* The following should keep unchanged:
	 * to->memsize
	 * to->buf 
	 * to->capacity
	 */
    /*
	to->datalen = desc->datalen;
	to->option = desc->option;
	to->type = desc->type;
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

	rtl_assert( to->capacity >= desc->datalen );
	if (desc->datalen > 0)
	{
		memmove( to->buf, desc->buf, desc->datalen );
		desc_parse( to, to->datalen );
	}
	return desc->datalen;
    */
    return 0;
}





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


