
/*
*下面的是闪灯程序，调试成功
*/
/*
#include "apl_foundation.h"

void main (void)
{ 
  	int i;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIO_Init(GPIOD, &GPIO_InitStructure);
	//GPIO_ResetBits( GPIOA,GPIO_Pin_8);
	GPIO_SetBits( GPIOA, GPIO_Pin_0);
	GPIO_SetBits( GPIOA, GPIO_Pin_1);
	GPIO_SetBits( GPIOA, GPIO_Pin_2);
	GPIO_SetBits( GPIOA, GPIO_Pin_3);
	GPIO_SetBits( GPIOA, GPIO_Pin_4);
	GPIO_SetBits( GPIOA, GPIO_Pin_5);


  
  while( 1) {
  GPIO_SetBits( GPIOA, GPIO_Pin_8);
  for(i=0;i<100000;i++);
  GPIO_ResetBits( GPIOA,GPIO_Pin_8);
  for(i=0;i<100000;i++); 
   
  }
}
*/

/******************************************************************************/

 /*
 * 下面是定时器的程序
 */
/*
#include "apl_foundation.h"

/* Private function prototypes -----------------------------------------------*/
/*void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void TIM3_Configuration(void);

/* Private functions ---------------------------------------------------------*/

 /*
int main(void)
{
	RCC_Configuration();
	NVIC_Configuration();
	GPIO_Configuration();
	TIM3_Configuration();


	TIM_ClearFlag(TIM3, TIM_FLAG_Update);/*清除更新标志位*/
/*	TIM_ARRPreloadConfig(TIM3, DISABLE);/*预装载寄存器的内容被立即传送到影子寄存器 */

/*	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);   
	TIM_Cmd(TIM3, ENABLE);

	while (1) {
		
	};
}

void TIM3_Configuration(void)
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 1000; 
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
}

void RCC_Configuration(void)
{
	SystemInit();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void GPIO_Configuration(void)
{
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits( GPIOA, GPIO_Pin_8);

}


void TIM3_IRQHandler(void)
{
  
  //TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  //GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8)));

	//GPIO_ResetBits( GPIOA,GPIO_Pin_8);
    
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8)));
	}
     
}

	*/
	

/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
***********************************************************************************/

/***********************************************************************************
  Filename: reg_read.c

  Description: This application reads the register values of CC2520. The values
  are output on the serial port. 

  Configure the PC HyperTerminal for baudrate 38400 bps, 8 data bits
  no parity, and 1 stop bit. 

***********************************************************************************/

/***********************************************************************************
 * INCLUDES
 */



#include "apl_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
//#include "../../common/openwsn/hal/opennode2010/hal_led.h"
#define MAX_IEEE802FRAME154_SIZE                128//todo

#define FAILED 1
#define  channel 11

#define GPIO_SPI GPIOB
#define SPI_pin_MISO  GPIO_Pin_14
#define SPI_pin_MOSI  GPIO_Pin_15
#define SPI_pin_SCK   GPIO_Pin_13
#define SPI_pin_SS    GPIO_Pin_12


/***********************************************************************************
 * CONSTANTS
 */

// Define SREG or FREG for output to serial port

#define FREG            

#define KVP(v)	{ v, #v }

typedef struct {
    const uint8 iRegNum;
    const char *szRegName;
} regKvp_t;


/***********************************************************************************
 * LOCAL VARIABLES
 */


#ifdef SREG
// CC2520 SREG names
static regKvp_t regLookup[]=
{
    KVP(CC2520_CHIPID),
    KVP(CC2520_VERSION),
    KVP(CC2520_EXTCLOCK),
    KVP(CC2520_MDMCTRL0),
    KVP(CC2520_MDMCTRL1),
    KVP(CC2520_FREQEST),
    KVP(CC2520_RXCTRL),
    KVP(CC2520_FSCTRL),
    KVP(CC2520_FSCAL0),
    KVP(CC2520_FSCAL1),
    KVP(CC2520_FSCAL2),
    KVP(CC2520_FSCAL3),
    KVP(CC2520_AGCCTRL0),
    KVP(CC2520_AGCCTRL1),
    KVP(CC2520_AGCCTRL2),
    KVP(CC2520_AGCCTRL3),
    KVP(CC2520_ADCTEST0),
    KVP(CC2520_ADCTEST1),
    KVP(CC2520_ADCTEST2),
    KVP(CC2520_MDMTEST0),
    KVP(CC2520_MDMTEST1),
    KVP(CC2520_DACTEST0),
    KVP(CC2520_DACTEST1),
    KVP(CC2520_ATEST),
    KVP(CC2520_DACTEST2),
    KVP(CC2520_PTEST0),
    KVP(CC2520_PTEST1),
    KVP(CC2520_DPUBIST),
    KVP(CC2520_ACTBIST),
    KVP(CC2520_RAMBIST),
    KVP(0xFF),
};
#elif defined FREG
// CC2520 FREG names
static regKvp_t regLookup[]=
{
    KVP(CC2520_FRMFILT0),
    KVP(CC2520_FRMFILT1),
    KVP(CC2520_SRCMATCH),
    KVP(CC2520_SRCSHORTEN0),
    KVP(CC2520_SRCSHORTEN1),
    KVP(CC2520_SRCSHORTEN2),
    KVP(CC2520_SRCEXTEN0),
    KVP(CC2520_SRCEXTEN1),
    KVP(CC2520_SRCEXTEN2),
    KVP(CC2520_FRMCTRL0),
    KVP(CC2520_FRMCTRL1),
    KVP(CC2520_RXENABLE0),
    KVP(CC2520_RXENABLE1),
    KVP(CC2520_EXCFLAG0),
    KVP(CC2520_EXCFLAG1),
    KVP(CC2520_EXCFLAG2),
    KVP(CC2520_EXCMASKA0),
    KVP(CC2520_EXCMASKA1),
    KVP(CC2520_EXCMASKA2),
    KVP(CC2520_EXCMASKB0),
    KVP(CC2520_EXCMASKB1),
    KVP(CC2520_EXCMASKB2),
    KVP(CC2520_EXCBINDX0),
    KVP(CC2520_EXCBINDX1),
    KVP(CC2520_EXCBINDY0),
    KVP(CC2520_EXCBINDY1),
    KVP(CC2520_GPIOCTRL0),
    KVP(CC2520_GPIOCTRL1),
    KVP(CC2520_GPIOCTRL2),
    KVP(CC2520_GPIOCTRL3),
    KVP(CC2520_GPIOCTRL4),
    KVP(CC2520_GPIOCTRL5),
    KVP(CC2520_GPIOPOLARITY),
    KVP(CC2520_GPIOCTRL),
    KVP(CC2520_DPUCON),
    KVP(CC2520_DPUSTAT),
    KVP(CC2520_FREQCTRL),
    KVP(CC2520_FREQTUNE),
    KVP(CC2520_TXPOWER),
    KVP(CC2520_TXCTRL),
    KVP(CC2520_FSMSTAT0),
    KVP(CC2520_FSMSTAT1),
    KVP(CC2520_FIFOPCTRL),
    KVP(CC2520_FSMCTRL),
    KVP(CC2520_CCACTRL0),
    KVP(CC2520_CCACTRL1),
    KVP(CC2520_RSSI),
    KVP(CC2520_RSSISTAT),
    KVP(CC2520_TXFIFO_BUF),
    KVP(CC2520_RXFIRST),
    KVP(CC2520_RXFIFOCNT),
    KVP(CC2520_TXFIFOCNT),
    KVP(0xFF),
};
#endif


/***********************************************************************************
 * LOCAL FUNCTIONS
 */

static char                 m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
TiIEEE802Frame154Descriptor m_desc;

static void appPrintMenu(void);
void RCC_Configuration(void);
static void CC2520_Activate( void);
static void SPI_GPIO_Configuration( void);


/******************************************************************************
 * @fn          appPrintMenu
 *
 * @brief       Prints menu on UART 
 *
 * @param       none
 *
 * @return      none
 */


static void appPrintMenu(void)
{
  printStr("------------------------------\n");
  printStr("CC2520 Application Example\n");
  printStr("Read Registers\n");
  printStr("------------------------------\n\n");
}



/*********************************************************************
 * @fn      appPrintRfRegs
 *
 * @brief   Print radio registers
 *
 * @param   none
 *
 * @return  none
 */



static void appPrintRfRegs(void)
{
    regKvp_t *p;
	uint8 state = 0;
    uint16 iRegVal;

    p= regLookup;

    while ( p->iRegNum != 0xFF) {
		led_on( LED_RED );
		
		// Read radio registers
        #ifdef SREG
        iRegVal = CC2520_MEMRD8(p->iRegNum);
        #elif defined FREG
        iRegVal = CC2520_REGRD8(p->iRegNum);
        #endif

        // Print name and contents
        printKvpHex((char*)p->szRegName,iRegVal);
		led_off( LED_RED );
		state = CC2520_SNOP();
        
        p++;
    }
}


void RCC_Configuration(void)
{
	RCC_PCLK2Config(RCC_HCLK_Div8);
	RCC_PCLK1Config(RCC_HCLK_Div8);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

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
	//hal_delayus( 2 );
	GPIO_SetBits( GPIOB,GPIO_Pin_5);//set the VREG_EN
//hal_delayus( 2 );
	for ( i=0;i<13500;i++);//wait for the regulator to be stabe.

	GPIO_SetBits( GPIOB,GPIO_Pin_1);////set the cc2520 nRST
//hal_delayus( 2 );
	GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
//hal_delayus( 2 );
	for ( i=0;i<13500;i++);//wait for the output of SO to be 1//todo for testing
	hal_assert( GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14));//todo该语句报错，可能是因为SO引脚的 输出模式改变的原因
	
    /*
	//if the output of SO is 1 then set the CSn
	while( !GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14))
	{
	}
	*/
	hal_delayus( 2 );
	GPIO_SetBits( GPIOB,GPIO_Pin_12);//set the cc2520 CSn
	hal_delayus( 2 );
	//GPIO_SetBits( GPIOA,GPIO_Pin_8);
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
    uint16 g;
	uint8 len;
	uint8 state;
	uint8 k;
	TiFrame *txbuf;//todo
	char * ptr;//todo
	TiIEEE802Frame154Descriptor * desc;//todo
    g = 0x01;
	state = 0x00;
    RCC_Configuration( );
    led_open();
    led_off( LED_RED);//todo 下面三句的顺序不能变
    CC2520_Activate();
	SPI_GPIO_Configuration();
	CC2520_SPI_OPEN( );
	//CC2520_SPI_BEGIN();

    //现在还缺少相应寄存器的配置;这可能是导致发送失败的原因之一
    halRfInit();//todo 设置相应的寄存器

	halRfSetPower( CC2520_TXPOWER_4_DBM);
	halRfSetChannel( channel);
	halRfSetShortAddr( 0x01);
	halRfSetPanId( 0x01);
    /*
	for ( i=0;i<0xff;i++)
	{
		CC2520_SPI_BEGIN();
		CC2520_SPI_TX( 0x02);
		g = CC2520_SPI_RX();
		CC2520_SPI_END();
		//CC2520_SRXON();
		//CC2520_SRXON();
		CC2520_SXOSCON();
		hal_delayus( 10);
		CC2520_STXON();
		//CC2520_INS_STROBE( 0x42);
		hal_delayus( 10);
		g = CC2520_SNOP();
	}

	if ( g==0)
	{
		led_on( LED_RED);
	}
 
    appPrintMenu();
  
    while(TRUE) 
    {
  
      appPrintRfRegs();
    }
	*/
	desc = ieee802frame154_open( &m_desc );
	txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );

	
	while (1)//todo for testing
	{
		frame_reset( txbuf,3,20,0);
		ptr = frame_startptr( txbuf);

		for ( i = 0;i< 6;i++)
			ptr[i] = i;
		frame_skipouter( txbuf,12,2);
		desc = ieee802frame154_format( desc, frame_startptr( txbuf), frame_capacity( txbuf ), 
		FRAME154_DEF_FRAMECONTROL_DATA ); 
		rtl_assert( desc != NULL );
		ieee802frame154_set_sequence( desc, 0x00); 
		ieee802frame154_set_panto( desc,  0x01 );
		ieee802frame154_set_shortaddrto( desc, 0x02 );
		ieee802frame154_set_panfrom( desc,  0x01 );
		ieee802frame154_set_shortaddrfrom( desc, 0x02 );
		frame_setlength( txbuf,20);
		len = frame_length( txbuf);
        //todo

		//while (HAL_READ_CC_SFD_PIN() == 1) {};//todo 这一句代码在CC2420中是来判断上一次发送是否为空，2520应该怎写？

		
			CC2520_SFLUSHTX();//cc2420_sendcmd( cc, CC2420_SFLUSHTX );//应写成想2520发送tx使能指令
            state = CC2520_SNOP();
			/*
			if (state)
			{
				led_on( LED_RED);
			} 
			else
			{
				led_off( LED_RED);
			}*/
			CC2520_TXBUF(len,(uint8*)txbuf);//todo 这一句可能有问题
		    CC2520_SRFOFF();
			CC2520_STXON();
			/*
			while(1)
			{
		 	     state = CC2520_SNOP();
				 if ( state&0x02)
				 {
					hal_assert( 0);
				 }
			}*/

			//hal_assert(  CC2520_REGRD8( CC2520_EXCFLAG0)&0x01);//灯在闪，page 112.
			//hal_assert(  CC2520_REGRD8( CC2520_EXCFLAG0)&0x02);//灯没闪,说明TX_FRM_DONE，把上一句
			//的CC2520_STXON()或CC2520_TXBUF(len,(uint8*)txbuf);注释掉后灯就开始闪 
			//hal_assert(  CC2520_REGRD8( CC2520_EXCFLAG0)&0x04);//灯在闪
			//hal_assert(  CC2520_REGRD8( CC2520_EXCFLAG0)&0x08);//灯在闪
			//hal_assert(  CC2520_REGRD8( CC2520_EXCFLAG0)&0x10);//灯在闪
			//hal_assert(  CC2520_REGRD8( CC2520_EXCFLAG0)&0x20);//灯在闪
			//hal_assert(  CC2520_REGRD8( CC2520_EXCFLAG0)&0x40);//灯在闪
			//hal_assert(  CC2520_REGRD8( CC2520_EXCFLAG0)&0x80);//灯没闪，说明RXENABLE_ZERO
             
			//让2520有足够的时间发出去
			hal_delayms( 500);

			led_off( LED_RED);
			hal_delayms( 500);
            /*
			CC2520_SPI_BEGIN();//HAL_CLR_CC_CS_PIN();

			CC2520_SPI_TX( len-1);//_cc2420_spi_put( len-1 );

			for (i=1; i<len; i++) 
			{
				CC2520_SPI_TX( txbuf[i]);//_cc2420_spi_put( buf[i] );
			}

			CC2520_SPI_END();
			*/
		}
	
}



