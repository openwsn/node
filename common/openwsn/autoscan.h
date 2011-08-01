#ifndef _AUTOSCAN_H_7484_
#define _AUTOSCAN_H_7484_
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
 
/* Compiler Type Detection
 *
 * @reference
 * - Pre-defined C/C++ Compiler Macros, 
 *   http://sourceforge.net/apps/mediawiki/predef/index.php?title=Main_Page
 * - Compiler detection macros,
 *   http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers 
 *
 * @author zhangwei on 20110420 
 * - first created.
 */
 
#define CONFIG_COMPILER_UNKNOWN
 
/* Altium MicroBlaze C */
#ifdef __CMB__
#define CONFIG_COMPILER_MICROBLAZEC
#undef CONFIG_COMPILER_UNKNOWN
#endif
 
/* ARM Compiler
 * The ARM compiler is also identified by macro  __arm__. 
 * Please note that the __ARMCC_VERSION macro is also used as version indicator 
 * for Norcroft C, but that the format is different.
 */
#ifdef __CC_ARM 
#ifdef __ARMCC_VERSION
#define CONFIG_COMPILER_ARMCC
#undef CONFIG_COMPILER_UNKNOWN
#endif
#endif
 
/* Borland C++ */
#ifdef __BORLANDC__
#define CONFIG_COMPILER_BORLANDC
#if (__BORLANDC__ < 0x500)
#error "Requires Borland C/C++ 5.0 or greater to compile the source files"
#endif
#undef CONFIG_COMPILER_UNKNOWN
#endif
#ifdef __CODEGEARC__
#define CONFIG_COMPILER_BORLANDC
#undef CONFIG_COMPILER_UNKNOWN
#endif

/* __GNUC__ */
#ifdef __GNUC__
#define CONFIG_COMPILER_GNUC
#undef CONFIG_COMPILER_UNKNOWN
#endif

/* IAR C/C++ */
#ifdef __IAR_SYSTEMS_ICC__
#define CONFIG_COMPILER_IAR
#undef CONFIG_COMPILER_UNKNOWN
#endif

/* Intel C/C++ */
#ifdef __INTEL_COMPILER
#define CONFIG_COMPILER_INTEL
#undef CONFIG_COMPILER_UNKNOWN
#endif

/* Keil C166 */
#ifdef __C166__
#define CONFIG_COMPILER_KEILC166
#undef CONFIG_COMPILER_UNKNOWN
#endif

/* Keil C51 */
#ifdef __C51__
#define CONFIG_COMPILER_KEILC51
#undef CONFIG_COMPILER_UNKNOWN
#endif
#ifdef __CX51__
#define CONFIG_COMPILER_KEILC51
#undef CONFIG_COMPILER_UNKNOWN
#endif
 
/* LCC */
#ifdef __LCC__
#define CONFIG_COMPILER_LCC
#undef CONFIG_COMPILER_UNKNOWN
#endif

/* LLVM */
#ifdef __llvm__
#define CONFIG_COMPILER_LLVM
#undef CONFIG_COMPILER_UNKNOWN
#endif
 
/* Metrowerks CodeWarrior */
#ifdef __MWERKS_
#define CONFIG_COMPILER_MWERKS
#undef CONFIG_COMPILER_UNKNOWN
#endif
#ifdef __CWCC__
#define CONFIG_COMPILER_MWERKS
#undef CONFIG_COMPILER_UNKNOWN
#endif
 
/* MinGW */
#ifdef __MINGW32__
#define CONFIG_COMPILER_MINGW
#undef CONFIG_COMPILER_UNKNOWN
#endif
 
/* MIPSpro */
#ifdef __sgi
#define CONFIG_COMPILER_MIPSPRO
#undef CONFIG_COMPILER_UNKNOWN
#endif
#ifdef sgi
#define CONFIG_COMPILER_MIPSPRO
#undef CONFIG_COMPILER_UNKNOWN
#endif
 
/* Microsoft Visual C++ */
#ifdef _MSC_VER
#define CONFIG_COMPILER__MICROSOFT
#if (_MSC_VER < 1000)
#error "Requires MS C/C++ 10.0 or greater"
#endif
#undef CONFIG_COMPILER_UNKNOWN
#endif
 
/* Small Device C Compiler */
#ifdef SDCC
#define CONFIG_COMPILER_SDCC
#undef CONFIG_COMPILER_UNKNOWN
#endif
 

/* Small Tiny C Compiler */
#ifdef __TINYC__
#define CONFIG_COMPILER_TINYC
#undef CONFIG_COMPILER_UNKNOWN
#endif

#if defined(__STDC__)
#define CONFIG_STDC
#endif

#if defined(__cplusplus)
#define CONFIG_STDCPP
#endif

#endif /* _AUTOSCAN_H_7484_ */



 
 