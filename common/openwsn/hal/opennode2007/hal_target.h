#ifndef _TARGET_H_1389_
#define _TARGET_H_1389_
/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name: 			target.h
** Last modified Date:  2004-09-17
** Last Version: 		1.0
** Descriptions: 		header file of the specific codes for LPC2100 target boards
**						Every project should include a copy of this file, user may modify it as needed
**------------------------------------------------------------------------------------------------------
** Created by: 			Chenmingji
** Created date:   		2004-02-02
** Version:				1.0
** Descriptions: 		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by: 		Chenmingji
** Modified date:		2004-09-17
** Version:				1.01
** Descriptions: 		Renewed the template, added codes to surport more compilers
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
********************************************************************************************************/

/****************************************************************************
 * target
 * this module contains target related source code. when you porting the whole
 * system onto a new platform, you often need to port the following three functions:
 *	- startup.s  contains ASM source code
 *	- target.h and target.c contains C language source code
 *
 * in openwsn, I also introduce the module arch_targetboard. currently, the
 * module target contains MCU related source code, and targetboard contains
 * multi-chip connection related source code. if you just want to run something
 * inside the chip without peripherials, "target" module should be enough.
 *
 * Reference
 * 	[1] __SWI的具体分析, http://group.ednchina.com/431/5461.aspx
 * 	[2] Keil Discussion Forum: Global Interruption Disable/Enable
 *		http://www.keil.com/forum/docs/thread5584.asp
 *	[3] UCOS-II在LPC2210上的移植－－OS_CPU.h, http://blog.csdn.net/kmajian/archive/2006/04/27/679836.aspx
 *	[x] UCOS-II在LPC2210上的移植－－OS_CPU_c.c , http://blog.csdn.net/kmajian/archive/2006/04/27/679960.aspx
 *	[x] UCOS-II在LPC2210上的移植－－OS_CPU_A.s, http://blog.csdn.net/kmajian/archive/2006/04/28/695592.aspx
 *	[x] uC/OS-II实时操作系统在嵌入式平台上进行移植的一般方法和技巧
 *		http://blog.csdn.net/kmajian/archive/2006/03/09/619448.aspx
 *	[x] uCOS-II在ARM上的移植, 2005, http://blog.csdn.net/kmajian/archive/2006/03/03/614826.aspx
 *	[x] uC/OS-II任务栈处理的一种改进方法, 2006, http://blog.csdn.net/kmajian/archive/2006/03/02/613644.aspx
 *	[x]  改进uC/OS II，减少内存使用量 , http://blog.csdn.net/kmajian/archive/2006/03/02/613641.aspx
 *	[x] μC/OS-II的多任务信息流与CAN总线驱动, http://blog.csdn.net/kmajian/archive/2006/03/02/613627.aspx
 *	[x] uC/OS-II Basic Programmers’Guide, http://blog.csdn.net/kmajian/archive/2006/03/02/613623.aspx
 *	[x] CARM User's Guide: __swi: Software Interrupt, http://www.keil.com/support/man/docs/ca/ca_le_swi.htm
 *	[x] ARM? Developer Suite Developer Guide:  Calling SWIs from an application
 *		http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0056d/ch05s04s04.html
 *
 * @author zhangwei on 200503
 * @modified by zhangwei on 200903
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//#ifndef IN_TARGET

extern void Reset(void);
/*********************************************************************************************************
** Function name:			Reset
** Descriptions:			resets the target board.
** input parameters:		None
**
** Returned value:			None
**
** Used global variables:	None
** Calling modules:			None
**
** Created by:				Chenmingji
** Created Date:			2004/02/02
**-------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

extern void TargetInit(void);
/*********************************************************************************************************
** Function name:			TargetInit
**
** Descriptions:			Initialize the target board; it is called in a necessary place, change it as
**							needed
**
** input parameters:		None
** Returned value:			None
**
** Used global variables:	None
** Calling modules:			None
**
** Created by:				Chenmingji
** Created Date:			2004/02/02
**-------------------------------------------------------------------------------------------------------
** Modified by:				Chenxibing
** Modified date:			2004-12-09
** Notice:					Add IntDisable() and IntEnable() Functions
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
//#endif

#ifdef __cplusplus
}
#endif

// @todo: 
// where's SwiHandle()? I only find SwiFunction in startup.s
// but it wasn't exported.

/* ARM CPU only
 * enable/disable the global interrupt control flag in CPU's flag register
 * for ARM CPU. it support fast interrupt FIQ. in openwsn, we haven't use fast 
 * interrupt yet. So in most cases, FIQ are diabled.
 */
#define hal_irq_enable() SwiHandle1(1)
#define hal_irq_disable() SwiHandle1(0)
#define hal_fiq_disable() SwiHandle1(2)
#define hal_fiq_enable() SwiHandle1(3)

#define _hal_irq_enable() SwiHandle1(1)
#define _hal_irq_disable() SwiHandle1(0)
#define _hal_fiq_disable() SwiHandle1(2)
#define _hal_fiq_enable() SwiHandle1(3)

#define	HAL_ENTER_CRITICAL()	ARMDisableInterrupts()
#define	HAL_EXIT_CRITICAL()	ARMEnableInterupts()


#ifndef CONFIG_WINDOWS
__swi(0x00) void SwiHandle1(int Handle);
#endif

#define IRQDisable() SwiHandle1(0)
#define IRQEnable() SwiHandle1(1)
#define FIQDisable() SwiHandle1(2)
#define FIQEnable() SwiHandle1(3)

#endif
