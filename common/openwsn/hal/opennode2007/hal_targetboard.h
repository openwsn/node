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
#ifndef _HAL_TARGET_H_3792_
#define _HAL_TARGET_H_3792_

/*****************************************************************************
 * @author zhangwei on 2006-07-20
 * Target utilities
 * 通常，target.*模块中包含目标硬件系统的配置，这些配置通常是电路板系统级的连接，
 * 它们放入MCU模块中并不合适，因为这些信息并不与特定的MCU有关。target中的代码
 * 很类似于许多RTOS中提到的BSP(board support package)概念。
 *
 * @modified by zhangwei on 20061013
 * first created
 * move the old PORT_INIT() and UART0_INIT() here.
 *
 ****************************************************************************/

#include "hal_foundation.h"
#include "hal_targetboard_more.h"

#ifdef CONFIG_TARGET_DEFAULT
#undef CONFIG_TARGET_DEFAULT
#endif

#if ((!defined(CONFIG_TARGET_OPENNODE_10)) && (!defined(CONFIG_TARGET_OPENNODE_20)) \
  	&& (!defined(CONFIG_TARGET_OPENNODE_30)) && (!defined(CONFIG_TARGET_WLSMODEM_11)))
#define CONFIG_TARGET_DEFAULT
#endif

/*****************************************************************************
 * LED Target Settings
 ****************************************************************************/

#ifdef CONFIG_TARGET_OPENNODE_10
#define LED_GREEN_PIN 	25
#define LED_YELLOW_PIN 	21
#define LED_RED_PIN	21
#define LED_GREEN_PORT 	1
#define LED_YELLOW_PORT 0
#define LED_RED_PORT	0
#endif

#ifdef CONFIG_TARGET_OPENNODE_20
#define LED_GREEN_PIN 	25
#define LED_YELLOW_PIN 	18
#define LED_RED_PIN	16
#define LED_GREEN_PORT 	0
#define LED_YELLOW_PORT 1
#define LED_RED_PORT	1
#endif

#ifdef CONFIG_TARGET_OPENNODE_30
#define LED_GREEN_PIN 	25
#define LED_YELLOW_PIN 	18
#define LED_RED_PIN	16
#define LED_GREEN_PORT 	1
#define LED_YELLOW_PORT 1
#define LED_RED_PORT	1
#endif

#ifdef CONFIG_TARGET_WLSMODEM_11
#define LED_GREEN_PIN 	19
#define LED_YELLOW_PIN 	19
#define LED_RED_PIN	18
#define LED_GREEN_PORT 	0
#define LED_YELLOW_PORT 0
#define LED_RED_PORT	0
#endif

#ifdef CONFIG_TARGET_DEFAULT
#define LED_GREEN_PIN 	25
#define LED_YELLOW_PIN 	18
#define LED_RED_PIN	16
#define LED_GREEN_PORT 	1
#define LED_YELLOW_PORT 1
#define LED_RED_PORT	1
#endif


/*****************************************************************************
 * Cc2420 Transceiver Target Settings
 ****************************************************************************/

#ifdef CONFIG_TARGET_OPENNODE_10
#define FIFO            8  // P0.8  - Input: FIFO from CC2420
#define FIFOP           9  // P0.9  - Input: FIFOP from CC2420
#define CCA            10  // p0.10 - Input:  CCA from CC2420
#define RESET_N        12  // P0.12 - Output: RESET_N to CC2420
#define VREG_EN        13  // P0.13 - Output: VREG_EN to CC2420
#define SFD            16  // P0.16 - Input:  SFD from CC2420
#define CSN            21  // P1.21 - Output: SPI Chip Select (CS_N)

#define FIFO_PORT      0
#define FIFOP_PORT     0
#define CCA_PORT       0
#define RESET_N_PORT   0
#define VREG_EN_PORT   0
#define SFD_PORT       0
#define CSN_PORT       1
#endif

#ifdef CONFIG_TARGET_OPENNODE_20
#define FIFO           22
#define FIFOP          15
#define CCA            13
#define RESET_N        12
#define VREG_EN        10
#define SFD            16
#define CSN            21

#define FIFO_PORT      1
#define FIFOP_PORT     0
#define CCA_PORT       0
#define RESET_N_PORT   0
#define VREG_EN_PORT   0
#define SFD_PORT       0
#define CSN_PORT       1
#endif

#ifdef CONFIG_TARGET_OPENNODE_30
#define FIFO           22  	// P1.22 input from 2420 to ARM
#define FIFOP          15  	// P0.15 input from 2420 to ARM as interrupt request
#define CCA            17  	// P1.17 input from 2420 to ARM as channel indication  // @TODO is port correct?
#define RESET_N        23 	// P1.23 output from ARM to cc2420.
#define VREG_EN        19  	// P1.19 output from ARM to cc2420
#define SFD            16 	// P0.16 input from 2420 to ARM to indicate the frame's arrival
#define CSN            21  	// P1.21 output from 2420 to cc2420 as SPI select (chip select)

#define FIFO_PORT      1
#define FIFOP_PORT     0
#define CCA_PORT       1
#define RESET_N_PORT   1
#define VREG_EN_PORT   1
#define SFD_PORT       0
#define CSN_PORT       1
#endif

#ifdef CONFIG_TARGET_WLSMODEM_11
#define FIFO           16  // P0.16  - Input: FIFO from CC2420
#define FIFOP          15  // P0.15  - Input: FIFOP from CC2420
#define CCA            12  // p0.12 - Input:  CCA from CC2420
#define RESET_N        23  // P1.23 - Output: RESET_N to CC2420
#define VREG_EN        10  // P0.10 - Output: VREG_EN to CC2420
#define SFD            11  // P0.11 - Input:  SFD from CC2420
#define CSN            17  // P0.17 - Output: SPI Chip Select (CS_N)

#define FIFO_PORT      0
#define FIFOP_PORT     0
#define CCA_PORT       0
#define RESET_N_PORT   1
#define VREG_EN_PORT   0
#define SFD_PORT       0
#define CSN_PORT       0
#endif

#ifdef CONFIG_TARGET_DEFAULT
#define FIFO           22  	// P1.22 input from 2420 to ARM
#define FIFOP          15  	// P0.15 input from 2420 to ARM as interrupt request
#define CCA            17  	// P1.17 input from 2420 to ARM as channel indication  // @TODO is port correct?
#define RESET_N        23 	// P1.23 output from ARM to cc2420.
#define VREG_EN        19  	// P1.19 output from ARM to cc2420
#define SFD            16 	// P0.16 input from 2420 to ARM to indicate the frame's arrival
#define CSN            21  	// P1.21 output from 2420 to cc2420 as SPI select (chip select)

#define FIFO_PORT      1
#define FIFOP_PORT     0
#define CCA_PORT       1
#define RESET_N_PORT   1
#define VREG_EN_PORT   1
#define SFD_PORT       0
#define CSN_PORT       1
#endif

/*****************************************************************************
 * initialize the target hardware
 * this function MUST run successfully. it's usually the first function called in your application.
 *
 * target_init() is different to hal_target_reset_init()/hal_target_reset().
 * the later one is called automatically when the target device reset or power up.
 * it is called before the boot loader call main() function. while target_init()
 * called in the application layer's main() function.
 ****************************************************************************/
void target_init( void );

#endif
