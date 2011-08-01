#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cc2520vx.h"
#include "hal_mcu.h"

// todo
#define BIT3 3
// todo end

// todo
// in the previous version, its const
//const digioConfig pinRadio_GPIO0 = {1, 3, BIT3, HAL_DIGIO_INPUT,  0};
digioConfig pinRadio_GPIO0 = {1, 3, BIT3, HAL_DIGIO_INPUT,  0};

static SPI_InitTypeDef SPI_InitStructure;

static void halRadioSpiInit(uint32 divider);
static void halMcuRfInterfaceInit(void);

/***********************************************************************************
* @fn          halRadioSpiInit
*
* @brief       Initalise Radio SPI interface
*
* @param       none
*
* @return      none
*/
static void halRadioSpiInit(uint32 divider)
{
/* todo
    UCB1CTL1 |= UCSWRST;                          // Put state machine in reset
    UCB1BR0 = LOWORD(divider);
    UCB1BR1 = HIWORD(divider);
    P5DIR |= 0x01;
    P5SEL |= 0x0E;                               // P7.3,2,1 peripheral select (mux to ACSI_A0)
    UCB1CTL1 = UCSSEL0 | UCSSEL1;                // Select SMCLK
    UCB1CTL0 |= UCCKPH | UCSYNC | UCMSB | UCMST; // 3-pin, 8-bit SPI master, rising edge capture
    UCB1CTL1 &= ~UCSWRST;                        // Initialize USCI state machine
*/	
}

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
static void halMcuRfInterfaceInit(void)
{
    // Initialize the CC2520 interface
    CC2520_SPI_END();
    CC2520_RESET_OPIN(0);
    CC2520_VREG_EN_OPIN(0);
    CC2520_BASIC_IO_DIR_INIT();
}

/**
 * @brief Initialize the cc2520's virtual execution interface layer.
 * @attention This function should be used by the TiCc2520Adapter component only. 
 * The final user should call cc2520_open() instead of calling this function directly.
 * @param None
 * @return None (But it should always be successful).
 */
void cc2520_dependent_init(void)
{
    halRadioSpiInit(0);
    halMcuRfInterfaceInit();
    
    halDigioConfig(&pinRadio_GPIO0);
}
//
//与2520相连的是spi2
void CC2520_SPI_OPEN( void)//initialize sip2.
{
	SPI_Cmd( SPI2,DISABLE);
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

	SPI2->CR1 &= 0xDFFF;//disable CRC
	SPI_SSOutputCmd(SPI2, ENABLE);
	SPI_Cmd( SPI2,ENABLE);//enable spi2
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

void CC2520_SPI_BEGIN( void)//NSS这一块有问题
{
	//SPI_NSSInternalSoftwareConfig( SPI2,SPI_NSSInternalSoft_Reset);
	GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
	hal_delayus(1);
}


void CC2520_SPI_TX( uint16 ch)
{
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2, ch);
	//while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
}

uint16 CC2520_SPI_RX( void)
{
	uint16 ch;
	//SPI_I2S_SendData( SPI2,0x00);//todo 
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
