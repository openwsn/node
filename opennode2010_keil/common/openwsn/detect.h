#ifndef _DETECT_COMPILER_H_
#define _DETECT_COMPILER_H_

/*
 * Compiler Type Detection
 * @reference
 * - Pre-defined C/C++ Compiler Macros, 
 *   http://sourceforge.net/apps/mediawiki/predef/index.php?title=Main_Page
 * - Compiler detection macros,
 *   http://sourceforge.net/apps/mediawiki/predef/index.php?title=Compilers 
 */
 
/* Altium MicroBlaze C */
#ifdef __CMB__
#define CONFIG_COMPILER_MICROBLAZEC
#endif
 
/* ARM Compiler
 * The ARM compiler is also identified by macro  __arm__. 
 * Please note that the __ARMCC_VERSION macro is also used as version indicator 
 * for Norcroft C, but that the format is different.
 */
#ifdef __CC_ARM 
#ifdef __ARMCC_VERSION
#define CONFIG_COMPILER_ARMCC
#endif
#endif
 
/* Borland C++ */
#ifdef __BORLANDC__
#define CONFIG_COMPILER_BORLANDC
#endif
#ifdef __CODEGEARC__
#define CONFIG_COMPILER_BORLANDC
#endif

/* __GNUC__ */
#ifdef __GNUC__
#define CONFIG_COMPILER_GNUC
#endif

/* IAR C/C++ */
#ifdef __IAR_SYSTEMS_ICC__
#define CONFIG_COMPILER_IAR
#endif

/* Intel C/C++ */
#ifdef __INTEL_COMPILER
#define CONFIG_COMPILER_INTEL
#endif

/* Keil C166 */
#ifdef __C166__
#define CONFIG_COMPILER_KEILC166
#endif

/* Keil C51 */
#ifdef __C51__
#define CONFIG_COMPILER_KEILC51
#endif
#ifdef __CX51__
#define CONFIG_COMPILER_KEILC51
#endif
 
/* LCC */
#ifdef __LCC__
#define CONFIG_COMPILER_LCC
#endif

/* LLVM */
#ifdef __llvm__
#define CONFIG_COMPILER_LLVM
#endif
 
/* Metrowerks CodeWarrior */
#ifdef __MWERKS_
#define CONFIG_COMPILER_MWERKS
#endif
#ifdef __CWCC__
#define CONFIG_COMPILER_MWERKS
#endif
 
/* MinGW */
#ifdef __MINGW32__
#define CONFIG_COMPILER_MINGW
#endif
 
/* MIPSpro */
#ifdef __sgi
#define CONFIG_COMPILER_MIPSPRO
#endif
#ifdef sgi
#define CONFIG_COMPILER_MIPSPRO
#endif
 
/* Microsoft Visual C++ */
#ifdef _MSC_VER
#define CONFIG_COMPILER__MSC
#endif
 
/* Small Device C Compiler */
#ifdef SDCC
#define CONFIG_COMPILER_SDCC
#endif
 

/* Small Tiny C Compiler */
#ifdef __TINYC__
#define CONFIG_COMPILER_TINYC
#endif

#endif


 
 