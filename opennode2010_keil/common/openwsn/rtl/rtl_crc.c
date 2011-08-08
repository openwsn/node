/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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
 * @author zhangwei on 2005-07-19
 * @note: Interface file CRC check module.
 *	thanks Ruijie to give the original source code (first version). 
 * 
 * @history
 * 2005-07-19 first created by zhangwei
 * @modified by zhangwei on 20061030
 * revised and released as part of RTL
 ******************************************************************************/

#ifdef CONFIG_CRC_FAST_VERSION

#define CRC_FCS(fcs, c) (((fcs) >> 8) ^ fcstab[((fcs) ^ (c)) & 0xff])

static const unsigned short fcstab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/**
 * calculate the CRC checksum
 * @param buf, 帧缓冲区的首指针，指向第一个待校验的字符
 * @param number, 待校验帧的字符个数
 * @return fcsOut, CRC校验码
 *   另外，本函数在输入buf缓冲区的末尾添加了CRC校验码  
 */
unsigned short crc_produce(unsigned char *buf, unsigned short number)
{
    unsigned short  n;
    unsigned short fcsOut;

    fcsOut=0xFFFF;
    for(n=0;n<number;n++)
    {
        fcsOut = (unsigned short)CRC_FCS(fcsOut, buf[n]);
    }
    buf[n]= (unsigned char)(fcsOut & 0xFF);
    buf[n+1]=(unsigned char)((fcsOut >> 8) & 0xFF);
    return fcsOut;
}

/**
 * do CRC verification
 * @param buf,  帧缓冲区的首指针，指向第一个已校验的字符
 * @param number, 已校验帧的字符个数，可以包括CRC校验码
 * @return fcsOut, CRC校验码
 *                    另外，当检验帧包含CRC校验码时，CRC校验正确时输出fcsOut为0  
 */
unsigned short crc_check(unsigned char *buf, unsigned short number)
{
    unsigned short  n;
    unsigned short fcsOut;

    fcsOut=0xFFFF;
    for(n=0;n<number;n++)
    {
        fcsOut = (unsigned short)CRC_FCS(fcsOut, buf[n]);
    }
    return fcsOut;
}

#endif /* CONFIG_CRC_FAST_VERSION */


#ifndef CONFIG_CRC_FAST_VERSION

/*
 * Reference
 * http://www.geocities.com/malbrain/crc_c.html
 */

/*
 *        calculate ccitt cyclic redundancy codes
 *
 *        actual Crc16 = x ^ 16 + x ^ 12 + x ^ 5 + 1
 *
 *        inverse Crc32 = x ^ 32 + x ^ 31 + x ^ 30 +
 *              x ^ 28 + x ^ 27 + x ^ 25 + x ^ 24 +
 *              x ^ 22 + x ^ 21 + x ^ 20 + x ^ 16 +
 *              x ^ 10 + x ^ 9 + x ^ 6 + 1
 *
 *        n.b. standard ANSI X3.66 crc-32 polynomial:
 *        x^0 + x^1 + x^2 + x^4 + x^5 + x^7 + x^8 + x^10 +
 *        x^11 + x^12 + x^16 + x^22 + x^23 + x^26 + x^32
 *
 *        Tables constructed so that entry 0x80 = inverse polynomial
 */

unsigned short Crc16[] = {
0x0000, 0x1081, 0x2102, 0x3183, 0x4204, 0x5285, 0x6306, 0x7387,
0x8408, 0x9489, 0xa50a, 0xb58b, 0xc60c, 0xd68d, 0xe70e, 0xf78f
};

unsigned long Crc32[] = {
0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

unsigned short crc16_calc (unsigned char *ptr, unsigned cnt, unsigned short crc)
{
    while( cnt-- ) {
        crc = ( crc >> 4 ) ^ Crc16[(crc & 0xf) ^ (*ptr & 0xf)];
        crc = ( crc >> 4 ) ^ Crc16[(crc & 0xf) ^ (*ptr++ >> 4)];
    }

    return crc;
}

unsigned long crc32_calc (unsigned char *ptr, unsigned cnt, unsigned long crc)
{
    while( cnt-- ) {
        crc = ( crc >> 4 ) ^ Crc32[(crc & 0xf) ^ (*ptr & 0xf)];
        crc = ( crc >> 4 ) ^ Crc32[(crc & 0xf) ^ (*ptr++ >> 4)];
    }

    return crc;
}


#endif /* Not defined CONFIG_CRC_FAST_VERSION  */

