#ifndef _INIT_H_5678_
#define _INIT_H_5678_

#include "../configall.h"

#include <stdio.h>

#include "./stm32/stm32f10x_conf.h"
#include "./stm32/stm32f10x_lib.h"
#include "./stm32/stm32f10x_type.h"
#include "./stm32/stm32f10x_it.h"
#include "./stm32/stm32f10x_gpio.h"
#include "./stm32/stm32f10x_nvic.h"
#include "./stm32/stm32f10x_exti.h"
#include "./stm32/stm32f10x_tim.h"
#include "./stm32/stm32f10x_usart.h"
#include "./stm32/stm32f10x_wwdg.h"
#include "./stm32/stm32f10x_rcc.h"

extern GPIO_InitTypeDef GPIO_InitStructure;
extern ErrorStatus HSEStartUpStatus;
extern NVIC_InitTypeDef NVIC_InitStruct;
extern EXTI_InitTypeDef EXTI_InitStructure;  
extern TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
extern TIM_OCInitTypeDef  TIM_OCInitStructure;


/*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library printf function to the USART.
*******************************************************************************/
int fputc(int ch, FILE *f);

/*******************************************************************************
* Function Name  : BSP_Init
* Description    : Configures RCC,GPIO,NVIC
*******************************************************************************/
void BSP_Init(void);

/*******************************************************************************
* Function Name  : delay_nus
* Description    : delay n us
*******************************************************************************/
// 延时n us: n>=6,最小延时单位6us
// 外部晶振：8M；PLL：9；8M*9=72MHz
void delay_nus(unsigned long n);

/*******************************************************************************
* Function Name  : delay_nms
* Description    : delay n ms
*******************************************************************************/
// 延时n ms
// 外部晶振：8M；PLL：9；8M*9=72MHz
void delay_nms(unsigned long n);


#endif /* _INIT_H_5678_ */
