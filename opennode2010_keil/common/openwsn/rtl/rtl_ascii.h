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
/*******************************************************************
 * @copyright(c)	2005-2020, XXX Tech. Co. Ltd
 * @author			zhangwei on 2005-07-30
 * @version			2005-07-30
 * @status			released
 *
 * @description
 *  ASCII Table 
 *  In paritcular the first 31 invisible control characters.
 *
 * @attention
 *
 * @interface
 *   NONE
 * @implementation
 *  NONE
 *
 * @history
 * @modified by zhangwei on 2005-07-30
 *   first created.
 * @modified by zhangwei on 2005-07-30
 *   released
 * @modified by zhangwei on 20081201
 * 	- rename as rtl_ascii.h
 ******************************************************************/

#ifndef _ASCII_H_
#define _ASCII_H_

/*
 * Dec Hx Oct Char                        Dec Hx Oct Char  Dec Hx Oct Char  Dec Hx Oct Char
 * ---------------                        ---------------  ---------------  ---------------
 *  0  0 000 NUL (null)                   32 20 040 SPACE  64 40 100 @      96 60 140 `
 *  1  1 001 SOH (start of heading)       33 21 041 !      65 41 101 A      97 61 141 a
 *  2  2 002 STX (start of text)          34 22 042 "      66 42 102 B      98 62 142 b
 *  3  3 003 ETX (end of text)            35 23 043 #      67 43 103 C      99 63 143 c
 *  4  4 004 EOT (end of transmission)    36 24 044 $      68 44 104 D     100 64 144 d
 *  5  5 005 ENQ (enquiry)                37 25 045 %      69 45 105 E     101 65 145 e
 *  6  6 006 ACK (acknowledge)            38 26 046 &      70 46 106 F     102 66 146 f
 *  7  7 007 BEL (bell)                   39 27 047 '      71 47 107 G     103 67 147 g
 *  8  8 010 BS  (backspace)              40 28 050 (      72 48 110 H     104 68 150 h
 *  9  9 011 TAB (horizontal tab)         41 29 051 )      73 49 111 I     105 69 151 i
 * 10  A 012 LF  (NL line feed, new line) 42 2A 052 *      74 4A 112 J     106 6A 152 j
 * 11  B 013 VT  (vertical tab)           43 2B 053 +      75 4B 113 K     107 6B 153 k
 * 12  C 014 FF  (NP form feed, new page) 44 2C 054 ,      76 4C 114 L     108 6C 154 l
 * 13  D 015 CR  (carriage return)        45 2D 055 -      77 4D 115 M     109 6D 155 m
 * 14  E 016 SO  (shift out)              46 2E 056 .      78 4E 116 N     110 6E 156 n
 * 15  F 017 SI  (shift in)               47 2F 057 /      79 4F 117 O     111 6F 157 o
 * 16 10 020 DLE (data link escape)       48 30 060 0      80 50 120 P     112 70 160 p
 * 17 11 021 DC1 (device control 1)       49 31 061 1      81 51 121 Q     113 71 161 q
 * 18 12 022 DC2 (device control 2)       50 32 062 2      82 52 122 R     114 72 162 r
 * 19 13 023 DC3 (device control 3)       51 33 063 3      83 53 123 S     115 73 163 s
 * 20 14 024 DC4 (device control 4)       52 34 064 4      84 54 124 T     116 74 164 t
 * 21 15 025 NAK (negative acknowledge)   53 35 065 5      85 55 125 U     117 75 165 u
 * 22 16 026 SYN (synchronous idle)       54 36 066 6      86 56 126 V     118 76 166 v
 * 23 17 027 ETB (end of trans. block)    55 37 067 7      87 57 127 W     119 77 167 w
 * 24 18 030 CAN (cancel)                 56 38 070 8      88 58 130 X     120 78 170 x
 * 25 19 031 EM  (end of medium)          57 39 071 9      89 59 131 Y     121 79 171 y
 * 26 1A 032 SUB (substitute)             58 3A 072 :      90 5A 132 Z     122 7A 172 z
 * 27 1B 033 ESC (escape)                 59 3B 073 ;      91 5B 133 [     123 7B 173 {
 * 28 1C 034 FS  (file separator)         60 3C 074 <      92 5C 134 \     124 7C 174 |
 * 29 1D 035 GS  (group separator)        61 3D 075 =      93 5D 135 ]     125 7D 175 }
 * 30 1E 036 RS  (record separator)       62 3E 076 >      94 5E 136 ^     126 7E 176 ~
 * 31 1F 037 US  (unit separator)         63 3F 077 ?      95 5F 137 _     127 7F 177 DEL
 */

#define NUL  0
#define SOH  1
#define STX  2
#define ETX  3
#define EOT  4
#define ENQ  5
#define ACK  6
#define BEL  7
#define BS   8
#define TAB  9
#define LF  10
#define VT  11
#define FF  12
#define CR  13
#define SO  14
#define SI  15
#define DLE 16
#define DC1 17
#define DC2 18
#define DC3 19
#define DC4 20
#define NAK 21
#define SYN 11
#define ETB 23
#define CAN 24
#define EM  25
#define SUB 26
#define ESC 27
#define FS  28
#define GS  29
#define RS  30
#define US  31

#endif /* _ASCII_H_ */

