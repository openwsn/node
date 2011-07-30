#ifndef _ARCH_FOUNDATION_H_
#define _ARCH_FOUNDATION_H_

#include "arch_configall.h"


/********************************/
/*      uC/OS-II specital code  */
/*      uC/OS-II的特殊代码      */
/********************************/

#define     USER_USING_MODE    0x10                    /*  User mode ,ARM 32BITS CODE 用户模式,ARM代码                  */
//
                                                     /*  Chosen one from 0x10,0x30,0x1f,0x3f.只能是0x10,0x30,0x1f,0x3f之一       */
//#include "Includes.h"


/********************************/
/*      ARM的特殊代码           */
/*      ARM specital code       */
/********************************/
//这一段无需改动
//This segment should not be modify

#include    "LPC2294.h"


/********************************/
/*     应用程序配置             */
/*Application Program Configurations*/
/********************************/
//以下根据需要改动
//This segment could be modified as needed.
#ifndef CONFIG_WINDOWS
#include    <stdio.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <setjmp.h>
#include    <rt_misc.h>
#endif

/********************************/
/*     本例子的配置             */
/*Configuration of the example */
/********************************/

/* System configuration .Fosc、Fcclk、Fcco、Fpclk must be defined
 * 系统设置, Fosc、Fcclk、Fcco、Fpclk必须定义 */

/*
#ifndef Fosc
//Crystal frequence,10MHz~25MHz，should be the same as actual status.
//应当与实际一至晶振频率,10MHz~25MHz，应当与实际一致
#define Fosc            11059200
//System frequence,should be (1~32)multiples of Fosc,and should be equal or less  than 60MHz.
//系统频率，必须为Fosc的整数倍(1~32)，且<=60MHZ
#define Fcclk           (Fosc * 4)
//CCO frequence,should be 2、4、8、16 multiples of Fcclk, ranged from 156MHz to 320MHz.
//CCO频率，必须为Fcclk的2、4、8、16倍，范围为156MHz~320MHz
#define Fcco            (Fcclk * 4)
//VPB clock frequence , must be 1、2、4 multiples of (Fcclk / 4).
//VPB时钟频率，只能为(Fcclk / 4)的1、2、4倍
#define Fpclk           (Fcclk / 4) * 1
#endif

//#include    "target.h"              //This line may not be deleted 这一句不能删除
*/


/* Currently, we support several hardware platforms. You can choose the target
 * platform by defining the platform macros in "configall.h". The hardware platform
 * most used is CONFIG_TARGET_OPENNODE_30 now.
 */

/* System wide configuration for lpc213x.
 * Fosc、Fcclk、Fcco、Fpclk must be defined or you may encounter data failure
 * when using UART, especially when the communication data rate is high.
 *
 * Fosc:	Crystal frequence,10MHz~25MHz，should be the same as actual status.
 *          应当与实际一至晶振频率,10MHz~25MHz，应当与实际一致
 * Fcclk:   System frequence,should be (1~32)multiples of Fosc,and should be equal or less  than 60MHz.
 *          系统频率，必须为Fosc的整数倍(1~32)，且<=60MHZ
 * Fcco:    CCO frequence,should be 2、4、8、16 multiples of Fcclk, ranged from 156MHz to 320MHz.
 *          CCO频率，必须为Fcclk的2、4、8、16倍，范围为156MHz~320MHz
 * Fpclk:   VPB clock frequence , must be 1、2、4 multiples of (Fcclk / 4).
 *          VPB时钟频率，只能为(Fcclk / 4)的1、2、4倍
 */
#if (defined(CONFIG_TARGET_OPENNODE_10) || defined(CONFIG_TARGET_OPENNODE_20) || defined(CONFIG_TARGET_WLSMODEM_11))
  #define Fosc            11059200
  #define Fcclk           (Fosc * 4)
  #define Fcco            (Fcclk * 4)
  #define Fpclk           (Fcclk / 4) * 1
#endif

#ifdef CONFIG_TARGET_OPENNODE_30
  #define Fosc            16000000
  #define Fcclk           (Fosc * 4)           // @todo warning; different macro definition why 200903
  #define Fcco            (Fcclk * 4)
  #define Fpclk           (Fcclk / 4) * 1
#endif



#endif

