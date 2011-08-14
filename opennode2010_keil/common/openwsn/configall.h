#ifndef _DEVX_CONFIGALL_H_4237_
#define _DEVX_CONFIGALL_H_4237_
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

/******************************************************************************
 * @author zhangwei on 2006-07-20
 * this is the configure file of the whole project
 * you can #define or #undef the macros in this file to enable/disable some
 * software modules.
 *
 * every module including re-configure functions SHOULD include this file as
 * their first one. this is implemented by including "foundation.h". the "foundation.h"
 * has already included the "configall.h"
 *
 * @modified by zhangwei on 2006-07-20
 * @modified by zhangwei on 20061015
 * add macro CONFIG_MAX_FRAME_LENGTH
 *
 * @modified by zhangwei on 200601-26
 * add macros to support different hardware platforms
 *
 * @modified by zhangwei on 20060510
 * replace TARGET_XXX with CONFIG_TARGET_XXX
 *
 * @modified by zhangwei on 20090523
 *	- replace CONFIG_TARGET_GAINS with CONFIG_TARGET_GAINZ
 *	- enable CONFIG_COMPILER_GNU by default
 *  - enable #define CONFIG_OS_OSX and disable CONFIG_OS_TINYOS at the same time
 * @modified by zhangwei on 20090708
 *  - add fundamental data types definition 
 * @modified by zhangwei on 20100712
 *  - intx/uintx are reconfigurable data types if the compiler support them. 
 *    in the past, they're bounded to the CPU architecture. 
 * @modified by zhangwei on 20110301
 *  - add warning alert for uintx. you cannot define it as uint8 or mistake encountered.
 *
 * @modified by zhangwei on 2011.07.19
 * - add support to automatic compiler detection of ARMCC (from arm.com)
 * 
 * @modified by zhangwei on 2011.08.14
 * - Modify CONFIG_TARGET_GAINZ as CONFIG_TARGETBOARD_GAINZ
 *
 *****************************************************************************/


/******************************************************************************
 * @attention
 *                  IMPORTANT ISSUE ON HOW TO USE THIS FILE
 *
 * OPENWSN_XXX are constant macro definitions. you should NOT change them. while,
 * CONFIG_XXX are configure setting macros. you can change them to reflect your own
 * settings
 *
 * if you want to cancel some macro, pls use #undef macro. you can just place
 * the #undef after the previous macro. if you want it to take effective, you
 * can place the #undef XXX macro before the configuration macro.
 *
 * NOT to use "//" to comment these macros. because future developes may think
 * they are really comments and deleted them!
 *****************************************************************************/

/* Detect current compiler settings automatically. This can help to develope compiler
 * portable applications. Generally, you needn't modify the "autoscan" module.
 */
#include "autoscan.h"

 /* attention
  * in the future, this file will be generated by some utility to help customize openwsn */
  
#define CONFIG_DEBUG

/******************************************************************************
 * User Changable Configurations
 *****************************************************************************/

/* Valid version numbers include
 * 	1.0.0.x: 
 *  1.5.0.x:
 * 	2.0.0.x: 
 * 	2.1.0.x:
 *	2.2.0.x:
 *	2.3.0.x:
 * Currently, there should only 2.4.0.x supported.
 */
#define OPENWSN_MAJOR_VERSION 2
#define OPENWSN_MINOR_VERSION 4
#define OPENWSN_MICRO_VERSION 0
#define OPENWSN_BUILD_VERSION 365

/* a macro to enable the debug source code
 * for release version, you should undef this macro
 */
#ifdef CONFIG_DEBUG
  #define GDEBUG
#endif

/* Hardware Platform Choosing Configuration
 * now we have four hardware platforms:
 * - CONFIG_TARGETBOARD_OPENNODE_10  for OpenNODE version 1.0
 * - CONFIG_TARGETBOARD_OPENNODE_20  for OpenNODE version 2.0
 * - CONFIG_TARGETBOARD_OPENNODE_30  for OpenNODE version 3.0
 * - CONFIG_TARGETBOARD_WLSMODEM_11 for WlsModem version 1.1
 * - CONFIG_TARGETBOARD_GAINZ for ICT's GAINZ hardware
 *
 * @attention: there're only one above macro allowed in the system!
 * currently, openwsn only support OPENNODE_10, 20, 30
 */
#define CONFIG_TARGETBOARD_OPENNODE_10
#undef  CONFIG_TARGETBOARD_OPENNODE_10

#define CONFIG_TARGETBOARD_OPENNODE_20
#undef  CONFIG_TARGETBOARD_OPENNODE_20

#define CONFIG_TARGETBOARD_OPENNODE_30
#undef  CONFIG_TARGETBOARD_OPENNODE_30

#undef  CONFIG_TARGETBOARD_OPENNODE2010
#define CONFIG_TARGETBOARD_OPENNODE2010

#define CONFIG_TARGETBOARD_WLSMODEM_11
#undef  CONFIG_TARGETBOARD_WLSMODEM_11

#define CONFIG_TARGETBOARD_GAINZ
#undef  CONFIG_TARGETBOARD_GAINZ

#define CONFIG_TARGETBOARD_CC2520DK
#undef  CONFIG_TARGETBOARD_CC2520DK

#define CONFIG_TARGETBOARD_CC2430DK
#undef  CONFIG_TARGETBOARD_CC2430DK

//
// #define CONFIG_HARDWARE_CPU
// #define CONFIG_HARDWARE_MCU
// #define CONFIG_HARDWARE_TARGETBOARD
//


/* Compiler used to Compile the Source code
 * CONFIG_COMPILER_GNU
 * should be defined when using GNU c/c++ compiler. this includes:
 *	- WinARM (GNU compilers for ARM)
 *	- WinAVR for GAINZ platform
 *	- AVR Studio for GAINZ platform
 * 
 * CONFIG_TOOLCHAIN_MDK
 * ARM Development Kit(MDK) is provided by ARM. It's used by Keil uVision IDE.
 * Which is the default developing software for OpenNode-2010.  
 * Another suggestion for OpenNode-2010 is TrueSTUDIO from ST.com. 
 */
 
/*
 * Detect compiler type automatically. If the following macro segment cannot detect
 * your compiler, then you can assign the compiler macro manually.
 */

/* @attention
 * - Macro __arm__ is always defined for the ARM compiler. Using __ARMCC_VERSION to 
 * to distinguish between RVCT and other tools that define __arm__.
 * - The ARM compiler in Keil also support __GNUC__ macro in GNU mode.
 */
#ifdef __arm__
#ifdef __ARMCC_VERSION
#endif
#endif
 
#define CONFIG_COMPILER_ADS
#undef  CONFIG_COMPILER_ADS

#define CONFIG_COMPILER_GNU
#undef  CONFIG_COMPILER_GNU

#undef  CONFIG_TOOLCHAIN_MDK
#define CONFIG_TOOLCHAIN_MDK

#ifdef __BORLANDC__
	#define CONFIG_COMPILER_BORLAND
	#if (__BORLANDC__ < 0x500)
		#error "Requires Borland C/C++ 5.0 or greater"
	#endif
	#define CONFIG_WINDOWS
#endif


/* "inline" and "_Bool" are standard C99 keywords. but not all the embedded compilers
 * support they two.
 *
 * Reference
 * [1] http://en.wikibooks.org/wiki/C_Programming/Reference_Tables
 * [2] http://publib.boulder.ibm.com/infocenter/lnxpcomp/v8v101/index.jsp?topic=/com.ibm.xlcpp8l.doc/language/ref/keyw.htm
 * [3] Supported Features of C99, http://docs.sun.com/source/819-3688/c99.app.html
 */
 
#ifdef CONFIG_COMPILER_GNU
  #define INLINE __inline__
#else
  #define INLINE inline
#endif

/* Keil MDK cannot recognize "inline", but it do support "__inline". */
#ifdef CONFIG_TOOLCHAIN_MDK
  #define inline __inline
#endif

#ifdef CONFIG_COMPILER_UNKNOWN
#error "You should choose at least one compiler by defining related macro! Or else simply choose CONFIG_STDC or CONFIG_STDCPP"
#define CONFIG_STDC
#endif

/* "openwsn" is designed to be integrated with existed mature OS. currently, it
 * only support uCOS-II. you can change the following macro to configure the OS
 * to be integrated.
 * currently, it only support uCOS. it's also the default settings.
 * in the future, the default setting will be OS_NONE
 */
#undef  CONFIG_OS_NONE
#undef  CONFIG_OS_TINYOS
#undef  CONFIG_OS_UCOSII
#undef  CONFIG_OS_EMBEDDEDLINUX
#undef  CONFIG_OS_FREERTOS
#define CONFIG_OS_OSX


/*******************************************************************************
 * Fundamental Data Types Used in All the Modules
 * attention the following code should be verified when the project porting to
 * a new hardware and compiling environment. 
 ******************************************************************************/

/* intx and uintx: are reconfigurable data types. they can be defined as int8/uint8,
 * int16/uint16, int32/uint32 no matter what kind of CPU used. 
 * 
 * in the past, these data types are related to CPU architecture. for most of the 
 * 8bit MCUs, intx/uintx are 8 bit, and for ARM, they're usually 32bit. however, 
 * this isn't always. you can still redefine uintx as uint16 on 8bit ATmega128 MCU
 * unless the compiler support them. 
 */
 
/*
 * @warning
 * - considering soome components uses uintx to respresent memory block size, you 
 * cannot define uintx as uint8. The valid definition is only by uint16 and uint32!!!
 * If you define uintx as uint8, a lot of objects such as TiFrame will encounter 
 * running mistake.
 *   You needn't worry about uint16 on 8bit MCUs because almost all 8bit MCU support
 * 16bit integer by hardware or software. You can freely use int16 on these MCUs.
 */

/* for atmega128 MCU and avr-gcc (WinAVR or AVR Studio)
 * ref: <stdint.h> in WinAVR
 */
#ifdef CONFIG_TARGET_GAINZ 
typedef signed char         int8;
typedef unsigned char       uint8;
typedef signed short int    int16;
typedef unsigned short int  uint16;
typedef signed long         int32;
typedef unsigned long       uint32;
typedef signed long long    int64;
typedef unsigned long long  uint64;
typedef float               fp32;             /* single precision floating point variable (32bits)  */
typedef double              fp64;             /* double precision floating point variable (64bits)  */
typedef signed char         intx;
//typedef unsigned char       uintx;
#define uintx uint16
#define intx int16
#endif

#ifndef CONFIG_TARGET_GAINZ 
typedef signed char         int8;
typedef unsigned char 	    uint8;
typedef signed short        int16; // todo?
typedef unsigned short 	    uint16; // todo?
typedef signed long	        int32;
typedef unsigned long 	    uint32;
typedef long long 		    int64;
typedef unsigned long long  uint64;
typedef float          	    fp32;             /* single precision floating point variable (32bits)  */
typedef double         	    fp64;             /* double precision floating point variable (64bits)  */
typedef signed int          intx;
typedef unsigned int        uintx;
#endif

#ifdef CONFIG_TARGETBOARD_OPENNODE2010
  #ifdef CONFIG_TOOLCHAIN_MDK
    //#define uint8_t uint8
    //#define uint16_t uint16
    //#define uint32_t uint32
  #endif 

  #if defined (__CC_ARM)
    #define __ASM            __asm    		/*!< asm keyword for ARM Compiler          */
    #define __INLINE         __inline       /*!< inline keyword for ARM Compiler       */

  #elif defined (__ICCARM__)
    #define __ASM           __asm           /*!< asm keyword for IAR Compiler          */
    #define __INLINE        inline          /*!< inline keyword for IAR Compiler. Only avaiable in High optimization mode! */

  #elif defined (__GNUC__)
    #define __ASM            __asm          /*!< asm keyword for GNU Compiler          */
    #define __INLINE         inline         /*!< inline keyword for GNU Compiler       */

  #elif defined (__TASKING__)			
    #define __ASM            __asm          /*!< asm keyword for TASKING Compiler      */
    #define __INLINE         inline         /*!< inline keyword for TASKING Compiler   */
  #endif	
  
  // The OpenNode 2010 platform (STM32F103x + cc2520) supports the following three
  // macros: 
  // - CONFIG_CPU_FREQUENCY_8MHZ 
  // - CONFIG_CPU_FREQUENCY_48MHZ 
  // - CONFIG_CPU_FREQUENCY_72MHZ 
  // You should define one and only one above macro.
  
  #define CONFIG_CPU_FREQUENCY_8MHZ 1
  //#warning "The target board running at 8MHz defined by CONFIG_CPU_FREQUENCY_8MHZ in configall.h"
#endif 

#ifndef CONFIG_WINDOWS
	#ifndef NULL
		#define NULL (void *)0
	#endif
#endif

#ifndef __cplusplus
	#define bool char
    #define true 1
    #define false 0
#endif

/* If you compile this on Windows, you may encounter compiling error due to the BOOL
 * confliction with windows default definition. It this occurs, you can uncomment 
 * BOOL define here
 */
#ifndef BOOL
	#define BOOL char
#endif

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#define BYTE uint8
#define WORD uint16
#define DWORD uint32
#define QWORD uint64

#ifdef __IAR_SYSTEMS_ICC__
#warning "The above macros TRUE and FALSE already defined"
#endif

#endif
