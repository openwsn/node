#ifndef _HAL_MCU_H_7489_
#define _HAL_MCU_H_7489_

#include "hal_configall.h"
#include "hal_foundation.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_TARGETBOARD_OPENNODE2010

#include <stdio.h>
//#include <stm32f10x_lib.h>
#include "./cm3/device/stm32f10x/stm32f10x.h"
#include "./stm32f10x/std/inc/stm32f10x_foundation.h"
#include "./stm32f10x/std/inc/misc.h"
#include "./stm32f10x/std/inc/stm32f10x_adc.h"
#include "./stm32f10x/std/inc/stm32f10x_bkp.h"
#include "./stm32f10x/std/inc/stm32f10x_can.h"
#include "./stm32f10x/std/inc/stm32f10x_cec.h"
#include "./stm32f10x/std/inc/stm32f10x_dac.h"
#include "./stm32f10x/std/inc/stm32f10x_dbgmcu.h"
#include "./stm32f10x/std/inc/stm32f10x_dma.h"
#include "./stm32f10x/std/inc/stm32f10x_exti.h"
#include "./stm32f10x/std/inc/stm32f10x_flash.h"
#include "./stm32f10x/std/inc/stm32f10x_fsmc.h"
#include "./stm32f10x/std/inc/stm32f10x_gpio.h"
#include "./stm32f10x/std/inc/stm32f10x_i2c.h"
#include "./stm32f10x/std/inc/stm32f10x_iwdg.h"
#include "./stm32f10x/std/inc/stm32f10x_pwr.h"
#include "./stm32f10x/std/inc/stm32f10x_rcc.h"
#include "./stm32f10x/std/inc/stm32f10x_rtc.h"
#include "./stm32f10x/std/inc/stm32f10x_sdio.h"
#include "./stm32f10x/std/inc/stm32f10x_spi.h"
#include "./stm32f10x/std/inc/stm32f10x_tim.h"
#include "./stm32f10x/std/inc/stm32f10x_usart.h"
#include "./stm32f10x/std/inc/stm32f10x_wwdg.h"
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
* Function Name  : fputc#include "common\devx_configall.h"
#include "common\devx_foundation.h"
#include "common\rtl\rtl_frame.h"
#include "common\rtl\rtl_debugio.h"
#include "common\rtl\rtl_ieee802frame154.h"
#include "common\rtl\rtl_dumpframe.h"

* Description    : Retargets the C library printf function to the USART.
*******************************************************************************/
int fputc(int ch, FILE *f);

/*******************************************************************************
* Function Name  : BSP_Init
* Description    : Configures RCC,GPIO,NVIC
*******************************************************************************/
void mcu_init(void);

/*******************************************************************************
* Function Name  : delay_nus
* Description    : delay n us
*******************************************************************************/

#define hal_delayus(n) delay_nus(n)
// 延时n us: n>=6,最小延时单位6us
// 外部晶振：8M；PLL：9；8M*9=72MHz
void delay_nus(unsigned long n);

/*******************************************************************************
* Function Name  : delay_nms
* Description    : delay n ms
*******************************************************************************/
// 延时n ms
// 外部晶振：8M；PLL：9；8M*9=72MHz

#define hal_delay(n) delay_nms(n)
#define hal_delayms(n) delay_nms(n)
void delay_nms(unsigned long n);
#endif /* CONFIG_TARGETBOARD_OPENNODE2010 */



/***********************************************************************************
* INCLUDES
*/
//#include "hal_types.h"


/***********************************************************************************
 * CONSTANTS AND DEFINES
 */
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


/***********************************************************************************
 * GLOBAL FUNCTIONS
 */

void halMcuInit(void);
void halMcuWaitUs(uint16 usec);
void halMcuWaitMs(uint16 msec);
void halMcuSetLowPowerMode(uint8 mode);
void halMcuReset(void);
uint8 halMcuGetResetCause(void);


#ifdef  __cplusplus
}
#endif

/**********************************************************************************/
#endif /* _HAL_MCU_H_7489_ */
