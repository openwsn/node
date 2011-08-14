#ifndef _HAL_MCU_H_7489_
#define _HAL_MCU_H_7489_

#include "hal_configall.h"
#include "hal_foundation.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_TARGETBOARD_OPENNODE2010

//#include <stdio.h>
//#include <stm32f10x_lib.h>
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_foundation.h"
#include "./opennode2010/cm3/device/stm32f10x/stm32f10x.h"
#include "./opennode2010/stm32f10x/std/inc/misc.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_adc.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_bkp.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_can.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_cec.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_dac.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_dbgmcu.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_dma.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_exti.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_flash.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_fsmc.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_gpio.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_i2c.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_iwdg.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_pwr.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_rcc.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_rtc.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_sdio.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_spi.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_tim.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_usart.h"
#include "./opennode2010/stm32f10x/std/inc/stm32f10x_wwdg.h"
//#include "./stm32f10x/std/inc/stm32f10x_nvic.h"

extern GPIO_InitTypeDef GPIO_InitStructure;
extern ErrorStatus HSEStartUpStatus;
//extern NVIC_InitTypeDef NVIC_InitStructure;
extern EXTI_InitTypeDef EXTI_InitStructure;  
extern TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
extern TIM_OCInitTypeDef  TIM_OCInitStructure;
extern SPI_InitTypeDef SPI_InitStructure;
//extern USART_InitTypeDef USART_InitStructure;

/*******************************************************************************
* Description    : Configures RCC,GPIO,NVIC
*******************************************************************************/
void mcu_init(void);


#endif /* CONFIG_TARGETBOARD_OPENNODE2010 */



/***********************************************************************************
* INCLUDES
*/
//#include "hal_types.h"


/***********************************************************************************
 * CONSTANTS AND DEFINES
 */
/* 
enum {                  // Input values to halMcuSetLowPowerMode()
    HAL_MCU_LPM_0,
    HAL_MCU_LPM_1,
    HAL_MCU_LPM_2,
    HAL_MCU_LPM_3,
    HAL_MCU_LPM_4
};


enum {                  // Return values of halMcuGetResetCause()
    HAL_MCU_RESET_CAUSE_POR,
    HAL_MCU_RESET_CAUSE_EXTERNAL,
    HAL_MCU_RESET_CAUSE_WATCHDOG
};
*/

/***********************************************************************************
 * GLOBAL FUNCTIONS
 */

/*
void halMcuInit(void);
void halMcuWaitUs(uint16 usec);
void halMcuWaitMs(uint16 msec);
void halMcuSetLowPowerMode(uint8 mode);
void halMcuReset(void);
uint8 halMcuGetResetCause(void);
*/

#ifdef  __cplusplus
}
#endif

/**********************************************************************************/
#endif /* _HAL_MCU_H_7489_ */
