 #include "apl_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
//#include "../../common/openwsn/hal/opennode2010/hal_led.h"

USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

#define MAX_IEEE802FRAME154_SIZE                128//todo

#define FAILED 1
#define RXFIFO_START 0x180
#define  channel 11

#define GPIO_SPI GPIOB
#define SPI_pin_MISO  GPIO_Pin_14
#define SPI_pin_MOSI  GPIO_Pin_15
#define SPI_pin_SCK   GPIO_Pin_13
#define SPI_pin_SS    GPIO_Pin_12


/***********************************************************************************
 * LOCAL FUNCTIONS
 */

static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiIEEE802Frame154Descriptor m_desc;

void RCC_Configuration(void);
static void CC2520_Activate( void);
static void SPI_GPIO_Configuration( void);
static void USART_GPIO_Configuration( void);
static uint8 USART_Send( uint8 ch);
void GPIO_Interrupt_Ini( void);
void EXTI9_5_IRQHandler(void);


void RCC_Configuration(void)
{
	RCC_PCLK2Config(RCC_HCLK_Div8);
	RCC_PCLK1Config(RCC_HCLK_Div8);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //由于是要用于触发中断，所以还需要打开GPIO复用的时钟.

}

static void SPI_GPIO_Configuration( void)
{
	GPIO_InitStructure.GPIO_Pin = SPI_pin_MOSI|SPI_pin_SCK;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIO_SPI,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_pin_MISO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIO_SPI,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SPI_pin_SS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( GPIO_SPI,&GPIO_InitStructure);
}

static void USART_GPIO_Configuration( void)
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

static void CC2520_Activate( void)
{
	int i;
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
//   /*
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//    */
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init( GPIOB,&GPIO_InitStructure);
	

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( GPIOB,&GPIO_InitStructure);

	//GPIO_ResetBits( GPIOA,GPIO_Pin_8);

	/*********************************************************/
	//GPIO_ResetBits( GPIOB,GPIO_Pin_5);//reset the VREG_EN
	//GPIO_SetBits( GPIOB,GPIO_Pin_5);//set the VREG_EN
	//GPIO_ResetBits( GPIOB,GPIO_Pin_5);//reset the VREG_EN
	//GPIO_SetBits( GPIOB,GPIO_Pin_1);////set the cc2520 nRST
	//GPIO_ResetBits( GPIOB,GPIO_Pin_1)//reset the cc2520 nRST
    //GPIO_SetBits( GPIOB,GPIO_Pin_12);//set the cc2520 CSn
	//GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
	//GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14);//read the input of the SO.
    /************************************************************************/
    GPIO_ResetBits( GPIOB,GPIO_Pin_1);//reset the cc2520 nRST
	GPIO_SetBits( GPIOB,GPIO_Pin_5);//set the VREG_EN
	for ( i=0;i<13500;i++);//wait for the regulator to be stabe.

	GPIO_SetBits( GPIOB,GPIO_Pin_1);////set the cc2520 nRST
	GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
	for ( i=0;i<13500;i++);//wait for the output of SO to be 1//todo for testing
	hal_assert( GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14));//todo该语句报错，可能是因为SO引脚的 输出模式改变的原
	GPIO_SetBits( GPIOB,GPIO_Pin_12);//set the cc2520 CSn
	hal_delayus( 2 );
}


static uint8 USART_Send( uint8 ch)
{
	USART_SendData( USART2,ch);
	while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
	{
	}
}



/***********************************************************************************
 * @fn          main
 *
 * @brief       This is the main entry of the "Read Regs" application
 *
 * @param       none
 *
 * @return      none
 */

void main (void)
{
	int i;
	uint8 len;
	uint8 data[40];
	len = 0x00;
    RCC_Configuration( );
    led_open();
    led_off( LED_RED);//todo 下面三句的顺序不能变
    CC2520_Activate();
	SPI_GPIO_Configuration();
	CC2520_SPI_OPEN( );
	//CC2520_SPI_BEGIN();
    USART_GPIO_Configuration();

	/***********************************************/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init( USART2,&USART_InitStructure);
	USART_Cmd( USART2,ENABLE);
    halRfInit();//todo 设置相应的寄存器
	hal_delay( 2);
	//CC2520_REGWR8(CC2520_GPIOCTRL3, CC2520_GPIO_FIFOP);//设置CC2520_GPIO3为fifop引脚功能
	//CC2520_REGWR8(CC2520_GPIOCTRL4, CC2520_GPIO_FIFOP);//设置CC2520_GPIO4为fifop引脚功能
	//CC2520_REGWR8(CC2520_GPIOCTRL5, CC2520_GPIO_FIFOP);//设置CC2520_GPIO4为fifop引脚功能
	CC2520_REGWR8(CC2520_GPIOCTRL0, CC2520_GPIO_FIFOP);//设置CC2520_GPIO4为fifop引脚功能
	//CC2520_REGWR8(CC2520_GPIOCTRL2, CC2520_GPIO_FIFOP);//设置CC2520_GPIO4为fifop引脚功能
	hal_delay( 2);

	halRfSetPower( TXPOWER_4_DBM);
	halRfSetChannel( channel);
	halRfSetShortAddr( 0x02);
	halRfSetPanId( 0x01);

    GPIO_Interrupt_Ini();
	CC2520_SRFOFF();
	CC2520_SRXON();
	while (1)
	{
//		for ( i=0;i<40;i++)
//		{
//			data[i] = 0;
//		}
//		hal_delayus( 2);
//
//		if (CC2520_REGRD8( CC2520_EXCFLAG1)&0x11)
//		{
//			led_toggle( LED_RED);
//
//			len = CC2520_RXBUF8();
//
//			
//			
//			CC2520_RXBUF( len,data);
//			 
//
//			for ( i=0;i<len;i++)
//			{
//				USART_Send( data[i]);
//			}
//            CC2520_REGWR8(CC2520_EXCFLAG1,0x00);//todo clear the exception
//            CC2520_SFLUSHRX();//todo clear the rxfifo
//			CC2520_SFLUSHRX();//todo clear the rxfifo
//
//		}
//		USART_Send( CC2520_REGRD8( CC2520_EXCFLAG0));
//		USART_Send( CC2520_REGRD8( CC2520_EXCFLAG1));
//		USART_Send( CC2520_REGRD8( CC2520_EXCFLAG2));
//		USART_Send( CC2520_REGRD8( CC2520_RXFIFOCNT));
//		hal_delay( 1000);
		//GPIOPOLARITY
//		hal_assert( GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_8));
//		len = GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_8);
//		USART_Send(len);
//		if ( len>0)
//		{
//			led_toggle( LED_RED);
//			len = CC2520_RXBUF8();
//			CC2520_RXBUF( len,data);
//			USART_Send( CC2520_REGRD8( CC2520_EXCFLAG0));
//			USART_Send( CC2520_REGRD8( CC2520_EXCFLAG1));
//			USART_Send( CC2520_REGRD8( CC2520_EXCFLAG2));
//			USART_Send( CC2520_REGRD8( CC2520_GPIOCTRL3));
//		}
//		led_toggle( LED_RED);
//		hal_delay( 1000);
	}
	
}

/*****************************extern interrupt*****************************************************/

/***********************************************
*测试cc2520_GPIO3
************************************************/

//void GPIO_Interrupt_Ini( void)
//{
//	/* Configure PB.8 as input floating */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//
//	/* Enable the EXTI15_10 Interrupt */
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//EXTI15_10_IRQn; //EXTI15_10_IRQChannel;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//
//    EXTI_ClearITPendingBit(EXTI_Line8);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);
//	// Configure EXTI Line11 to generate an interrupt on falling edge 
//	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//
//
//
//	
//}
//
//
//void EXTI9_5_IRQHandler(void)
//{
//	led_toggle( LED_RED);
//	CC2520_SFLUSHRX();
//	CC2520_SFLUSHRX();
//	EXTI_ClearITPendingBit(EXTI_Line8); 
////	uint8 len;
////	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
////	{
////		led_toggle( LED_RED);
////		USART_Send( 0xab);
////		len = CC2520_RXBUF8();
////		CC2520_SFLUSHRX();
////		CC2520_SFLUSHRX();
////		EXTI_ClearITPendingBit(EXTI_Line8); 
////	} 
//}

/****************************************************************************
*测试cc2520_GPIO4
******************************************************************************/
//void GPIO_Interrupt_Ini( void)
//{
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//
//	EXTI_ClearITPendingBit(EXTI_Line9);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//
//
//
//
//}
//
//
//void EXTI9_5_IRQHandler(void)
//{
//	led_toggle( LED_RED);
//	USART_Send(0xa0);
//	CC2520_SFLUSHRX();
//	CC2520_SFLUSHRX();
//	EXTI_ClearITPendingBit(EXTI_Line9); 
//	
//}

/*******************************************************************
*测试cc2520_GPIO5
********************************************************************/
//void GPIO_Interrupt_Ini( void)
//{
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//
//	EXTI_ClearITPendingBit(EXTI_Line10);
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//}
//
//
//void EXTI15_10_IRQHandler(void)
//{
//	led_toggle( LED_RED);
//	USART_Send(0xb0);
//	CC2520_SFLUSHRX();
//	CC2520_SFLUSHRX();
//	EXTI_ClearITPendingBit(EXTI_Line10); 
//
//}

/********************************************************************
*测试cc2520_GPIO0
***********************************************************************/
void GPIO_Interrupt_Ini( void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	EXTI_ClearITPendingBit(EXTI_Line0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void EXTI0_IRQHandler(void)
{
	led_toggle( LED_RED);
	USART_Send(0xc0);
	CC2520_SFLUSHRX();
	CC2520_SFLUSHRX();
	EXTI_ClearITPendingBit(EXTI_Line0); 

}

