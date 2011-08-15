

#include "../hal_foundation.h"
//#include <intrins.h>
#include "../hal_cpu.h"
#include "../hal_mcu.h"
#include "cm3/device/stm32f10x/stm32f10x.h"

GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;
NVIC_InitTypeDef NVIC_InitStruct;
EXTI_InitTypeDef EXTI_InitStructure;  
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

static void _RCC_Configuration(void);
static void _GPIO_Configuration(void);
static void _NVIC_Configuration(void);

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
void mcu_init(void)
{
  /* Configure the system clocks */
  _RCC_Configuration();
    
  /* GPIO Configuration */
  _GPIO_Configuration();

  /* NVIC Configuration */
  _NVIC_Configuration();
}

void _RCC_Configuration(void)
{/*todo for testing
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    RCC_PCLK1Config(RCC_HCLK_Div2);
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)	;
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while(RCC_GetSYSCLKSource() != 0x08);
  }
   
  * Enable GPIOA|B*
  *todo for testing
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);

  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  * TIM1 clock enable *
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  * TIM2 clock enable *
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  * ADC1 clock enable *
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  * GPIOA, GPIOB and SPI1 clock enable *
    *todo for testing
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  * SPI2 Periph clock enable *
    *todo for testing
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  todo for testing*/
}

/*******************************************************************************
* Function Name  : _GPIO_Configuration
* Description    : Configures GPIO Modes
*******************************************************************************/
void _GPIO_Configuration()
{
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : _NVIC_Configuration
* Description    : Configures NVIC Modes for T1
*******************************************************************************/
void _NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  //NVIC_SetVectorTableNVIC_SetVectorTableNVIC_SetVectorTableNVIC_SetVectorTableNVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

  /* Configure the NVIC Preemption Priority Bits[配置优先级组] */  
 // todo
 // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* Enable the TIM1 gloabal Interrupt [允许TIM1全局中断]*/
/*  
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
*/  

  /* Enable the TIM2 gloabal Interrupt [允许TIM2全局中断]*/
/*
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
*/
  /* Enable the RTC Interrupt */
/*
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
*/
  /* Configure INT IO  PC9 enable exti9_5*/
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);  
  EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
/*
  NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQChannel;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStruct);
*/
  /* Configure INT IO  PE4 enable exti4*/
/*
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);  
  EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQChannel;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStruct);
*/
  /* Configure INT IO  PE5 enable exti9_5*/
/*
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5);  
  EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQChannel;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStruct);
*/
}
  
/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
*******************************************************************************/



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
/*void halMcuSetLowPowerMode(uint8 mode)
{
 todo
    switch (mode)
    {
    case HAL_MCU_LPM_0:
        __low_power_mode_0();
        break;
    case HAL_MCU_LPM_1:
        __low_power_mode_1();
        break;
    case HAL_MCU_LPM_2:
        __low_power_mode_2();
        break;
    case HAL_MCU_LPM_3:
        __low_power_mode_3();
        break;
    case HAL_MCU_LPM_4:
        __low_power_mode_4();
        break;
    }
}
*/

