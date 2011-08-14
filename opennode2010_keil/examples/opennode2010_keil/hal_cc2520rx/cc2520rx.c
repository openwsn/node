///******************************************************************************
//  Filename: rf_modem.c
//    
//  Description: This application function as a secure RF modem. It is used
//  to transfer serial data in a secure way over the air.
//    
//  Push S1 to enter the menu. The menu will prompt to choose a device number. 
//  This device number must be distinct between the two nodes to give them 
//  unique addresses in the network. Push S1 to confirm the choice.
//    
//  Configure the PC HyperTerminal for baudrate 38400 bps, 8 data bits
//  no parity, 1 stop bit, and HW flow control. 
//    
//******************************************************************************/
///*todo for testing
//#include "apl_foundation.h"
//
//
///******************************************************************************
//* CONSTANTS
//*/
///*todo for testing
//// Application parameters
//#define RF_CHANNEL                25      // 2.4 GHz RF channel
//#define APP_PAYLOAD_LENGTH        103
//
//// BasicRF address definitions
//#define PAN_ID                0x2007
//#define DEVICE_1_ADDR         0x2520
//#define DEVICE_2_ADDR         0xBEEF
//
//// Application states
//#define INIT                      0
//#define ROLE_SET                  1
//
//// Application devices
//#define NONE                      0
//#define DEVICE_1                  1
//#define DEVICE_2                  2
//#define DEVICES                   2
//
//// Application modes
//#define UART_RX_IDLE_TIME       100      // 100 ms
//#define RETRIES                  30      // Number of transmission retries
//
//#define SECURITY_MODE NONE
//
///******************************************************************************
//* LOCAL VARIABLES
//*/
///*todo for testing
//static uint8 pTxData[APP_PAYLOAD_LENGTH];
//static uint8 pRxData[APP_PAYLOAD_LENGTH];
//static uint8 appRole;
//static uint16 appRemoteAddr;
//static basicRfCfg_t basicRfConfig;
//static volatile uint8 appUartRxIdle;
//
////spi_variables
//typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
//
///* Private define ------------------------------------------------------------*/
///*todo for testing
//#define BufferSize  32
//
///* Private macro -------------------------------------------------------------*/
///* Private variables ---------------------------------------------------------*/
///*todo for testing
//SPI_InitTypeDef  SPI_InitStructure;
//u16 SPI1_Buffer_Tx[BufferSize] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
//                                  0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
//                                  0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
//                                  0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
//                                  0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
//                                  0x3D3E, 0x3F40};
//u16 SPI2_Buffer_Tx[BufferSize] = {0x5152, 0x5354, 0x5556, 0x5758, 0x595A, 0x5B5C,
//                                  0x5D5E, 0x5F60, 0x6162, 0x6364, 0x6566, 0x6768,
//                                  0x696A, 0x6B6C, 0x6D6E, 0x6F70, 0x7172, 0x7374,
//                                  0x7576, 0x7778, 0x797A, 0x7B7C, 0x7D7E, 0x7F80,
//                                  0x8182, 0x8384, 0x8586, 0x8788, 0x898A, 0x8B8C,
//                                  0x8D8E, 0x8F90};
//u16 SPI1_Buffer_Rx[BufferSize], SPI2_Buffer_Rx[BufferSize];
//u8 TxIdx = 0, RxIdx = 0;
//vu16 CRC1Value = 0, CRC2Value = 0;
//volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
//
/////////////////////////////////////////////////////////////////////////////////////////////
//
//// Security key 
//static uint8 key[]= {
//    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
//    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
//};
//
//// Mode menu
//static const char* pDeviceMenu[2] = {"Device 1", "Device 2"};
//static const uint8 deviceValues[2] = {DEVICE_1, DEVICE_2};
//
///******************************************************************************
//* LOCAL FUNCTIONS
//*/
///*todo for testing
//static void appConfigTimer(uint16 rate);
//static void appPrintMenu(void);
//static void appReceiverTask(void); 
//static void appSenderTask(void); 
//static uint8 appSelectRole(void);
//static void spi_config(void);
//
//// todo 
//#define halLedSet(n) 0
//
///******************************************************************************
//* @fn          main
//*
//* @brief       This is the main entry of the rf modem application. It sets
//*              distinct short addresses for the nodes, initalises and runs
//*              receiver and sender tasks sequentially in an endless loop
//*
//* @param       basicRfConfig - file scope variable. Basic RF configuration 
//*              data
//*              appRole - file scope variable. Is set to DEVICE_1 or DEVICE_2
//*
//* @return      none
//*/
///*todo for testing
//int main(void)
//{    
//	/*!< At this stage the microcontroller clock setting is already configured, 
//	 * this is done through SystemInit() function which is called from startup
//	 * file (startup_stm32f10x_xx.s) before to branch to application main.
//	 * To reconfigure the default setting of SystemInit() function, refer to
//	 * system_stm32f10x.c file.
//	 */
//	 /*todo for testing      
//    mcu_init();
//	
//	spi_config();
//	
//	appUartRxIdle = FALSE;
//    // Config basicRF
//    basicRfConfig.panId = PAN_ID;
//    basicRfConfig.channel = RF_CHANNEL;
//    //basicRfConfig.securityKey = key;
//    basicRfConfig.ackRequest = TRUE;
//    
//    // Initialise board peripherals
//    halBoardInit();
//    halUartInit(HAL_UART_BAUDRATE_38400, 0);
//    
//    // Initalise hal_rf
//    if(halRfInit()==FAILED) {
//      hal_assert(FALSE);
//    }
//    
//    // 100 ms timeout
//    appConfigTimer(1000/UART_RX_IDLE_TIME);
//    // appPrintMenu();
//    // utilPrintLogo("Secure Modem");
//    
//    // Indicate that device is powered
//    halLedSet(1);
//    
//    // Wait for user to press S1 to enter menu
//    // while (!BUTTON_1_PUSHED());
//    halMcuWaitMs(350);
//    //halLcdClear();
//    
//    // Set application role
//    appRole = appSelectRole();
//    //halLcdClear();
//    
//    // Set distinct addresses  
//    if(appRole == DEVICE_1) {  
//        basicRfConfig.myAddr = DEVICE_1_ADDR;
//        //printStr("Role: Device 1\n");
//        //halLcdWriteLine(HAL_LCD_LINE_2, "Device 1"); 
//        appRemoteAddr=DEVICE_2_ADDR;
//    } else if (appRole == DEVICE_2) {
//        basicRfConfig.myAddr = DEVICE_2_ADDR;
//        //printStr("Role: Device 2\n");
//        //halLcdWriteLine(HAL_LCD_LINE_2, "Device 2"); 
//        appRemoteAddr=DEVICE_1_ADDR;
//    }
//    else {
//        // Role is undefined. This code should never be reached
//        hal_assert(FALSE);
//    }
//    //halLcdWriteLine(HAL_LCD_LINE_1, "Secure Modem"); 
//    //halLcdWriteLine(HAL_LCD_LINE_3, "Ready"); 
//    //printStr("Ready\n");
//    
//    // Initialize BasicRF
//    if(basicRfInit(&basicRfConfig)==FAILED) {
//      hal_assert(FALSE);
//    }
//    basicRfReceiveOn();
///*    
//    // Config and start Timer
//    halTimer32kIntEnable();
//*/    
//    // No return from here. 
//    // Run appReceiverTask() and appSenderTask() sequentially. 
//	/*todo for testing
//    while(TRUE) {
//        appReceiverTask();
//        appSenderTask();
//    }
//
//	return 0;
//}
//
///***********************************************************************************
//* @fn          appTimerISR
//*
//* @brief       32KHz timer interrupt service routine. Signals UART RX timeout to 
//*              appSenderTask
//*
//* @param       none
//*
//* @return      none
//*/
///*todo for testing
//static void appTimerISR(void)
//{
//    appUartRxIdle = TRUE;
//}
//
//
///***********************************************************************************
//* @fn          appConfigTimer
//*
//* @brief       Configure timer interrupts for application. Uses 32 KHz timer 
//*
//* @param       uint16 period - Frequency of timer interrupt. This value must be 
//*              between 1 and 32768 Hz
//*
//* @return      none
//*/
///*todo for testing
//static void appConfigTimer(uint16 rate)
//{/*
//    halTimer32kInit(TIMER_32K_CLK_FREQ/rate);
//    halTimer32kIntConnect(&appTimerISR);				*/
//	/*todo for testing
//}
//
//
///******************************************************************************
//* @fn          appPrintMenu
//*
//* @brief       Prints menu on UART 
//*
//* @param       none
//*
//* @return      none
//*/
///*todo for testing
//static void appPrintMenu(void)
//{
//    printStr("--------------------------\n");
//    printStr("CC2520 Application Example\n");
//    printStr("Secure RF Modem\n");
//    printStr("File transfer\n");
//    printStr("--------------------------\n\n");
//}
//
//
//
///******************************************************************************
//* @fn          appReceiverTask
//*
//* @brief       Check if a new packet is received from the air. If a new packet
//*              is received the payload is sent out to the UART. 
//*
//* @param       none
//*
//* @return      none
//*/
///*todo for testing
//static void appReceiverTask(void) 
//{
//    uint8 numBytes=0;
//    
//    if(basicRfPacketIsReady()) {
//        
//        // Signal PC not to send on UART
//        //halUartEnableRxFlow(FALSE);
//        // Wait for PC to respond
//        //halMcuWaitUs(1000);
//        
//        // Receive packet from air
//        numBytes = basicRfReceive(pRxData, APP_PAYLOAD_LENGTH, NULL);
//		
//		// todo: suggest to indicate whether frame received by control the LED
//        
//        // If reception successful, send packet to UART
//        /*if(numBytes>0) {
//            // Send bytes to UART
//            for(int i=0;i<numBytes;i++) {
//                halUartBufferedWrite(&pRxData[i],1);
//            }
//        }*/
//        // Signal RX flow on
//        //halUartEnableRxFlow(TRUE);
//		/*todo for testing
//    }
//}
//
//
///******************************************************************************
//* @fn          appSenderTask
//*
//* @brief       Checks if there are new bytes arrived from the UART. If there
//*              are enough bytes to fill a maximal sized packet, or if UART 
//*              is idled the arrived bytes are sent out on air. 
//*
//* @param       none
//*
//* @return      none
//*/
///*todo for testing
//static void appSenderTask(void) 
//{  /*
//    uint8 ch=0;
//    uint8 numBytes=0;
//    uint8 payloadLength=0;
//    uint8 bytesToRead=0;
//	
//	pTxData[i]=0x12; //just send a bytesize data:0x12
//	payloadLength=1;
//        
//    for(int i=0; i<RETRIES;i++) 
//	{
//		if( (basicRfSendPacket(appRemoteAddr, pTxData, payloadLength)) == SUCCESS) 
//		{
//			break;
//        } 
//		else {
//			// wait random time if sending is not successful
//            halMcuWaitUs( (20000/255*halRfGetRandomByte()) + 20000 ); // From 20000 to
//            // 40000 us random delay
//        }    
//	}
//	*/
///*		
//    // Restart idle timer
//    halTimer32kRestart();
//    halTimer32kIntEnable();
//    // Reset idle fimer flag
//    appUartRxIdle = FALSE;
//*/	
//    /*
//    //numBytes = halUartGetNumRxBytes();
//    
//    //if(numBytes >= APP_PAYLOAD_LENGTH || (appUartRxIdle && numBytes>0) ) {
//        // Signal PC not to send on UART, while sending on air. (basicRfSendPacket
//        // disables interupt)
//        //halUartEnableRxFlow(FALSE);
//        // Wait for PC to respond
//        //halMcuWaitUs(1000);
//        
//        //bytesToRead = min(numBytes, APP_PAYLOAD_LENGTH);
//        for(int i=0;i<bytesToRead;i++) {
//            if(halUartRead(&ch,1) == 1) {
//                pTxData[i]= ch;
//                payloadLength++;
//            }
//        } 
//		pTxData[i]=0x12; //just send a bytesize data:0x12
//		payloadLength=1;
//        
//        for(int i=0; i<RETRIES;i++) {
//            if( (basicRfSendPacket(appRemoteAddr, pTxData, payloadLength)) == SUCCESS) {
//                break;
//            } else {
//                // wait random time if sending is not successful
//                halMcuWaitUs( (20000/255*halRfGetRandomByte()) + 20000 ); // From 20000 to
//                // 40000 us random delay
//            }    
//        }
//        
//        // Signal RX flow on
//        //halUartEnableRxFlow(TRUE);
//	
//        // Restart idle timer
//        // halTimer32kRestart();
//        // halTimer32kIntEnable();
//        // Reset idle fimer flag
//        // appUartRxIdle = FALSE;
//    */
//	/*todo for testing
//}
//
//
///***********************************************************************************
//* @fn          appSelectRole
//*
//* @brief       Select application role
//*
//* @param       none
//*
//* @return      uint8 - Application role chosen
//*/
///*todo for testing
//static uint8 appSelectRole(void)
//{
//    uint8 index;
//    //halLcdWriteLine(1, "Device Role: ");
//    
//    //index = utilMenuSelect(pDeviceMenu, DEVICES);
//    //return deviceValues[index];
//	return 1;
//}
//
//void spi_config(void)
//{
//
//  SPI_I2S_DeInit(SPI1);//Reset SPI2
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//
//RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
//
//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
//
//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//
//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//
//GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//
//  SPI_Cmd(SPI1, DISABLE);
//
//	/* SPI1 configuration ------------------------------------------------------*/
//	/*todo for testing
//  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
//  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
//  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
//  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//  SPI_InitStructure.SPI_CRCPolynomial = 7;
//  SPI_Init(SPI1, &SPI_InitStructure);
//
//  /* SPI2 configuration ------------------------------------------------------*/
//  //SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
//  //SPI_Init(SPI2, &SPI_InitStructure);
//
//  /* Enable SPI1 CRC calculation */
//  /*todo for testing
//  SPI_CalculateCRC(SPI1, ENABLE);
//  /* Enable SPI2 CRC calculation */
//  //SPI_CalculateCRC(SPI2, ENABLE);
//
//  /* Enable SPI1 */
//  /*todo for testing
//  SPI_Cmd(SPI1, ENABLE);
//  /* Enable SPI2 */
//  //SPI_Cmd(SPI2, ENABLE);
//
//  /* Transfer procedure */
//    /* Wait for SPI1 Tx buffer empty */
//    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//    /* Send SPI2 data */
//    //SPI_I2S_SendData(SPI2, SPI2_Buffer_Tx[TxIdx]);
//    /* Send SPI1 data */
//    //SPI_I2S_SendData(SPI1, 0x1234);
//    /* Wait for SPI2 data reception */
//    //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//    /* Read SPI2 received data */
//    //SPI2_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI2);
//    /* Wait for SPI1 data reception */
//    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//    /* Read SPI1 received data */
//    //SPI1_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI1);
//  
//
//  /* Wait for SPI1 Tx buffer empty */
//  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
//  /* Wait for SPI2 Tx buffer empty */
//  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
//
//  /* Send last SPI2_Buffer_Tx data */
//  //SPI_I2S_SendData(SPI2, SPI2_Buffer_Tx[TxIdx]);
//  /* Enable SPI2 CRC transmission */
//  //SPI_TransmitCRC(SPI2);
//  /* Send last SPI1_Buffer_Tx data */
//  //SPI_I2S_SendData(SPI1, SPI1_Buffer_Tx[TxIdx]);
//  /* Enable SPI1 CRC transmission */
//  //SPI_TransmitCRC(SPI1);
//
//  /* Wait for SPI1 last data reception */
//  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//  /* Read SPI1 last received data */
//  //SPI1_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI1);
//
//  /* Wait for SPI2 last data reception */
//  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//  /* Read SPI2 last received data */
//  //SPI2_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI2);
//
//  /* Wait for SPI1 data reception: CRC transmitted by SPI2 */
//  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//  /* Wait for SPI2 data reception: CRC transmitted by SPI1 */
//  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
//
//  /* Check the received data with the send ones */
//  //TransferStatus1 = Buffercmp(SPI2_Buffer_Rx, SPI1_Buffer_Tx, BufferSize);
//  //TransferStatus2 = Buffercmp(SPI1_Buffer_Rx, SPI2_Buffer_Tx, BufferSize);
//  /* TransferStatus1, TransferStatus2 = PASSED, if the data transmitted and received
//     are correct */
//  /* TransferStatus1, TransferStatus2 = FAILED, if the data transmitted and received
//     are different */
//
//  /* Test on the SPI1 CRC Error flag */
//  //if ((SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_CRCERR)) == SET)
//  //{
//    //TransferStatus2 = FAILED;
//  //}
//
//  /* Test on the SPI2 CRC Error flag */
//  //if ((SPI_I2S_GetFlagStatus(SPI2, SPI_FLAG_CRCERR)) == SET)
//  //{
//    //TransferStatus1 = FAILED;
//  //}
//
//  /* Read SPI1 received CRC value */
//  //CRC1Value = SPI_I2S_ReceiveData(SPI1);
//  /* Read SPI2 received CRC value */
//  //CRC2Value = SPI_I2S_ReceiveData(SPI2);
//  /*todo for testing
//
//}
//todo for testing*/
//
//
///********************************************************************************
//*********************************************************************************/
//
//#include "apl_foundation.h"
//#include "../../common/openwsn/rtl/rtl_frame.h"
//#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"
////#include "../../common/openwsn/hal/opennode2010/hal_led.h"
//
//USART_InitTypeDef USART_InitStructure;
//
//#define MAX_IEEE802FRAME154_SIZE                128//todo
//
//#define FAILED 1
//#define RXFIFO_START 0x180
//#define  channel 11
//
//#define GPIO_SPI GPIOB
//#define SPI_pin_MISO  GPIO_Pin_14
//#define SPI_pin_MOSI  GPIO_Pin_15
//#define SPI_pin_SCK   GPIO_Pin_13
//#define SPI_pin_SS    GPIO_Pin_12
//
//
///***********************************************************************************
// * LOCAL FUNCTIONS
// */
//
//static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
//TiIEEE802Frame154Descriptor m_desc;
//
//void RCC_Configuration(void);
//static void CC2520_Activate( void);
//static void SPI_GPIO_Configuration( void);
//static void USART_GPIO_Configuration( void);
//static uint8 USART_Send( uint8 ch);
//
//
//void RCC_Configuration(void)
//{
//	RCC_PCLK2Config(RCC_HCLK_Div8);
//	RCC_PCLK1Config(RCC_HCLK_Div8);
//
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//
//}
//
//static void SPI_GPIO_Configuration( void)
//{
//	GPIO_InitStructure.GPIO_Pin = SPI_pin_MOSI|SPI_pin_SCK;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init( GPIO_SPI,&GPIO_InitStructure);
//
//	GPIO_InitStructure.GPIO_Pin = SPI_pin_MISO;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init( GPIO_SPI,&GPIO_InitStructure);
//
//	GPIO_InitStructure.GPIO_Pin = SPI_pin_SS;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init( GPIO_SPI,&GPIO_InitStructure);
//}
//
//static void USART_GPIO_Configuration( void)
//{
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//
//	// Configure USART2 Rx (PA.3) as input floating 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//}
//
//static void CC2520_Activate( void)
//{
//	int i;
//    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
////   /*
////	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
////	GPIO_Init(GPIOA, &GPIO_InitStructure);
////    */
//	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
//	GPIO_Init( GPIOB,&GPIO_InitStructure);
//	
//
//	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init( GPIOB,&GPIO_InitStructure);
//
//	//GPIO_ResetBits( GPIOA,GPIO_Pin_8);
//
//	/*********************************************************/
//	//GPIO_ResetBits( GPIOB,GPIO_Pin_5);//reset the VREG_EN
//	//GPIO_SetBits( GPIOB,GPIO_Pin_5);//set the VREG_EN
//	//GPIO_ResetBits( GPIOB,GPIO_Pin_5);//reset the VREG_EN
//	//GPIO_SetBits( GPIOB,GPIO_Pin_1);////set the cc2520 nRST
//	//GPIO_ResetBits( GPIOB,GPIO_Pin_1)//reset the cc2520 nRST
//    //GPIO_SetBits( GPIOB,GPIO_Pin_12);//set the cc2520 CSn
//	//GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
//	//GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14);//read the input of the SO.
//    /************************************************************************/
//    GPIO_ResetBits( GPIOB,GPIO_Pin_1);//reset the cc2520 nRST
//	GPIO_SetBits( GPIOB,GPIO_Pin_5);//set the VREG_EN
//	for ( i=0;i<13500;i++);//wait for the regulator to be stabe.
//
//	GPIO_SetBits( GPIOB,GPIO_Pin_1);////set the cc2520 nRST
//	GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
//	for ( i=0;i<13500;i++);//wait for the output of SO to be 1//todo for testing
//	hal_assert( GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14));//todo该语句报错，可能是因为SO引脚的 输出模式改变的原
//	GPIO_SetBits( GPIOB,GPIO_Pin_12);//set the cc2520 CSn
//	hal_delayus( 2 );
//}
//
//
//static uint8 USART_Send( uint8 ch)
//{
//	USART_SendData( USART2,ch);
//	while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
//	{
//	}
//}
//
//
//
///***********************************************************************************
// * @fn          main
// *
// * @brief       This is the main entry of the "Read Regs" application
// *
// * @param       none
// *
// * @return      none
// */
//
//void main (void)
//{
//	int i;
//    uint16 g;
//	uint8 len;
//	uint8 state;
//	uint8 k;
//	uint8 data[40];
//	TiFrame *rxbuf;//todo
//	char * ptr;//todo
//	TiIEEE802Frame154Descriptor * desc;//todo
//    g = 0x01;
//	state = 0x00;
//	len = 0x00;
//    RCC_Configuration( );
//    led_open();
//    led_off( LED_RED);//todo 下面三句的顺序不能变
//    CC2520_Activate();
//	SPI_GPIO_Configuration();
//	CC2520_SPI_OPEN( );
//	//CC2520_SPI_BEGIN();
//    USART_GPIO_Configuration();
//
//	/***********************************************/
//	USART_InitStructure.USART_BaudRate = 9600;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//
//	USART_Init( USART2,&USART_InitStructure);
//	USART_Cmd( USART2,ENABLE);
//	/***********************************************/
//    //现在还缺少相应寄存器的配置;这可能是导致发送失败的原因之一
//    halRfInit();//todo 设置相应的寄存器
//
//	halRfSetPower( TXPOWER_4_DBM);
//	halRfSetChannel( channel);
//	halRfSetShortAddr( 0x02);
//	halRfSetPanId( 0x01);
//	//desc = ieee802frame154_open( &m_desc );
//	//rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
//
//	CC2520_SRFOFF();
//	CC2520_SRXON();
//	while (1)//todo for testing
//	{
//		hal_delayus( 2);
//        state = CC2520_SNOP();
//		//len = CC2520_RXBUF8();//从fifo中读取字节的长度
//		//USART_Send( state);
//		//串口显示state = 0x20，即rx_underflow exception产生
//		//有时显示state = 0x30,即 rx_underflow 及tx_overflow都产生了
////		USART_Send( 0xff);
////		//USART_Send( state);CC2520_FSCAL1
////		USART_Send( CC2520_MEMRD8( CC2520_FRMCTRL0));
////		USART_Send( CC2520_MEMRD8( CC2520_FSCAL1));
////		USART_Send( CC2520_MEMRD8( CC2520_TXPOWER));
////		state = CC2520_MEMRD8( CC2520_CHIPID);
////		USART_Send( CC2520_REGRD8( CC2520_EXCFLAG0));
////		USART_Send( CC2520_REGRD8( CC2520_FRMCTRL0));
////		USART_Send( CC2520_REGRD8( CC2520_EXCFLAG1));//一开始输出结果是06，即SRC_MATCH_DONE exception和
////		//RX_FRM_ACCEPTED exception产生，后来变成0x26即又增加了SFD excepyion;0x26符合逻辑但是没有node发送frame，为什么会有这几个exception呢？
////		USART_Send( CC2520_REGRD8( CC2520_EXCFLAG2));
////		USART_Send( CC2520_REGRD8( CC2520_RXFIFOCNT));//输出结果是0说明rxfifo中根本没有数据,但是为什么CC2520_EXCFLAG1不是0呢？
////		USART_Send( 0xff);
////        hal_delayms( 10);
////		//CC2520_SFLUSHRX();
//		//CC2520_SFLUSHRX();
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
////			for( i=0;i<20;i++)
////			{
////                USART_Send( CC2520_MEMRD8( RXFIFO_START+i));
////			}
//		}
//		USART_Send( CC2520_REGRD8( CC2520_EXCFLAG0));
//		USART_Send( CC2520_REGRD8( CC2520_EXCFLAG1));
//		USART_Send( CC2520_REGRD8( CC2520_EXCFLAG2));
//		USART_Send( CC2520_REGRD8( CC2520_RXFIFOCNT));
//		hal_delayms( 1000);
//	}
//	
//}
//
//
//

#include "apl_foundation.h"
#include "openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "openwsn/hal/hal_foundation.h"
#include "openwsn/rtl/rtl_frame.h"
#include "openwsn/hal/hal_cpu.h"
#include "openwsn/hal/hal_led.h"
#include "openwsn/hal/hal_assert.h"
#include "openwsn/hal/hal_uart.h"
#include "openwsn/hal/hal_cc2520.h"
#include "openwsn/hal/hal_debugio.h"

#define CONFIG_LISTENER    
#undef  CONFIG_LISTENER    

#define TEST_CHOICE 1
//#define TEST_ACK
//#undef  TEST_ACK


#define PANID				0x0001
#define LOCAL_ADDRESS		0x02
#define REMOTE_ADDRESS		0x01
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11
#define MAX_IEEE802FRAME154_SIZE                128

static char                 m_rxbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
void recvnode1(void);


/*
#if (TEST_CHOICE == 1)
static void recvnode1(void);
#endif

#if (TEST_CHOICE == 2)
static void recvnode2(void);
static void _cc2420_listener( void * ccptr, TiEvent * e );
#endif
*/

int main(void)
{
   recvnode1();
   /*
    #if (TEST_CHOICE == 1)
	recvnode1();
    #endif

    #if (TEST_CHOICE == 2)
	recvnode2();
    #endif
	*/
}

//#if (TEST_CHOICE == 1)
void recvnode1(void)
{
    TiCc2520Adapter * cc;
	TiFrame * rxbuf;
	uint8 len;
    uint8 i;
    char *pc;
	led_open();
	led_on( LED_RED );
	hal_delayms( 500 );
	led_off( LED_ALL );
	halUartInit(9600,0);
	cc = cc2520_construct( (void *)(&m_cc), sizeof(TiCc2520Adapter) );

	cc2520_open( cc, 0, NULL, NULL, 0x00 );
	
	cc2520_setchannel( cc, DEFAULT_CHANNEL );
	cc2520_rxon( cc );							//Enable RX
	cc2520_enable_addrdecode( cc );					//使能地址译码
	//cc2420_disable_addrdecode(cc);
	#ifdef TEST_ACK
	cc2420_enable_autoack(cc);
	#endif

	cc2520_setpanid( cc, PANID );					//网络标识
	cc2520_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	rxbuf = frame_open( (char*)(&m_rxbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

	// when use this scan mode to receive data, interrupt should be disable; otherwise the data will be
	// read twice and in the second time there are no data actually which leads to a assert.
 	// Attention: in this scan mode, MCU always try to read and in my  test it is faster than the transmission of data. 
	// Thus, after 4 times, there no data at all, and the MCU still want to read, which lead to an assert. So we'd better
	// not use this scan mode.
    
	while(1) 
	{
		frame_reset( rxbuf,0,0,0);
		
		cc2520_evolve( cc );

		len = cc2520_read( cc, frame_startptr(rxbuf), frame_capacity(rxbuf), 0x00 );
        if ( len)
        {
			frame_setlength( rxbuf,len);
            pc = frame_startptr(rxbuf);
            for ( i=0;i<len;i++)
            {
                USART_Send( pc[i]);
            }
			led_toggle( LED_RED);
        }
	}
}
