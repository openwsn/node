#include "init.h"

GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;
NVIC_InitTypeDef NVIC_InitStruct;
EXTI_InitTypeDef EXTI_InitStructure;  
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures System Clocks
*******************************************************************************/
void RCC_Configuration(void)
{
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
   
  /* Enable GPIOA|B*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);

  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  /* TIM1 clock enable */
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* TIM2 clock enable */
  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  /* ADC1 clock enable */
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* GPIOA, GPIOB and SPI1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  /* SPI2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures GPIO Modes
*******************************************************************************/
void GPIO_Configuration()
{
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures NVIC Modes for T1
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

  /* Configure the NVIC Preemption Priority Bits[配置优先级组] */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* Enable the TIM1 gloabal Interrupt [允许TIM1全局中断]*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM2 gloabal Interrupt [允许TIM2全局中断]*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure INT IO  PC9 enable exti9_5*/
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9);  
  EXTI_InitStructure.EXTI_Line=EXTI_Line9;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQChannel;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStruct);

  /* Configure INT IO  PE4 enable exti4*/
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

  /* Configure INT IO  PE5 enable exti9_5*/
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
}
  
/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configures the USART1.
*******************************************************************************/


/*******************************************************************************
* Function Name  : fputc
* Description    : Retargets the C library printf function to the USART.
*******************************************************************************/
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (u8) ch);

  /* Loop until the end of transmission */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) ;  // waiting here
  
  return ch;
}

/*******************************************************************************
* Function Name  : BSP_Init
* Description    : Configures RCC,GPIO,NVIC
*******************************************************************************/
void BSP_Init()
{
  /* Configure the system clocks */
  RCC_Configuration();
    
  /* GPIO Configuration */
  GPIO_Configuration();

  /* NVIC Configuration */
  NVIC_Configuration();
}

/*******************************************************************************
* Function Name  : delay_nus
* Description    : delay n us
*******************************************************************************/
void delay_nus(unsigned long n)  //延时n us: n>=6,最小延时单位6us
{ 
  unsigned long j;
  while(n--)              // 外部晶振：8M；PLL：9；8M*9=72MHz
  {
    j=8;				  // 微调参数，保证延时的精度
	while(j--);
  }
}

/*******************************************************************************
* Function Name  : delay_nms
* Description    : delay n ms
*******************************************************************************/
void delay_nms(unsigned long n)  //延时n ms
{
  while(n--)		   // 外部晶振：8M；PLL：9；8M*9=72MHz
    delay_nus(1100);   // 1ms延时补偿
}

