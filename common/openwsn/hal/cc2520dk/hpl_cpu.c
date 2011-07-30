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

#include "hal_cpu.h"
/*
inline cpu_atomic_t _cpu_atomic_start(void )
{
	cpu_atomic_t result = * (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x3F + 0x20);
	__asm volatile ("cli");
	return result;
}
*/


/***********************************************************************************
* @fn          halMcuWaitUs
*
* @brief       Busy wait function. Waits the specified number of microseconds. Use
*              assumptions about number of clock cycles needed for the various
*              instructions. The duration of one cycle depends on MCLK. In this HAL
*              , it is set to 8 MHz, thus 8 cycles per usec.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 usec - number of microseconds delay
*
* @return      none
*/

/* delay for usec.
 * this function is hardware dependent and should used for very short delays only. 
 * if you need accurate long delays, please use timer to help you do so.
 * 
 * @attention
 * the input parameter value should not large than the maximum value of int16 type
 */
void cpu_delayus(uint16 usec) // 5 cycles for calling
{
    // The least we can wait is 3 usec:
    // ~1 usec for call, 1 for first compare and 1 for return
    while(usec > 3)       // 2 cycles for compare
    {                // 2 cycles for jump
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        NOP();       // 1 cycles for nop
        usec -= 2;        // 1 cycles for optimized decrement
    }
}                         // 4 cycles for returning


/***********************************************************************************
* @fn          halMcuWaitMs
*
* @brief       Busy wait function. Waits the specified number of milliseconds. Use
*              assumptions about number of clock cycles needed for the various
*              instructions.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 millisec - number of milliseconds delay
*
* @return      none
*/

/* delay for micro seconds.
 * this function is hardware dependent and should used for short delays only. 
 * if you need very long accurate delays, please use timer to help you do so.
 * 
 * attention: 
 * the input msec value should not exceed the maximum value of uint16 type 
 */
#pragma optimize=none
inline void cpu_delay(uint16 msec) 
{
    while(msec-- > 0)
    {
        halMcuWaitUs(1000);
    }
}


/***********************************************************************************
* @fn          halMcuReset
*
* @brief       MCU soft reset.
*
* @param       none
*
* @return      none
*/
void cpu_reset(void)
{
    void (*pf)(void)= 0;
    (*pf)();
}


void cpu_reboot( void )
{
    // asm: jump to start address
}



   
