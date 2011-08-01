 #include "apl_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
//#include "../../common/openwsn/hal/opennode2010/hal_led.h"

USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
ADC_InitTypeDef ADC_InitStructure;


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
void ADC_GPIO_Configuration(void);
void usart2_init( void);
void ADC_Configuration( void);
u16 TestAdc(void);


void RCC_Configuration(void)
{
	RCC_PCLK2Config(RCC_HCLK_Div8);
	RCC_PCLK1Config(RCC_HCLK_Div8);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //由于是要用于触发中断，所以还需要打开GPIO复用的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);

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

void ADC_GPIO_Configuration(void)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void ADC_Configuration( void)
{
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ADC1 regular channel14 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_55Cycles5);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	/* Enable ADC1 reset calibration register */   
	//ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	//while(ADC_GetResetCalibrationStatus(ADC1));
	/* Start ADC1 calibration */
	//ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	//while(ADC_GetCalibrationStatus(ADC1));
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}

u16 TestAdc(void)
{
	u16 adc;
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==RESET); //检查制定ADC标志位置1与否 ADC_FLAG_EOC 转换结束标志位
	adc=ADC_GetConversionValue(ADC1);

	return adc;//返回最近一次ADCx规则组的转换结果

}

void usart2_init( void)
{
	/***********************************************/
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
	uint16 adc;
	len = 0x00;
    RCC_Configuration( );
    led_open();
    led_off( LED_RED);//todo 下面三句的顺序不能变
    CC2520_Activate();
	SPI_GPIO_Configuration();
	CC2520_SPI_OPEN( );
	//CC2520_SPI_BEGIN();
    USART_GPIO_Configuration();
	ADC_GPIO_Configuration();
	ADC_Configuration();
	usart2_init();	
    halRfInit();//todo 设置相应的寄存器

	halRfSetPower( TXPOWER_4_DBM);
	halRfSetChannel( channel);
	halRfSetShortAddr( 0x02);
	halRfSetPanId( 0x01);

	hal_delay( 2);
	
	//enable GPIO0 and GPIO1 for temperature sensor
	CC2520_REGWR8(CC2520_GPIOCTRL0, 0x80);
	hal_delay(2);
	CC2520_REGWR8(CC2520_GPIOCTRL1, 0x80);
	hal_delay( 2);
	CC2520_REGWR8(CC2520_GPIOCTRL, 0x40);
	hal_delay( 2);
	CC2520_REGWR8(CC2520_ATEST, 0x01);
	hal_delay( 2);

	

	while (1)
	{
       adc = TestAdc();
	   len = ( uint8)( adc&0x00ff);
	   USART_Send( len);
	   len = ( uint8)( adc>>8);
	   USART_Send( len);
	   hal_delay( 1000);

	}
	
}


