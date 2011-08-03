#ifndef _RTL_XTPFRAME_H_AA78_
#define _RTL_XTPFRAME_H_AA78_
/***********************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2004-2010 zhangwei(TongJi University)
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

#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_iobuf.h"


/* CONFIG_XTP_FRAME_MAX_LENGTH 
 * configure the internal buffer for TiXtpFrame. this value constraints the frame that 
 * can be handled. if the input/output frame exceed this value, then it will be cleared,
 * and xtp_frame_BUFFER_OVERRUN error message is set.
 */
#define CONFIG_XTP_FRAME_MAX_LENGTH 256
#define CONFIG_XTP_FRAME_SOF 0x55


/* TiXtpFrame represents an frame which has the following format:
 * 
 *   [Start of Frame 1B] [Length 2B] [Data nB] {Checksum 2B}
 * 
 * [SOF]
 *  - one byte only. 
 * 
 * [LENGTH]
 *  - the length of the data field, including the checksum if it exists.
 *
 * [b15, ..., b0]
 * - b15£ºchecksum enable. If it's 1, then checksum enabled.
 * - b14, b13£ºdata encoding set. 
 *      00 means raw data(no encoding). in this case, the caller must guarantee there's 
 *         no SOF byte in the data. 
 *      01 means compact encoding. in this case, each byte is represented by 2 byte ascii codes. 
 *      10 means the binary coding with escape characters. this is the most efficient one. 
 *      11 means text based Base64 encoding. 
 * b12: reserved. must set to 0 now. 
 * b11-b0£ºdata length. so the maximum frame length is 3+2^12, which is 3+4096 byte. 
 *      attention this value is the data length after the data is encoded.
 * 
 * during transmission, the highest byte will be transmitted firstly, which is known as 
 * "network order".
 * 
 * [DATA]
 * - various length
 *
 * {CHECKSUM}
 * - if the checksum bit in the length field is set, then the last two bytes in the 
 * frame is considered the checksum. the checksum is calculated using CRC algorithm.
 */

/* attention
 * since there's a powerful TiIoBuf object, we can make use it to represent TiXtpFrame
 * directly. 
 */

#define TiXtpFrame TiIoBuf

#define xtp_frame_empty(frame) iobuf_empty(frame)

/* using macros to implement the TiXtpFrame object based on what TiIoBuf provided. */

#ifdef __cplusplus
extern "C" {
#endif

#define xtp_frame_construct(mem, memsize) iobuf_construct(mem, memsize)
#define xtp_frame_destroy(xtpframe) iobuf_destroy(xtpframe)
#define xtp_frame_create(size) iobuf_create(size)
#define xtp_frame_free(xtpframe) iobuf_free(xtpframe)

bool xtp_frame_checksum_enabled(TiXtpFrame * xtpframe);
uint8 xtp_frame_encodetype(TiXtpFrame * xtpframe);
uint16 xtp_frame_length(TiXtpFrame * xtpframe);
char * xtp_frame_dataptr(TiXtpFrame * xtpframe);
uint16 xtp_frame_data( TiXtpFrame * xtpframe, char * buf, uint16 size );
uint16 xtp_frame_checksum(TiXtpFrame * xtpframe);

void xtp_frame_set_checksum_enabled(TiXtpFrame * xtpframe);
void xtp_frame_set_encodetype(TiXtpFrame * xtpframe, uint8 type);
void xtp_frame_set_length(TiXtpFrame * xtpframe, uint16 len);
void xtp_frame_set_data(TiXtpFrame * xtpframe, char * mem, uint16 memsize);
void xtp_frame_set_checksum(TiXtpFrame * xtpframe, uint16 checksum);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _RTL_XTPFRAME_H_AA78_ */
