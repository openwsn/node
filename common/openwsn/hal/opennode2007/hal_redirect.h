#ifndef _ARCH_RETARGET_H_
#define _ARCH_RETARGET_H_
/*****************************************************************************
 * retarget.c
 * re-direct C/C++ library functions, usually the standard I/O
 *
 * Q: How do I retarget C++ streamed I/O?
 * R: Applies to: ARM Developer Suite (ADS), RealView Developer Kit (RVDK) for
 * OKI, RealView Developer Kit (RVDK) for ST, RealView Developer Kit for XScale
 * (RVXDK), RealView Development Suite (RVDS)
 *
 * The C++ libraries depend on the C library for target-specific support. There
 * are no target dependencies in the C++ libraries.
 *
 * To retarget your C++ streamed I/O, we recommend that you retarget the "_sys_xxxx"
 * driver level functions. These are the functions used by the C and C++ library
 * and are responsible for target specific I/O. The default implementation uses
 * Semihosting to handle I/O on the host machine.
 *
 * Reference
 * ARM Technical Support Knowledge Articles: How do I retarget C++ streamed I/O?
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka3844.html
 *
 * @author zhangwei on 200903
 * @modified by zhangwei on 200903
 ****************************************************************************/

#endif
