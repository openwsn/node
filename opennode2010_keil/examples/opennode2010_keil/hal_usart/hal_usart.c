

#include "apl_foundation.h"

USART_InitTypeDef USART_InitStructure;
USART_ClockInitTypeDef USART_ClockInitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

/*
static void USART_GPIOConfiguration( void);
static void USART_RCCconfiguration(void);
static void USART_NVICcofiguration(void);
static void USART_INIT(void);
void USART2_IRQHandler(void);

int main( void)
{
	uint16 ch;
	ch = 0x00;
	SystemInit();
	led_open();
	led_on(LED_RED);
	hal_delay( 1000);
	led_off( LED_RED);
	USART_RCCconfiguration();
	USART_GPIOConfiguration();
	USART_NVICcofiguration();
	USART_INIT();
//todo for testing
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
//
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIO_Init(GPIOD, &GPIO_InitStructure);
//	GPIO_ResetBits( GPIOA,GPIO_Pin_8);
//	GPIO_SetBits( GPIOA, GPIO_Pin_0);
//	GPIO_SetBits( GPIOA, GPIO_Pin_1);
//	GPIO_SetBits( GPIOA, GPIO_Pin_2);
//	GPIO_SetBits( GPIOA, GPIO_Pin_3);
//	GPIO_SetBits( GPIOA, GPIO_Pin_4);
//	GPIO_SetBits( GPIOA, GPIO_Pin_5);


	while (1)
	{
		
		USART_SendData( USART2,0x01);
		
		while ( USART_GetITStatus( USART2,USART_FLAG_TC))
		{
		}
        
		hal_delay( 300);
	}
}

static void USART_RCCconfiguration(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

static void USART_GPIOConfiguration( void)
{
	// Configure USART2 Tx (PA.02) as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	// Configure USART2 Rx (PA.3) as input floating 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	


}

static void USART_NVICcofiguration(void)
{
	//Configure the NVIC Preemption Priority Bits 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	//Enable the USART2 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


}

static void USART_INIT(void)
{
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	//USART_InitStructure.USART_Clock = USART_Clock_Disable;
	//USART_InitStructure.USART_CPOL = USART_CPOL_Low;
	//USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
	//USART_InitStructure.USART_LastBit = USART_LastBit_Disable; 

	USART_Init(USART2, &USART_InitStructure);
	//USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);    //开串口2接收中断

	//Enable USART2 
	USART_Cmd(USART2, ENABLE);


}

void USART2_IRQHandler(void)
{
	

	

	//处理接收到的数据
//	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
//	{
//
//
//		Rs485Rx[Rs485Rxcont]=USART_ReceiveData(USART2);
//
//		Rs485Rxcont++;
//		if(Rs485Rxcont>=7)
//		{
//			Usart2_Get_Flag=TRUE;   
//			Delay(3);
//		}
//
//
//		// Clear the USART2 Receive interrupt
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE);  
//
//	}
	
}

*/
 //todo从正面看，黑线（第一根）接地，第二根线接下数第4根线（tx），第三根线接下数第5根线（rx）
void RCC_Configuration( void);
void GPIO_Configuration( void);

int main( void)
{
	uint8 ch;
	ch = 0x00;
	led_open();
	RCC_Configuration();
	GPIO_Configuration();

	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init( USART2,&USART_InitStructure);
	USART_Cmd( USART2,ENABLE);
    led_on( LED_RED);
	hal_delayms( 1000);
	led_off( LED_RED);
	/*
	while ( 1)//实现简单的发送功能
	{
		USART_SendData( USART2,0x45);
		while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		{
		}
		led_on( LED_RED);
		hal_delay( 500);
		led_off( LED_RED);
		hal_delay( 500);
	}
	*/

	while ( 1)//由串口调试助手发送数据后再向串口调试助手会送想通的数据（usart_echo)
	{
		//USART_SendData( USART2,0x45);
		//while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		//{
		//}
		while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
			{
		    }
		ch = (USART_ReceiveData(USART2) & 0xFF); 

        led_toggle(LED_RED);

		USART_SendData( USART2,ch);
		while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		{
		}
       // led_on( LED_RED);
		//hal_delay( 500);
		//led_off( LED_RED);
		//hal_delay( 500);
	}

}

void RCC_Configuration( void)
{
	
//	RCC_APB2PeriphClockCmd(USARTy_GPIO_CLK | USARTz_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);
//	 
//		 #ifndef USE_STM3210C_EVAL
//		
//		 RCC_APB2PeriphClockCmd(USARTy_CLK, ENABLE); 
//	 #else
//		
//		 RCC_APB1PeriphClockCmd(USARTy_CLK, ENABLE); 
//	#endif
//		
//		RCC_APB1PeriphClockCmd(USARTz_CLK, ENABLE);  

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

}

void GPIO_Configuration( void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	// Configure USART2 Rx (PA.3) as input floating 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
