/*****************************************************************************
* This file is part of OpenWSN, the Open Wireless Sensor Network System.
*
* Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
* 
* OpenWSN is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 or (at your option) any later version.
* 
* OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
* 
* You should have received a copy of the GNU General Public License along
* with eCos; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
* 
* As a special exception, if other files instantiate templates or use macros
* or inline functions from this file, or you compile this file and link it
* with other works to produce a work based on this file, this file does not
* by itself cause the resulting work to be covered by the GNU General Public
* License. However the source code for this file must still be made available
* in accordance with section (3) of the GNU General Public License.
* 
* This exception does not invalidate any other reasons why a work based on
* this file might be covered by the GNU General Public License.
* 
****************************************************************************/ 
#ifndef _TIMERADAPTER_H_6828_
#define _TIMERADAPTER_H_6828_

/******************************************************************************
 * @author zhangwei on 2006-08-11
 * TiTimerAdapter 
 * This object is the software mapping of MCU's hardware timer object.
 * attention that TiTimerAdapter is not the 1:1 mapping of hardware timer. actually, one 
 * TiTimerAdapter object corrspond to 1 separate channel of hardware timer.
 * 
 * @history
 * @modified by zhangwei 2006-09-05
 * @modified by tangwen on 2006-10-21
 * 	add channel parameter to construct function.
 *  and other modifications to support the channel(sub-timer).
 *
 * @modified by Shanlei on 200611
 * 
 *  timer测试通过。本版本说明如下：
 *  timer_setvalue 和 timer_setcapture为空函数。
 *  timer_configure的priority参数表示了timer对象是否使用中断方式，priority
 * 为0-15表示使用中断方式，>15表示不使用中断。
 *  timer_VICinit只是根据timer对象的id 和 priority 设置了VIC地址，priority
 * 的值只能为0-15超出范围不赋予地址，此函数在timer_configure函数中调用，当
 * timer_configure的参数priority在0-15之间。原本两个timer的VIC地址为同一个
 *（timer_interrupt），经测试发现，当两个timer同时进入中断的时候因为使用同一
 * 地址，会产生冲突。现在为每一个timer分配一个VIC地址（Timer0_Int for timer0 
 * and Timer1_Int for timer1）解决了这种冲突。
 *  从timer_VICinit中分离出timer_VICenable和timer_VICdisable控制整个timer的
 * 使能或禁止，这两个函数在timer_start和timer_stop中根据timer->state被调用。
 *  timer_setinterval的计时最小间隔改为1ms，参数interval的值代表ms数。
 * repeat功能恢复，repeat为0，只匹配一次；repeat为1，连续匹配。
 *  timer_enable和timer_disable是对timer的每个比较通道中断的使能或禁止，在
 * 使用timer_expired的查询方式下，也需要使能比较通道的中断，以便产生中断标志，
 * 因此，这两个函数不能作为timer真正使用中断方式的表现，他们将不改变timer-
 * >state。
 *   timer_VICinit、timer_VICenable、timer_VICdisable、timer_enable、
 * timer_disable对上层都是不可见的。
 *
 * modified by zhangwei on 200612
 * timer的启动与停止建议使用start/stop，不要使用disable和enable
 *
 *****************************************************************************/
 
#include "hal_foundation.h"

#define CONFIG_TIMERADAPTER_COUNT 4
#define GET_TIMERADAPTER(id) (&g_timer[id])

/* timer对象的几种工作方式
 * 实际中，timer还可以继续分通道，每个通道相当于一个小Timer
 */
typedef struct{
	uint8 id;
	uint8 state;
	uint32 interval;
	void * owner;
	TiEventHandler evthandler
	TiNotifier * notifier;
};

extern g_timer[CONFIG_TIMERADAPTER_COUNT];

TiTimerAdapter* timer_construct( char * buf, uint8 size );
void   timer_destroy( TiTimerAdapter * timer );
void   timer_init( TiTimerAdapter * timer, uint8 id, uint8 channel );
void   timer_configure( TiTimerAdapter * timer, TEventHandler callback, void * data, uint8 priority );

start
stop
restart
enable
disable


void   timer_setvalue( TiTimerAdapter * timer, uint32 value );
uint32 timer_getvalue( TiTimerAdapter * timer );
void   timer_setinterval( TiTimerAdapter * timer, uint32 interval, uint8 repeat );
void   timer_setcapture( TiTimerAdapter * timer, uint8 opt );
void   timer_start( TiTimerAdapter * timer );
void   timer_stop( TiTimerAdapter * timer );
void   timer_restart( TiTimerAdapter * timer, uint32 interval, uint8 repeat );
boolean timer_expired( TiTimerAdapter * timer );
uint32 timer_elapsed( TiTimerAdapter * timer );
void   timer_enable( TiTimerAdapter * timer );
void   timer_disable( TiTimerAdapter * timer );
uint32 timer_clocksperms( TiTimerAdapter * timer );
void   timer_VICinit( TiTimerAdapter * timer );
void   timer_VICenable( TiTimerAdapter * timer);
void   timer_VICdisable( TiTimerAdapter * timer);




/* TiTimerAdapter Object
 *  id		timer hardware id. usually it is 0, 1, 2. it depends on hardware
 *  channel	each sub-timer in timer hardware. usually 0-3
 *	state	bit 7 - bit 0
 *			bit 0: enabled or not
 *			bit 1: period trigger or not
 *			bit 2: raise interrupt or not
 *	data	used as the parameter for the timer callback function
 */
#define TIMER_STATE_ENABLED 	0x01
#define TIMER_STATE_PERIOD	 	0x02
#define TIMER_STATE_INTERRUPT 	0x04
#define TIMER_STATE_CAPTURE 	0x08

typedef struct{
  uint8 id;
  uint8 state;
  uint32 interval;
  void * data;
  TEventHandler callback;
  uint8 priority;
  uint32 lasttime;
  // @TODO 20061010
  // 以下为黄欢定义的Timer结构，两个结构要实现归并，以上一个为主  
  uint8 enabled;
  uint16 ctrl;
  uint16 init;
  void (* hhcallback)(void *);
}TiTimerAdapter;

/* global variables define in "hal_global.c". They will be initialized in  
 * hal_global_construct(). */

extern TiTimerAdapter * g_timer0;
extern TiTimerAdapter * g_timer1;
extern TiTimerAdapter * g_timer2;

/* This macro defines how many clocks the Timer run in one milli-seconds.
 * since we often use milli-seconds when dealing with scheduling and communication
 * problems, this macro is quite good for such conversion.
 */
#define TIMER_CLOCKS_PER_MILLISECOND  (Fpclk / 1000)
#define timer_msecof(clocks) (clocks/TIMER_CLOCKS_PER_MILLISECOND)
#define timer_clocksof(ms) (ms*TIMER_CLOCKS_PER_MILLISECOND)

TiTimerAdapter* timer_construct( char * buf, uint8 size );
void   timer_destroy( TiTimerAdapter * timer );
void   timer_init( TiTimerAdapter * timer, uint8 id, uint8 channel );
void   timer_configure( TiTimerAdapter * timer, TEventHandler callback, void * data, uint8 priority );
void   timer_setvalue( TiTimerAdapter * timer, uint32 value );
uint32 timer_getvalue( TiTimerAdapter * timer );
void   timer_setinterval( TiTimerAdapter * timer, uint32 interval, uint8 repeat );
void   timer_setcapture( TiTimerAdapter * timer, uint8 opt );
void   timer_start( TiTimerAdapter * timer );
void   timer_stop( TiTimerAdapter * timer );
void   timer_restart( TiTimerAdapter * timer, uint32 interval, uint8 repeat );
boolean timer_expired( TiTimerAdapter * timer );
uint32 timer_elapsed( TiTimerAdapter * timer );
void   timer_enable( TiTimerAdapter * timer );
void   timer_disable( TiTimerAdapter * timer );
uint32 timer_clocksperms( TiTimerAdapter * timer );
void   timer_VICinit( TiTimerAdapter * timer );
void   timer_VICenable( TiTimerAdapter * timer);
void   timer_VICdisable( TiTimerAdapter * timer);

void __irq Timer0_Int (void);
void __irq Timer1_Int (void);

  

// @TODO
// 以下为黄欢提供的新代码 20061010
// these are obsolete source code! you should not use them in the future!
//
// should be integrated with the above in the near future
// to support one timer object only



void INTERRUPT_LPC_TIMER0_T0MR0(void);
void INTERRUPT_LPC_TIMER0_T0MR2(void);
void INTERRUPT_LPC_TIMER0_CAPT(void);


#define MR0IF 0
#define MR1IF 1
#define MR2IF 2
#define MR3IF 3
#define CR0IF 4
#define CR1IF 5
#define CR2IF 6
#define CR3IF 7
  
#define CAP0RE  0
#define CAP0FE  1
#define CAP0I   2
#define CAP1RE  3
#define CAP1FE  4
#define CAP1I   5
#define CAP2RE  6
#define CAP2FE  7
#define CAP2I   8

#define MR0I  0
#define MR0R  1
#define MR1I  3
#define MR1R  4
#define MR2I  6
#define MR2R  7

void LPC_TIMER0_STOP(void);
void LPC_TIMER0_START(void);
void LPC_TIMER0_INIT(void);
void LPC_TIMER0_SET_T0MR0_VALUE(uint32 value);
void LPC_TIMER0_SET_T0MR1_VALUE(uint32 value);
void LPC_TIMER0_SET_T0MR2_VALUE(uint32 value);
void LPC_TIMER0_CLEAR_T0MR0(void);
void LPC_TIMER0_CLEAR_T0MR1(void);
void LPC_TIMER0_CLEAR_T0MR2(void);

void DISABLE_LPC_TIMER0_T0MR0_INT(void);
void ENABLE_LPC_TIMER0_T0MR0_INT(void);
void DISABLE_LPC_TIMER0_T0MR2_INT(void);
void ENABLE_LPC_TIMER0_T0MR2_INT(void);
void DISABLE_LPC_TIMER0_T0CAP2_INT(void);
void ENABLE_LPC_TIMER0_T0CAP2_INT(void);
void ENABLE_LPC_TIMER0_INT(void);
void DISABLE_LPC_TIMER0_INT(void);


#endif /* _TIMERADAPTER_H_6828_ */

