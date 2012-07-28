/**************************************************************************************************
  Filename:       hal_timer.h
  Revised:        $Date: 2007-07-06 10:42:24 -0700 (Fri, 06 Jul 2007) $
  Revision:       $Revision: 13579 $

  Description:    This file contains the interface to the Timer Service.


  Copyright 2005-2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************************************
 *                                            INCLUDES
 ***************************************************************************************************/
#include "hal_board.h"

/***************************************************************************************************
 *                                             MACROS
 ***************************************************************************************************/



/***************************************************************************************************
 *                                            CONSTANTS
 ***************************************************************************************************/
/* Timer ID definitions */
#define HAL_TIMER_0                0x00    // 8bit timer
#define HAL_TIMER_1                0x01    // 16bit Mac timer
#define HAL_TIMER_2                0x02    // 8bit timer
#define HAL_TIMER_3                0x03    // 16bit timer
#define HAL_TIMER_MAX              4       // Max number of timer

/* Operation Modes for timer */
#define HAL_TIMER_MODE_NORMAL     0x01    // Normal Mode
#define HAL_TIMER_MODE_CTC        0x02    // Clear Timer On Compare
#define HAL_TIMER_MODE_MASK       (HAL_TIMER_MODE_NORMAL | HAL_TIMER_MODE_CTC)

/* Channel definitions */
#define HAL_TIMER_CHANNEL_SINGLE   0x01    // Single Channel - default
#define HAL_TIMER_CHANNEL_A        0x02    // Channel A
#define HAL_TIMER_CHANNEL_B        0x04    // Channel B
#define HAL_TIMER_CHANNEL_C        0x08    // Channel C
#define HAL_TIMER_CHANNEL_MASK    (HAL_TIMER_CHANNEL_SINGLE |  \
                                   HAL_TIMER_CHANNEL_A |       \
                                   HAL_TIMER_CHANNEL_B |       \
                                   HAL_TIMER_CHANNEL_C)

/* Channel mode definitions */
#define HAL_TIMER_CH_MODE_INPUT_CAPTURE   0x01    // Channel Mode Input-Capture
#define HAL_TIMER_CH_MODE_OUTPUT_COMPARE  0x02    // Channel Mode Output_Compare
#define HAL_TIMER_CH_MODE_OVERFLOW        0x04    // Channel Mode Overflow
#define HAL_TIMER_CH_MODE_MASK            (HAL_TIMER_CH_MODE_INPUT_CAPTURE |  \
                                           HAL_TIMER_CH_MODE_OUTPUT_COMPARE | \
                                           HAL_TIMER_CH_MODE_OVERFLOW)

/* Error Code */
#define HAL_TIMER_OK              0x00
#define HAL_TIMER_NOT_OK          0x01
#define HAL_TIMER_PARAMS_ERROR    0x02
#define HAL_TIMER_NOT_CONFIGURED  0x03
#define HAL_TIMER_INVALID_ID      0x04
#define HAL_TIMER_INVALID_CH_MODE 0x05
#define HAL_TIMER_INVALID_OP_MODE 0x06

/* Timer clock pre-scaler definitions for 16bit timer1 and timer3 */
#define HAL_TIMER3_16_TC_STOP    0x00  // No clock, timer stopped
#define HAL_TIMER3_16_TC_DIV1    0x01  // No clock pre-scaling
#define HAL_TIMER3_16_TC_DIV8    0x02  // Clock pre-scaled by 8
#define HAL_TIMER3_16_TC_DIV64   0x03  // Clock pre-scaled by 64
#define HAL_TIMER3_16_TC_DIV256  0x04  // Clock pre-scaled by 256
#define HAL_TIMER3_16_TC_DIV1024 0x05  // Clock pre-scaled by 1024
#define HAL_TIMER3_16_TC_EXTFE   0x06  // External clock (T2), falling edge
#define HAL_TIMER3_16_TC_EXTRE   0x07  // External clock (T2), rising edge

/* Timer clock pre-scaler definitions for 8bit timer0 and timer2 */
#define HAL_TIMER0_8_TC_STOP    0x00  // No clock, timer stopped
#define HAL_TIMER0_8_TC_DIV1    0x01  // No clock pre-scaling
#define HAL_TIMER0_8_TC_DIV8    0x02  // Clock pre-scaled by 8
#define HAL_TIMER0_8_TC_DIV32   0x03  // Clock pre-scaled by 32
#define HAL_TIMER0_8_TC_DIV64   0x04  // Clock pre-scaled by 64
#define HAL_TIMER0_8_TC_DIV128  0x05  // Clock pre-scaled by 128
#define HAL_TIMER0_8_TC_DIV256  0x06  // Clock pre-scaled by 256
#define HAL_TIMER0_8_TC_DIV1024 0x07  // Clock pre-scaled by 1024

/* Timer clock pre-scaler definitions for 8bit timer2 */
#define HAL_TIMER2_8_TC_STOP    0x00  // No clock, timer stopped
#define HAL_TIMER2_8_TC_DIV1    0x01  // No clock pre-scaling
#define HAL_TIMER2_8_TC_DIV8    0x02  // Clock pre-scaled by 8
#define HAL_TIMER2_8_TC_DIV64   0x03  // Clock pre-scaled by 32
#define HAL_TIMER2_8_TC_DIV256  0x04  // Clock pre-scaled by 64
#define HAL_TIMER2_8_TC_DIV1024 0x05  // Clock pre-scaled by 128
#define HAL_TIMER2_8_TC_EXTFE   0x06  // External clock (T2), falling edge
#define HAL_TIMER2_8_TC_EXTRE   0x07  // External clock (T2), rising edge


/***************************************************************************************************
 *                                             TYPEDEFS
 ***************************************************************************************************/
typedef void (*halTimerCBack_t) (uint8 timerId, uint8 channel, uint8 channelMode);

/***************************************************************************************************
 *                                         GLOBAL VARIABLES
 ***************************************************************************************************/


/***************************************************************************************************
 *                                          FUNCTIONS - API
 ***************************************************************************************************/

/*
 * Initialize Timer Service
 */
extern void HalTimerInit ( void );

/*
 * Configure channel in different modes
 */
extern uint8 HalTimerConfig ( uint8 timerId,
                              uint8 opMode,
                              uint8 channel,
                              uint8 channelMode,
                              bool intEnable,
                              halTimerCBack_t cback );

/*
 * Start a Timer
 */
extern uint8 HalTimerStart ( uint8 timerId, uint32 timePerTick );

/*
 * Stop a Timer
 */
extern uint8 HalTimerStop ( uint8 timerId );


/*
 * This is used for polling, provide the tick increment
 */
extern void HalTimerTick ( void );

/*
 * Enable and disable particular timer
 */
extern uint8 HalTimerInterruptEnable (uint8 timerId, uint8 channelMode, bool enable);


/***************************************************************************************************
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif


#ifndef _HAL_TIMERADAPTER_H_6828_
#define _HAL_TIMERADAPTER_H_6828_

/******************************************************************************
 * @author zhangwei on 2006-08-11
 * TiTimerAdapter 
 * This object is the software encapsulation of the MCU's hardware timer.
 * 
 * @modified by zhangwei 2006-09-05
 * @modified by tangwen on 2006-10-21
 * 	- revision
 *
 * @modified by Shanlei on 200611
 *  - tested on arm
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
 * @modified by XieJing in 2009.05
 *	- ported to atmega128 MCU and GAINZ platform.
 * @modified by Jiang Ridong in 2011.07
 *  - Ported to OpenNode 2010 platform (based on stm32f103)
 *
 *****************************************************************************/
 
#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_mcu.h"
#include "hal_common.h"

/**
 * TiTimerAdapter
 * This's an encapsulation of the TIMER hardware in microcontrollers. 
 * 
 * @attention
 * - Since the TIMER hardware varies greatly in different kinds of microcontrollers,
 *   it's pretty difficult to define an general interface for them all. However, 
 *   We strongly suggest the TiTimerAdapter object should implement TiBasicTimerInterface
 *   and TiLightTimerInterface. For some systems, can implement TiPowerTimerInterface.
 * - Though the interface functions are identical for different timer hardware, 
 *   the input parameter value may still varies. For example, the setinterval()
 *   function. Same value may lead to different results. And the difference between 
 *   each timer (8bit, 16bit, 32bit and frequency, etc) also lead to different 
 *   valid parameter ranges.
 */

/* This macro defines how many clocks the Timer run in one milli-seconds.
 * since we often use milli-seconds when dealing with scheduling and communication
 * problems, this macro is quite good for such conversion.
 *
 * @attention
 *	though atmega128 is a 8 bit MCU, we still use uint16 as tm_value_t. 
 */
#ifdef CONFIG_TARGETBOARD_GAINZ
  #define tm_value_t uint16
  #define TIMER_CLOCKS_PER_MILLISECOND  (GAINZ_CLOCK_FREQUENCY / 1000)
  #define timer_msecof(clocks) (clocks/TIMER_CLOCKS_PER_MILLISECOND)
  #define timer_clocksof(ms) (ms*TIMER_CLOCKS_PER_MILLISECOND)
#else
  #define tm_value_t uint32
  #define TIMER_CLOCKS_PER_MILLISECOND  (Fpclk / 1000)
  #define timer_msecof(clocks) (clocks/TIMER_CLOCKS_PER_MILLISECOND)
  #define timer_clocksof(ms) (ms*TIMER_CLOCKS_PER_MILLISECOND)
#endif

/******************************************************************************
 * TiTimerAdapter Object
 *  id		    timer hardware id. usually it is 0, 1, 2. it depends on hardware
 *  channel	    each sub-timer in timer hardware. usually 0-3
 *	state	    bit 7 - bit 0
 *			    bit 0: enabled or not
 *			    bit 1: period trigger or not
 *			    bit 2: raise interrupt or not
 *  listener    this's a function pointer. it will be called when the timer expired
 *              and the interrupt enabled.
 *	object	    used as the parameter for the timer callback listener function
 *  priority	the interrupt priority level. 0 is the highest and 15 is the lowest.
 *****************************************************************************/
 
#define TIMER_STATE_ENABLED 	0x01
#define TIMER_STATE_PERIOD	 	0x02
#define TIMER_STATE_INTERRUPT 	0x04
#define TIMER_STATE_CAPTURE 	0x08

#pragma pack(1) 
typedef struct{
  uint8 id;
  uint8 channel;
  uint8 subpriority;
  uint8 state;
  uint16 interval;
  uint16 count;
  uint16 prescale_factor;
  uint8 repeat;
  TiFunEventHandler listener;
  void * listenowner;//object 
  uint8 priority;
  tm_value_t starttime;
  uint8 option;//1->使能定时中断，0->不使能
}TiTimerAdapter;

#ifdef __cplusplus
extern "C" {   
#endif

/**
 * Construct a TiTimerAdapter object in the memory. This function should always be 
 * success. 
 */
TiTimerAdapter* timer_construct( char * buf, uint8 size );

/**
 * Destroy the timer and revoke allocated resources for this timer before.
 * 
 * @attention This function assumes the timer object has already been stopped before.
 * You should call timer_stop() first before calling this function.
 */
void timer_destroy( TiTimerAdapter * timer );

//若以查询方式判断中断时间是否到，则option必须为0.尤其作为aloha等的时钟使用时更要注意！
/**
 * Initialize the timer adapter object (TiTimerAdapter) for further operation. 
 * 
 * @attention You should still manually start the timer by call timer_start() or 
 *  	timer_restart().
 *
 * @param id Start from 0. This id is used to distinguish different hardware timers.
 * @param listener This is a call back listener be called when the timer expired.
 *		It can be NULL.
 * @param object The owner of the listener function. 
 * @param option[bit7...bit0]
 *      bit0 == 0 interrupt driven bit (default)(中断驱动模式)
 *      bit0 == 1, query driven (查询驱动模式)
 */
TiTimerAdapter* timer_open( TiTimerAdapter * timer, uint8 id, TiFunEventHandler listener, void * object, uint8 option );


void timer_close( TiTimerAdapter * timer );

/**
 * Set the timing interval. The timer will set expired flag when the timing interval
 * elapsed after the timer is started.
 *
 * @attentioin The input value of "interval" parameter is important! It's hardware
 * dependent. It should NOT be too small to enable the application have enough time 
 * to finish the execution of the ISR. And it should NOT be too large so that no 
 * calculation overflow occured! 
 * 
 * @attention The GAINZ hardware node adopts the Atmega128 microcontroller. The valid
 *	input range for "interval" paramete is 1~8.
 *
 * @param timer TiTimerAdapter object
 * @param interval Determins the timing duration when the timer expires from timer_start() call. 
 *		It's based on milliseconds.
 * @param repeat Decides whether the timer should work periodically.
 *      FALSE/0  trigger only once (default)
 *  	TRUE/1   period triggering
 *
 * @return Nothing happens even if failed.
 *
 * @warning
 *	Be careful with your input interval range! The automatic tuning inside this function
 * may generate wrong results inside. 
 * 
 * @todo repeat isn't implemented yet!!!!
 */

//repeat:1->use interrupt 0->not use interrupt.
void timer_setinterval( TiTimerAdapter * timer, uint16 interval, uint8 repeat );

void timer_setscale( TiTimerAdapter * timer, uint16 prescaler );

/**
 * Start the timer according to the interval, scale and repeat settings. 
 * If the timer interrupt is enabled, then the callback listener function will be 
 * called automatically if listener isn't NULL.
 */
void timer_start( TiTimerAdapter * timer );

/** 
 * Restart the timer. This function is the combination of timer_setinterval() and 
 * timer_start() function.
 * 
 * @atttention: If the timer is still running, then this function will discard the 
 * current execution and start the timing from the timer point this function is called.
 * 
 * @param timer TiTimerAdapter object
 * @param interval Timing interval.
 * @param repeat 0 means this's a one time only timing. 1 means the timer will restart
 * 		after each expiration. 
 */
void timer_restart( TiTimerAdapter * timer);
// void timer_restart( TiTimerAdapter * timer, uint16 interval, uint8 repeat );

void timer_stop( TiTimerAdapter * timer );

/**
 * Enable the interrupt. The timer hardware can generate interrupt request to drive
 * the timer object to run. 
 * 
 * This funtion is only meaningful when the timer object runs in interrupt driven 
 * mode. For query driven mode, this function doesn nothing.
 * 
 * @attention If the timer is disabled, the timer hardware can still run. But you 
 * 	should check the timer status frequently to get whether the timer is expired
 *  or not. 
 * 
 * Q: What's the different between timer_enable() and timer_start()?
 * R: timer_enable() affect interrupt setting, while timer_start() affect the running 
 * or not of the hardware. The default setting is timer enabled. 
 */
void timer_enable( TiTimerAdapter * timer );
void timer_disable( TiTimerAdapter * timer );

void timer_setlistener( TiTimerAdapter * timer, TiFunEventHandler listener, void * object );

// void       timer_setchannel( TiTimerAdapter * timer, uint8 channel );

void timer_setprior( TiTimerAdapter * timer, uint8 preprior,uint8 subprior );
//void timer_setprior( TiTimerAdapter * timer, uint8 prior );

//void       timer_setvalue( TiTimerAdapter * timer, tm_value_t value );
//tm_value_t timer_getvalue( TiTimerAdapter * timer );
//tm_value_t timer_elapsed( TiTimerAdapter * timer );

uint8 timer_expired( TiTimerAdapter *timer);//1 time interrupt occur,0 not occur.
//bool       timer_expired( TiTimerAdapter * timer );


void timer_CLR_IT( TiTimerAdapter *timer);//clear the interrupt bit


tm_value_t timer_clocksperms( TiTimerAdapter * timer );


/**
 * Get the basic timer interface
 * @return An pointer to the basic timer interface.
 */
TiBasicTimerInterface * timer_basicinterface( TiTimerAdapter * timer, TiBasicTimerInterface * intf );

TiLightTimerInterface * timer_lightinterface( TiTimerAdapter * timer, TiLightTimerInterface * intf );

TiPowerTimerInterface * timer_powerinterface( TiTimerAdapter * timer, TiPowerTimerInterface * intf );


#ifdef __cplusplus
}
#endif

#endif /* _HAL_TIMERADAPTER_H_6828_ */

