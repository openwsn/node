/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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
 * @modified by Yan Shixing
 		add #define FAIL 1
 ******************************************************************************/

#ifndef _HPL_ATMEGA128_H_4282_
#define _HPL_ATMEGA128_H_4282_

/* Hardware Platform Layer
 * including CPU/MCU specific source codes
 * 
 * Reference 
 * - Hardware Abstraction Architecture, http://www.tinyos.net/tinyos-2.x/doc/html/tep2.html
 * - AVR Libc 1.6.5 Online Reference, http://www.nongnu.org/avr-libc/user-manual/index.html
 */ 

//#include "hal_configall.h"
//#include <stdint.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

/* attention: actually you should not include <iom128.h> directly because the WinAVR/AVR Studio
 * should help you to do so. but i don't know why i cann found the correct register 
 * definitions in some projects, so i had to include the header file for atmega128 MCU
 * manually here. 
 */ 
//#include <avr/iom128.h>


#ifndef __GNUC__
#error "this file is compiled by WinAVR GNU C compiler by default. other compilers havn't been tested yet!"
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* check for version 3.3 of GNU gcc or later */

#if ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 3))
  #define __outw(val, port) outw(port, val);
#endif

#ifndef __inw
  #ifndef __SFR_OFFSET
    #define __SFR_OFFSET 0
  #endif /* !__SFR_OFFSET */
  #define __inw(_port) inw(_port)

  #define __inw_atomic(__sfrport) ({	\
	uint16_t __t;						\
	bool bStatus;						\
	bStatus = bit_is_set(SREG,7);		\
	cli();								\
	__t = inw(__sfrport);				\
	if (bStatus) sei();					\
	__t;								\
 })
#endif /* __inw */


#define FAIL 1

/* macros help to set the MCU's PIN directions, state, read and write */

#define HAL_ASSIGN_PIN_DECLARE(name, port, bit)								\
    void HAL_SET_##name##_PIN(void);										\
    void HAL_CLR_##name##_PIN(void);										\
    int  HAL_READ_##name##_PIN(void);										\
    void HAL_MAKE_##name##_OUTPUT(void);									\
    void HAL_MAKE_##name##_INPUT(void);

#define HAL_ASSIGN_PIN(name, port, bit)										\
    void HAL_SET_##name##_PIN() {PORT##port |= (1<<(bit));}					\
    void HAL_CLR_##name##_PIN() {PORT##port &= (~(1<<(bit)));}				\
    int  HAL_READ_##name##_PIN()	{return (PIN##port & (1 << bit)) != 0;} \
    void HAL_MAKE_##name##_OUTPUT() {DDR##port |= (1<<(bit));}				\
    void HAL_MAKE_##name##_INPUT() {DDR##port &= (~(1<<(bit)));} 


/* reference: utility functions in \portable\WinAVR-20080610\avr\include\avr\power.h 
 * to power on/off a specified component */

#define hal_set_power_mode(mode) \
	if (mode==0) power_all_disable(); else power_all_enable();



//---------------------
#define HAL_ASSIGN_PIN_list(name, port, bit) \
    void HAL_SET_##name##_PIN(void);  \
    void HAL_CLR_##name##_PIN(void);  \
    int    HAL_READ_##name##_PIN(void);\
    void HAL_MAKE_##name##_OUTPUT(void);  \
    void HAL_MAKE_##name##_INPUT(void) ;
//--------------------------------------------------
#define HAL_ASSIGN_PIN(name, port, bit) \
    void HAL_SET_##name##_PIN() {PORT##port |= (1<<(bit));} \
    void HAL_CLR_##name##_PIN() {PORT##port &= (~(1<<(bit)));} \
    int HAL_READ_##name##_PIN()							\
  {return (PIN##port & (1 << bit)) != 0;}								\
    void HAL_MAKE_##name##_OUTPUT() {DDR##port |= (1<<(bit));} \
    void HAL_MAKE_##name##_INPUT() {DDR##port &= (~(1<<(bit)));} 



//--------------------------------------------------------------------------
	
// LED assignments
HAL_ASSIGN_PIN_list(RED_LED, A, 2);
HAL_ASSIGN_PIN_list(GREEN_LED, A, 1);
HAL_ASSIGN_PIN_list(YELLOW_LED, A, 0);

HAL_ASSIGN_PIN_list(SERIAL_ID, A, 4);
HAL_ASSIGN_PIN_list(BAT_MON, A, 5);
HAL_ASSIGN_PIN_list(THERM_PWR, A, 7);


// ChipCon control assignments

#define HAL_CC_FIFOP_INT SIG_INTERRUPT6

HAL_ASSIGN_PIN_list(CC_RSTN, A, 6); // chipcon reset
HAL_ASSIGN_PIN_list(CC_VREN, A, 5); // chipcon power enable
//HAL_ASSIGN_PIN_list(CC_FIFOP1, D, 7);  // fifo interrupt
HAL_ASSIGN_PIN_list(CC_FIFOP, E, 6);  // fifo interrupt

HAL_ASSIGN_PIN_list(CC_CCA, D, 6);	  // 
HAL_ASSIGN_PIN_list(CC_SFD, D, 4);	  // chipcon packet arrival
HAL_ASSIGN_PIN_list(CC_CS, B, 0);	  // chipcon enable
HAL_ASSIGN_PIN_list(CC_FIFO, B, 7);	  // chipcon fifo

HAL_ASSIGN_PIN_list(RADIO_CCA, D, 6);	  // 

// Flash assignments
HAL_ASSIGN_PIN_list(FLASH_SELECT, A, 3);
HAL_ASSIGN_PIN_list(FLASH_CLK,  D, 5);
HAL_ASSIGN_PIN_list(FLASH_OUT,  D, 3);
HAL_ASSIGN_PIN_list(FLASH_IN,  D, 2);

// interrupt assignments
HAL_ASSIGN_PIN_list(INT0, E, 4);
HAL_ASSIGN_PIN_list(INT1, E, 5);
HAL_ASSIGN_PIN_list(INT2, E, 6);
HAL_ASSIGN_PIN_list(INT3, E, 7);

// spibus assignments 
HAL_ASSIGN_PIN_list(MOSI,  B, 2);
HAL_ASSIGN_PIN_list(MISO,  B, 3);
//HAL_ASSIGN_PIN_list(SPI_OC1C, B, 7);
HAL_ASSIGN_PIN_list(SPI_SCK,  B, 1);

// power control assignments
HAL_ASSIGN_PIN_list(PW0, C, 0);
HAL_ASSIGN_PIN_list(PW1, C, 1);
HAL_ASSIGN_PIN_list(PW2, C, 2);
HAL_ASSIGN_PIN_list(PW3, C, 3);
HAL_ASSIGN_PIN_list(PW4, C, 4);
HAL_ASSIGN_PIN_list(PW5, C, 5);
HAL_ASSIGN_PIN_list(PW6, C, 6);
HAL_ASSIGN_PIN_list(PW7, C, 7);

// i2c bus assignments
HAL_ASSIGN_PIN_list(I2C_BUS1_SCL, D, 0);
HAL_ASSIGN_PIN_list(I2C_BUS1_SDA, D, 1);

// uart assignments
HAL_ASSIGN_PIN_list(UART_RXD0, E, 0);
HAL_ASSIGN_PIN_list(UART_TXD0, E, 1);
HAL_ASSIGN_PIN_list(UART_XCK0, E, 2);
	
HAL_ASSIGN_PIN_list(UART_RXD1, D, 2);
HAL_ASSIGN_PIN_list(UART_TXD1, D, 3);
HAL_ASSIGN_PIN_list(UART_XCK1, D, 5);





int is_host_lsb(void);

uint16_t toLSB16(uint16_t a);
uint16_t fromLSB16(uint16_t a);

uint8_t  rcombine(uint8_t  r1, uint8_t  r2);
uint8_t rcombine4(uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4);
 
void HAL_SET_PIN_DIRECTIONS(void);

//---------------------------------------------------------------------------
/*


#define HAL_CYCLE_TIME_NS 136

// each nop is 1 clock cycle
// 1 clock cycle on mica2 == 136ns
*/

#ifdef __cplusplus
}
#endif

#endif
