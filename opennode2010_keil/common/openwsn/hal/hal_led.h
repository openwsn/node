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

#ifndef _HAL_LED_H_1329_
#define _HAL_LED_H_1329_

/*******************************************************************************
 * @author zw (tongji university) on 20051001
 *	- first created
 * @modified by shi-miaojing (tongji university) in 200907
 *	- ported to ICT GAINZ hardware platform. GAINZ is using atmega128L MCU.
 * @modified by zhangwei (tongji university) in 200907
 *	- tested ok. this module is self-contained except the led_twinkle() uses
 *    the hal_cpu module.
 ****************************************************************************/ 
	
#include "hal_configall.h"
#include "hal_foundation.h"

#define LED1        0x01
#define LED2        0x02
#define LED3        0x04
#define LED4		0x08
#define LED_RED		LED1
#define LED_GREEN 	LED2
#define LED_YELLOW 	LED3
#define LED_BLUE	LED4

#define LED_ALL     (LED1 + LED2 + LED3 + LED4)

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * the parameter "id" should be the macro defined above
 *	id	= { LED_GREEN, LED_YELLOW, LED_RED } 
 *
 * @attention: you must call led_open() before you call other LED functions
 * @attention: you should NOT use "led_twinkle()" in interrupts. because it may 
 *	lead to quite a long delay.
 ****************************************************************************/ 

void led_open( uint16 id );
void led_close( void );
void led( uint16 id, bool state );

/** 
 * Turn the specific LED on 
 * @param id The LED id, usually represented by the macro LED1~LED3 or LED_RED ~ LED_YELLOW.
 *		id	= { LED_GREEN, LED_YELLOW, LED_RED } 
 * @return None
 */
void led_on( uint16 id );

/** 
 * Turn the specific LED off
 * @param id The LED id, usually represented by the macro LED1~LED3 or LED_RED ~ LED_YELLOW.
 *		id	= { LED_GREEN, LED_YELLOW, LED_RED } 
 * @return None
 */
void led_off( uint16 id );
void led_toggle( uint16 id );
void led_twinkle( uint16 id, uint16 interval, uintx count );
void led_show( uint16 state );

 
#ifdef __cplusplus
}
#endif
   
#endif
