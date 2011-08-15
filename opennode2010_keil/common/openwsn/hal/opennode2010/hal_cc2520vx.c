#include "../hal_configall.h"
#include "../hal_foundation.h"
#include "../hal_cpu.h"
#include "../hal_mcu.h"
#include "../hal_assert.h"
#include "../hal_led.h"
#include "hal_cc2520base.h"
#include "hal_cc2520vx.h"

#define GPIO_SPI GPIOB
#define SPI_pin_MISO  GPIO_Pin_14
#define SPI_pin_MOSI  GPIO_Pin_15
#define SPI_pin_SCK   GPIO_Pin_13
#define SPI_pin_SS    GPIO_Pin_12

// @todo Can we remove this variable?
static SPI_InitTypeDef SPI_InitStructure;

void CC2520_ACTIVATE(void)
{
    int i;

    // activate the SPI module which is used for communication between MCU and cc2520.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,  ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Port B Pin 14 is used for SPI's MISO (IPD means Input Pull Down). 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init( GPIOB,&GPIO_InitStructure);

    // Port B Pin 1 is used for cc2520 RST   
    // Port B Pin 5 is used for VREG_EN
    // Port B Pin 12 is used for NSS  
    // GPIO_Mode_Out_PP here means Push Pull(推挽输出)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOB,&GPIO_InitStructure);

    // reset the cc2520 nRST
    GPIO_ResetBits( GPIOB, GPIO_Pin_1);
    // set VREG_EN which will enable the cc2520's internal voltage regulator
    GPIO_SetBits( GPIOB,GPIO_Pin_5);
    // wait for the regulator to be stabe.
    // @todo
    for ( i=0;i<13500;i++);
    // hal_delayus(?)

    // set the cc2520 nRST
    GPIO_SetBits( GPIOB,GPIO_Pin_1);
    //reset the cc2520 CSn
    GPIO_ResetBits( GPIOB,GPIO_Pin_12);
    // @todo: shall we need to wait a little while after CS and then RST for stable?
    // @todo repalce with hal_delayus(?)
    for ( i=0;i<13500;i++);//wait for the output of SO to be 1//todo for testing
    hal_assert( GPIO_ReadInputDataBit( GPIOB, GPIO_Pin_14));//todo该语句报错，可能是因为SO引脚的 输出模式改变的原
    // set the cc2520 CSn
    GPIO_SetBits( GPIOB, GPIO_Pin_12);
    hal_delayus( 2 );

    // Port B Pin 13 is used for SCK 
    // Port B Pin 15 is used for SPI's MOSI 
    GPIO_InitStructure.GPIO_Pin = SPI_pin_MOSI|SPI_pin_SCK;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIO_SPI, &GPIO_InitStructure);

    // Port B Pin 14 is used for MISO
    GPIO_InitStructure.GPIO_Pin = SPI_pin_MISO;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIO_SPI, &GPIO_InitStructure);

    // Port B Pin 12 is used for NSS
    GPIO_InitStructure.GPIO_Pin = SPI_pin_SS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIO_SPI,&GPIO_InitStructure);
}

/**
 * PB0 => FIFOP
 */
void CC2520_ENABLE_FIFOP( void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    CC2520_REGWR8(CC2520_GPIOCTRL0, CC2520_GPIO_FIFOP);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
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
void CC2520_DISABLE_FIFOP( void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

    EXTI_ClearITPendingBit(EXTI_Line0);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
}

/** 
 * Initialize SPI2 which is connected to cc2520 in OpenNode 2010 design. Other SPI 
 * operations should be sent after calling this function.
 */
void CC2520_SPI_OPEN(void)
{
	SPI_Cmd(SPI2, DISABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI2->CR1 &= 0xDFFF;        //disable CRC
	SPI_SSOutputCmd(SPI2, ENABLE);
	SPI_Cmd( SPI2,ENABLE);      //enable spi2
    
    // @todo: to be organized
	/*************todo*******************/
	/*Enable SPI1.NSS as a GPIO*/
	SPI_SSOutputCmd(SPI2, ENABLE);

	/*Configure PA.4(NSS)--------------------------------------------
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	/**************todo********************
	//start 2520
	GPIO_ResetBits( GPIOB,GPIO_Pin_1);
	hal_delayus( 100);
	GPIO_SetBits( GPIOB,GPIO_Pin_1);
	/*********************************/
}

void CC2520_SPI_CLOSE(void)
{
}

// @todo NSS这一块有问题
void CC2520_SPI_BEGIN(void)
{
	//SPI_NSSInternalSoftwareConfig( SPI2,SPI_NSSInternalSoft_Reset);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12); //reset the cc2520 CSn
	hal_delayus(1);
}

// @todo why uint16?
void CC2520_SPI_TX( uint16 ch)
{
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2, ch);
	//while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
}

// @todo why uint16?
uint16 CC2520_SPI_RX( void)
{
	uint16 ch;
	//SPI_I2S_SendData( SPI2,0x00); //todo 
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	ch = SPI_I2S_ReceiveData(SPI2);
	
	return ch;
}

void CC2520_SPI_WAIT_RXRDY( void)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
}

void CC2520_SPI_END( void)
{
	//SPI_NSSInternalSoftwareConfig(SPI2,SPI_NSSInternalSoft_Set);
	hal_delayus(1);
	GPIO_SetBits( GPIOB,GPIO_Pin_12);
	hal_delayus(1);
}








// todo
#define BIT3 3
// todo end

// todo
// in the previous version, its const
//const digioConfig pinRadio_GPIO0 = {1, 3, BIT3, HAL_DIGIO_INPUT,  0};
digioConfig pinRadio_GPIO0 = {1, 3, BIT3, HAL_DIGIO_INPUT,  0};


// static void halRadioSpiInit(uint32 divider);
// static void halMcuRfInterfaceInit(void);

/***********************************************************************************
* @fn          halRadioSpiInit
*
* @brief       Initalise Radio SPI interface
*
* @param       none
*
* @return      none
*/
/*
static void halRadioSpiInit(uint32 divider)
{
 todo
    UCB1CTL1 |= UCSWRST;                          // Put state machine in reset
    UCB1BR0 = LOWORD(divider);
    UCB1BR1 = HIWORD(divider);
    P5DIR |= 0x01;
    P5SEL |= 0x0E;                               // P7.3,2,1 peripheral select (mux to ACSI_A0)
    UCB1CTL1 = UCSSEL0 | UCSSEL1;                // Select SMCLK
    UCB1CTL0 |= UCCKPH | UCSYNC | UCMSB | UCMST; // 3-pin, 8-bit SPI master, rising edge capture
    UCB1CTL1 &= ~UCSWRST;                        // Initialize USCI state machine
}
*/	

/***********************************************************************************
* @fn      halMcuRfInterfaceInit
*
* @brief   Initialises SPI interface to CC2520 and configures reset and vreg
*          signals as MCU outputs.
*
* @param   none
*
* @return  none
*/
/*
static void halMcuRfInterfaceInit(void)
{
    // Initialize the CC2520 interface
    CC2520_SPI_END();
    CC2520_RESET_OPIN(0);
    CC2520_VREG_EN_OPIN(0);
    CC2520_BASIC_IO_DIR_INIT();
}
*/

/**
 * @brief Initialize the cc2520's virtual execution interface layer.
 * @attention This function should be used by the TiCc2520Adapter component only. 
 * The final user should call cc2520_open() instead of calling this function directly.
 * @param None
 * @return None (But it should always be successful).
 */
 /*
void cc2520_dependent_init(void)
{
    halRadioSpiInit(0);
    halMcuRfInterfaceInit();
    
    halDigioConfig(&pinRadio_GPIO0);
}
*/

