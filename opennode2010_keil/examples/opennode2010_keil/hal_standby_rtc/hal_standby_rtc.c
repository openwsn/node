
#include "apl_foundation.h"

USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void RCC_Configuration(void);
static void USART_GPIO_Configuration( void);
void USART2_Init( void);
static uint8 USART_Send( uint8 ch);
static void NVIC_Configuration( void);
static void RTC_Configuration( void);
static void Start_RTC( void);
//void RTC_IRQHandler(void);
void RTCAlarm_IRQHandler( void );
void EXTI_Configuration(void);


void RCC_Configuration(void)
{
	RCC_PCLK2Config(RCC_HCLK_Div8);
	RCC_PCLK1Config(RCC_HCLK_Div8);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

}


static void USART_GPIO_Configuration( void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void USART2_Init( void)
{
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART2,&USART_InitStructure);
	USART_Cmd( USART2,ENABLE);
}

static uint8 USART_Send( uint8 ch)
{
	USART_SendData( USART2,ch);
	while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
	{
	}
}

static void NVIC_Configuration( void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void RTC_Configuration( void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);


	PWR_WakeUpPinCmd(ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Reset Backup Domain */
	BKP_DeInit(); //记录0XA5A5 来确定是否重置时间

	/* Enable LSE */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{}

	/* Select LSE as RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);

	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	RTC_SetAlarm(RTC_GetCounter()+ 2);
	RTC_WaitForLastTask();

	/* Enable the RTC Alarm */
	RTC_ITConfig(RTC_IT_ALR, ENABLE);

	RTC_ITConfig( RTC_IT_SEC,DISABLE);//todo for testing

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
}

//static void Start_RTC( void)
//{
//	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
//  {
//    /* Backup data register value is not correct or not yet programmed (when
//       the first time the program is executed) */
//     USART_Send( 0x00);
//  
//    /* RTC Configuration */
//    RTC_Configuration();
//
//    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
//  }
//  else
//  {
//    /* Check if the Power On Reset flag is set */
//    if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
//    {
//      //printf("\r\n\n Power On Reset occurred....");
//		USART_Send( 0x01);
//    }
//    /* Check if the Pin Reset flag is set */
//    else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
//    {
//      //printf("\r\n\n External Reset occurred....");
//		USART_Send( 0x02);
//    }
//
//   // printf("\r\n No need to configure RTC....");
// /*一下都是可以省略的 RTC_Configuration 已有启用 RTC_IT_SEC */
//    /* Wait for RTC registers synchronization */
//    RTC_WaitForSynchro();
//
//    /* Enable the RTC Second */
//    RTC_ITConfig(RTC_IT_SEC, ENABLE);
//    /* Wait until last write operation on RTC registers has finished */
//    RTC_WaitForLastTask(); 
//  }
//}


void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	/* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


void RTCAlarm_IRQHandler(void)
{
	//EXTI_ClearITPendingBit(EXTI_Line17);
	if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_ALR);
	}

	
}



	
	   void init();

void main (void)
{
     init();

	//led_on( LED_RED);

	USART_Send( 0xff);

	while (1)
	{
		USART_Send( 0xac);
		/* Insert 1.5 second delay */
		//hal_delay( 1000);
		/* Wait till RTC Second event occurs */
		//RTC_ClearFlag(RTC_FLAG_SEC);
		//while(RTC_GetFlagStatus(RTC_FLAG_SEC) == RESET);
		/* Alarm in 3 second */
		RTC_WaitForLastTask();
		RTC_SetAlarm(RTC_GetCounter()+ 2);

		USART_Send( 0xad);
		/* Wait until last write operation on RTC registers has finished */
		USART_Send( 0xae);
		//RTC_WaitForLastTask();
		//hal_delay(500);
		USART_Send( 0x01);
		USART_Send( 0x02);
		//USART_Send( 0x02);
        led_on( LED_RED);
		hal_delayms(500);
		//USART_Send( 0xaf);
		
		PWR_EnterSTANDBYMode();//进入standby模式后，当程序唤醒时，将从main（）处重新开始运行。
		RCC_DeInit();
		init();
		//RCC_Configuration( );
		//USART_GPIO_Configuration();
	    //USART2_Init();
		//hal_delay( 5000);
		USART_Send( 0xab);
		//RCC_Configuration( );
		//led_open();
		/* At this stage the system has resumed from STOP mode -------------------*/
		//SystemInit();
		//RCC_DeInit(); 
		//RCC_SYSCLKConfig( RCC_SYSCLKSource_HSI);
		//RCC_Configuration();

	}
	
}

	   void init()
	   {
	       RCC_Configuration( );
    led_open();
    led_off( LED_RED);
    USART_GPIO_Configuration();
	USART2_Init();
    
	//EXTI_Configuration();
	NVIC_Configuration();
	RTC_Configuration();
	   }
