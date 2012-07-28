/*
 * @section history
 * - modified by zhangwei on 2011.09.12
 *   - include system_stm32f10x.h to enable the SystemInit() function.
 */

#include "../hal_foundation.h"
#include <intrinsics.h>
#include "../hal_cpu.h"
#include "../hal_mcu.h"
#include "./components/targets/interface/hal_mcu_ti.h"
#include "./components/radios/cc2430/clock.h"

/**
 * Initialize the microcontroller. After initialization, the MCU should be able to 
 * run some little programs. For external device initialization other than the MCU,
 * target_init() should be called. 
 * 
 * Generally you needn't call mcu_init() because the target_init() will call it.
 *
 * @section For OpenNode 2010 hardware platform only:
 * For OpenNode 2010 hardware platform, STM32F10x MCU is used. The mcu_init() function
 * will configure the RCC, GPIO and NVIC which related to the MCU only.
 */

/***********************************************************************************
* @fn          mcu_init
*
* @brief       Set system clock
*
* @param       none
*
* @return      none
*/
void mcu_init(void)
{
    clockSetMainSrc(CLOCK_SRC_XOSC);
}


/***********************************************************************************
* @fn          halMcuSetLowPowerMode
*
* @brief      Sets the MCU in a low power mode. Will turn global interrupts on at
*             the same time as entering the LPM mode. The MCU must be waken from
*             an interrupt (status register on stack must be modified).
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint8 mode - power mode
*
* @return      none
*/
/*
void halMcuSetLowPowerMode(uint8 mode)
{
  // comment: not yet implemented
  //HAL_ASSERT(FALSE);
}
*/

/******************************************************************************
* @fn  halMcuReset
*
* @brief
* Resets the MCU. This utilize the watchdog timer as there is no other way
* for a software reset. The reset will not occur until ~2 ms.
* NB: The function will not return! (hangs until reset)
*
* Parameters:
*
* @param  void
*
* @return void
*
******************************************************************************/
/*
void halMcuReset(void)
{
    const uint8 WDT_INTERVAL_MSEC_2=   0x03;   // after ~2 ms

    WDCTL = ((WDCTL & 0xFC) | (WDT_INTERVAL_MSEC_2 & 0x03));
    // Start watchdog
    WDCTL &= ~0x04;     // Select watchdog mode
    WDCTL |= 0x08;      // Enable timer
    while(1);                                   // Halt here until reset
}


*/
