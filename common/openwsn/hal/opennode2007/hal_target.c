/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			target.c
** Last modified Date:	2004-09-17
** Last Version:		1.0
** Descriptions:		header file of the specific codes for LPC2100 target boards
**						Every project should include a copy of this file, user may modify it as needed
**------------------------------------------------------------------------------------------------------
** Created by:			Chenmingji
** Created date:		2004-02-02
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			Chenmingji
** Modified date:		2004-09-17
** Version:				1.01
** Descriptions:		Renewed the template, added more compiler supports
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
********************************************************************************************************/

//#define IN_TARGET
#include "arch_foundation.h"
#include "lpc2294.h"
#include "os_cfg.h"
#include "arch_target.h"
//#include "../../ucos-ii/ucos_ii.h"

/****************************************************************************
 * @modified by zhangwei on 200903
 * 	- great revision today. rename this from "target.c" to "arch_target.c"
 * and move it to "target" directory.
 ***************************************************************************/

// @modified by zhangwei on 20061025
// this variable is added by huanghuan
// i think it is no use
// zhangwei comment it in 200903, please remove in the near future
//uint8 IRQ_FLAG;

/*********************************************************************************************************
** Function name:			IRQ_Exception
**
** Descriptions:			interrupt exceptional handler , change it as needed
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
** Modified by:
** Modified date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void __irq IRQ_Exception(void)
{
    while(1);                   //  change it to your code 这一句替换为自己的代码
}

/*********************************************************************************************************
** Function name:			FIQ_Exception
**
** Descriptions:			Fast interrupt exceptional handler , change it as needed
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
** Modified by:
** Modified date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void FIQ_Exception(void)
{
    while(1);                   // change it to your code  这一句替换为自己的代码
}
/*********************************************************************************************************
** Function name:			Timer0_Exception
**
** Descriptions:			Timer0 interrupt service function
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
** Modified by:
** Modified date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void Timer0_Exception(void)
{
    T0IR = 0x01;
    VICVectAddr = 0;            //interrupt close 通知中断控制器中断结束
    
    /* @todo 
     * OSTimeTick is implemented in ucos-ii/os_core.c 
     * so i simply comment it today in the revision 
      !!!! you should comment the following line. because it drive the running the ucos 
      i comment it just want to compile sucessfully.
     */
    // OSTimeTick();
}

/*********************************************************************************************************
** Function name:			Timer0Init
**
** Descriptions:			Initialize the Time0
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
** Modified by:
** Modified date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void Timer0Init(void)
{
    T0IR = 0xffffffff;
    T0TC = 0;
    T0TCR = 0x01;
    T0MCR = 0x03;
    T0MR0 = (Fpclk / HAL_TICKS_PER_SEC);
 }
/*********************************************************************************************************
** Function name:			VICInit
**
** Descriptions:			Initialize the Interrupt Vevtor Controller
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
** Modified by:
** Modified date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

void VICInit(void)
{
// @modified by zhangwei on 20061010
// 与Huanghuan进行归并，但是hh中没有这个函数。我拿不准是否应该删除. 很可能这是为了RTOS
// @TODO
/*
    extern void IRQ_Handler(void);
    extern void Timer0_Handler(void);

    VICIntEnClr = 0xffffffff;
    VICDefVectAddr = (uint32)IRQ_Handler;

    VICVectAddr0 = (uint32)Timer0_Handler;
    VICVectCntl0 = (0x20 | 0x04);
    VICIntEnable = 1 << 4; */
}

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
** Modified by:
** Modified date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void TargetInit(void)
{
	// @modified by zhangwei on 20061013
	// @TODO
	// 黄欢的代码中没有如下这几句 20061010
	// 但是张伟原先的版本中有这样几句
	// 这几句很可能是为了跑uCOS，要保留的——有待确认
    //HAL_ENTER_CRITICAL();
    hal_irq_disable();
    //srand((uint32) TargetInit); // 这里什么意思?不懂?所以张伟comment it @modified by zhangwei on 20070623
    VICInit();
    Timer0Init();
    hal_irq_enable();
    //HAL_EXIT_CRITICAL();
}
/*********************************************************************************************************
** Function name:			TargetInitUart0
**
** Descriptions:			Initialize the Uart0
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
** Modified by:
** Modified date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
void TargetInitUart0(uint32 bps)
{
    uint16 Fdiv;

    PINSEL0 = (PINSEL0 & 0xfffffff0) | 0x05;    /* Select the pins for Uart 选择管脚为UART0 */

    U0LCR = 0x80;                               /* Enable to access the frequenc regecter 允许访问分频因子寄存器 */
    Fdiv = (Fpclk / 16) / bps;                  /* Set the baudrate设置波特率 */
    U0DLM = Fdiv / 256;
	U0DLL = Fdiv % 256;
    U0LCR = 0x03;                               /* Disable to access the frequenc regecter 禁止访问分频因子寄存器 */
                                                /* set to 8,1,n 且设置为8,1,n */
	U0IER = 0x00;                               /* Disable interrupt禁止中断 */
    U0FCR = 0x00;                               /* initial FIFO 初始化FIFO */
}
/*********************************************************************************************************
** Function name:			TargetResetInit
**
** Descriptions:			Initialize the target
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
** Modified by:
** Modified date:
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

void TargetResetInit(void)
{
#ifdef __DEBUG_RAM
    MEMMAP = 0x2;                   //remap
#endif

#ifdef __DEBUG_FLASH
    MEMMAP = 0x1;                   //remap
#endif

#ifdef __IN_CHIP
    MEMMAP = 0x1;                   //remap
#endif

	// 黄欢原有代码没有这句，但张伟版本中有这句，暂时保留
    PINSEL0 = (PINSEL0 & 0xFFFF0000) | 0x05 | 0x50;

/* 设置系统各部分时钟 */
/* Set system timers for each component */
    PLLCON = 1;
#if (Fpclk / (Fcclk / 4)) == 1
    VPBDIV = 0;
#endif
#if (Fpclk / (Fcclk / 4)) == 2
    VPBDIV = 2;
#endif
#if (Fpclk / (Fcclk / 4)) == 4
    VPBDIV = 1;
#endif

#if (Fcco / Fcclk) == 2
    PLLCFG = ((Fcclk / Fosc) - 1) | (0 << 5);
#endif
#if (Fcco / Fcclk) == 4
    PLLCFG = ((Fcclk / Fosc) - 1) | (1 << 5);
#endif
#if (Fcco / Fcclk) == 8
    PLLCFG = ((Fcclk / Fosc) - 1) | (2 << 5);
#endif
#if (Fcco / Fcclk) == 16
    PLLCFG = ((Fcclk / Fosc) - 1) | (3 << 5);
#endif
    PLLFEED = 0xaa;
    PLLFEED = 0x55;
    while((PLLSTAT & (1 << 10)) == 0);
    PLLCON = 3;
    PLLFEED = 0xaa;
    PLLFEED = 0x55;

/* 设置存储器加速模块 */
/* Set memory accelerater module*/
    MAMCR = 0;
#if Fcclk < 20000000
    MAMTIM = 1;
#else
#if Fcclk < 40000000
    MAMTIM = 2;
#else
    MAMTIM = 3;
#endif
#endif
    MAMCR = 2;

	/* 设置串行口 */
	/* initialize UART*/
    TargetInitUart0( CONFIG_UART0_STARTUP_BAUDRATE );

	/* 设置实时时钟 */
	/* initialize RTC*/
    CCR = 1;
    PREINT = Fpclk / 32768 - 1;
    PREFRAC = Fpclk - (Fpclk / 32768) * 32768;
    YEAR = 2003;
    MONTH = 6;
    DOM = 2;

	/* initialize VIC*/
    VICIntEnClr = 0xffffffff;
    VICVectAddr = 0;
    VICIntSelect = 0;
    T0IR = 0xffffffff;
    T0TCR = 0X02;
}


/* 在某个网站上看到ZLG公司技术人员回答：
 * Q:请问陈版主ADS1.2可以使用semihosting技术么?
 * R:需要仿真器支持(EasyJTAG暂时不支持)。
 */

/****************************************************************************
**                  以下为一些与系统相关的库函数的实现
**                  具体作用请ads的参考编译器与库函数手册
**                  用户可以根据自己的要求修改
****************************************************************************/
/****************************************************************************
**                  The implementations for some library functions
**                  For more details, please refer to the ADS compiler handbook and The library
**					function manual
**                  User could change it as needed
****************************************************************************/

/* @attention
 * header file "rt_sys.h" is provided by ARM's ADS, RVDS... If you use other
 * compilers, you may need to comment it or refer to the compile instructions
 */
#include "rt_sys.h"
#include "stdio.h"

#pragma import(__use_no_semihosting_swi)

int __rt_div0(int a)
{
    a = a;
    return 0;
}

int fputc(int ch,FILE *f)
{
    ch = ch;
    f = f;
    return 0;
}

int fgetc(FILE *f)
{
    f = f;
    return 0;
}

int _sys_close(FILEHANDLE fh)
{
    fh = fh;
    return 0;
}

int _sys_write(FILEHANDLE fh, const unsigned char * buf, unsigned len, int mode)
{
    fh = fh;
    buf = buf;
    len =len;
    mode = mode;
    return 0;
}
int _sys_read(FILEHANDLE fh, unsigned char * buf, unsigned len, int mode)
{
    fh = fh;
    buf = buf;
    len =len;
    mode = mode;

    return 0;
}

void _ttywrch(int ch)
{
    ch = ch;
}

int _sys_istty(FILEHANDLE fh)
{
    fh = fh;
    return 0;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
    fh = fh;
    return 0;
}

int _sys_ensure(FILEHANDLE fh)
{
    fh = fh;
    return 0;
}

long _sys_flen(FILEHANDLE fh)
{
    fh = fh;
    return 0;
}

int _sys_tmpnam(char * name, int sig, unsigned maxlen)
{
    name = name;
    sig = sig;
    maxlen = maxlen;
    return 0;
}

void _sys_exit(int returncode)
{
    returncode = returncode;
}

char *_sys_command_string(char * cmd, int len)
{
    cmd = cmd;
    len = len;
    return 0;
}

