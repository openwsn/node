/******************************************************************************
  Filename: rf_modem.c
    
  Description: This application function as a secure RF modem. It is used
  to transfer serial data in a secure way over the air.
    
  Push S1 to enter the menu. The menu will prompt to choose a device number. 
  This device number must be distinct between the two nodes to give them 
  unique addresses in the network. Push S1 to confirm the choice.
    
  Configure the PC HyperTerminal for baudrate 38400 bps, 8 data bits
  no parity, 1 stop bit, and HW flow control. 
    
******************************************************************************/

#include "configall.h"
#include "./hal/hal_uart.h"
//#include <hal_lcd.h>
//#include <hal_led.h>
//#include <hal_timer_32k.h>

#include "./hal/init.h"
#include "./hal/hal_board.h"
#include "./hal/hal_rf.h"
#include "./hal/util.h"
#include "./hal/basic_rf.h"
#include "./hal/hal_assert.h"


/******************************************************************************
* CONSTANTS
*/
// Application parameters
#define RF_CHANNEL                25      // 2.4 GHz RF channel
#define APP_PAYLOAD_LENGTH        103

// BasicRF address definitions
#define PAN_ID                0x2007
#define DEVICE_1_ADDR         0x2520
#define DEVICE_2_ADDR         0xBEEF

// Application states
#define INIT                      0
#define ROLE_SET                  1

// Application devices
#define NONE                      0
#define DEVICE_1                  1
#define DEVICE_2                  2
#define DEVICES                   2

// Application modes
#define UART_RX_IDLE_TIME       100      // 100 ms
#define RETRIES                  30      // Number of transmission retries

#define SECURITY_MODE NONE

/******************************************************************************
* LOCAL VARIABLES
*/
static uint8 pTxData[APP_PAYLOAD_LENGTH];
static uint8 pRxData[APP_PAYLOAD_LENGTH];
static uint8 appRole;
static uint16 appRemoteAddr;
static basicRfCfg_t basicRfConfig;
static volatile uint8 appUartRxIdle;

//spi_variables
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BufferSize  32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SPI_InitTypeDef  SPI_InitStructure;
u16 SPI1_Buffer_Tx[BufferSize] = {0x0102, 0x0304, 0x0506, 0x0708, 0x090A, 0x0B0C,
                                  0x0D0E, 0x0F10, 0x1112, 0x1314, 0x1516, 0x1718,
                                  0x191A, 0x1B1C, 0x1D1E, 0x1F20, 0x2122, 0x2324,
                                  0x2526, 0x2728, 0x292A, 0x2B2C, 0x2D2E, 0x2F30,
                                  0x3132, 0x3334, 0x3536, 0x3738, 0x393A, 0x3B3C,
                                  0x3D3E, 0x3F40};
u16 SPI2_Buffer_Tx[BufferSize] = {0x5152, 0x5354, 0x5556, 0x5758, 0x595A, 0x5B5C,
                                  0x5D5E, 0x5F60, 0x6162, 0x6364, 0x6566, 0x6768,
                                  0x696A, 0x6B6C, 0x6D6E, 0x6F70, 0x7172, 0x7374,
                                  0x7576, 0x7778, 0x797A, 0x7B7C, 0x7D7E, 0x7F80,
                                  0x8182, 0x8384, 0x8586, 0x8788, 0x898A, 0x8B8C,
                                  0x8D8E, 0x8F90};
u16 SPI1_Buffer_Rx[BufferSize], SPI2_Buffer_Rx[BufferSize];
u8 TxIdx = 0, RxIdx = 0;
vu16 CRC1Value = 0, CRC2Value = 0;
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED;
ErrorStatus HSEStartUpStatus;
///////////////////////////////////////////////////////////////////////////////////////////

// Security key 
static uint8 key[]= {
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
};

// Mode menu
static const char* pDeviceMenu[2] = {"Device 1", "Device 2"};
static const uint8 deviceValues[2] = {DEVICE_1, DEVICE_2};

/******************************************************************************
* LOCAL FUNCTIONS
*/
static void appConfigTimer(uint16 rate);
static void appPrintMenu(void);
static void appReceiverTask(void); 
static void appSenderTask(void); 
static uint8 appSelectRole(void);
static void spi_config(void);

/******************************************************************************
* @fn          main
*
* @brief       This is the main entry of the rf modem application. It sets
*              distinct short addresses for the nodes, initalises and runs
*              receiver and sender tasks sequentially in an endless loop
*
* @param       basicRfConfig - file scope variable. Basic RF configuration 
*              data
*              appRole - file scope variable. Is set to DEVICE_1 or DEVICE_2
*
* @return      none
*/
void main (void)
{    
    
    BSP_Init();	//开发板初始化函数
	spi_config();
	
	appUartRxIdle = FALSE;
    // Config basicRF
    basicRfConfig.panId = PAN_ID;
    basicRfConfig.channel = RF_CHANNEL;
    basicRfConfig.securityKey = key;
    basicRfConfig.ackRequest = TRUE;
    
    // Initialise board peripherals
    halBoardInit();
    halUartInit(HAL_UART_BAUDRATE_38400, 0);
    
    // Initalise hal_rf
    if(halRfInit()==FAILED) {
      HAL_ASSERT(FALSE);
    }
    
    // 100 ms timeout
    appConfigTimer(1000/UART_RX_IDLE_TIME);
    // appPrintMenu();
    // utilPrintLogo("Secure Modem");
    
    // Indicate that device is powered
    halLedSet(1);
    
    // Wait for user to press S1 to enter menu
    // while (!BUTTON_1_PUSHED());
    halMcuWaitMs(350);
    //halLcdClear();
    
    // Set application role
    appRole = appSelectRole();
    //halLcdClear();
    
    // Set distinct addresses  
    if(appRole == DEVICE_1) {  
        basicRfConfig.myAddr = DEVICE_1_ADDR;
        //printStr("Role: Device 1\n");
        //halLcdWriteLine(HAL_LCD_LINE_2, "Device 1"); 
        appRemoteAddr=DEVICE_2_ADDR;
    } else if (appRole == DEVICE_2) {
        basicRfConfig.myAddr = DEVICE_2_ADDR;
        //printStr("Role: Device 2\n");
        //halLcdWriteLine(HAL_LCD_LINE_2, "Device 2"); 
        appRemoteAddr=DEVICE_1_ADDR;
    }
    else {
        // Role is undefined. This code should never be reached
        HAL_ASSERT(FALSE);
    }
    //halLcdWriteLine(HAL_LCD_LINE_1, "Secure Modem"); 
    //halLcdWriteLine(HAL_LCD_LINE_3, "Ready"); 
    //printStr("Ready\n");
    
    // Initialize BasicRF
    if(basicRfInit(&basicRfConfig)==FAILED) {
      HAL_ASSERT(FALSE);
    }
    basicRfReceiveOn();
    
    // Config and start Timer
    halTimer32kIntEnable();
    
    // No return from here. 
    // Run appReceiverTask() and appSenderTask() sequentially. 
    while(TRUE) {
        appReceiverTask();
        appSenderTask();
    }
}

/***********************************************************************************
* @fn          appTimerISR
*
* @brief       32KHz timer interrupt service routine. Signals UART RX timeout to 
*              appSenderTask
*
* @param       none
*
* @return      none
*/
static void appTimerISR(void)
{
    appUartRxIdle = TRUE;
}


/***********************************************************************************
* @fn          appConfigTimer
*
* @brief       Configure timer interrupts for application. Uses 32 KHz timer 
*
* @param       uint16 period - Frequency of timer interrupt. This value must be 
*              between 1 and 32768 Hz
*
* @return      none
*/
static void appConfigTimer(uint16 rate)
{
    halTimer32kInit(TIMER_32K_CLK_FREQ/rate);
    halTimer32kIntConnect(&appTimerISR);
}


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
    printStr("--------------------------\n");
    printStr("CC2520 Application Example\n");
    printStr("Secure RF Modem\n");
    printStr("File transfer\n");
    printStr("--------------------------\n\n");
}



/******************************************************************************
* @fn          appReceiverTask
*
* @brief       Check if a new packet is received from the air. If a new packet
*              is received the payload is sent out to the UART. 
*
* @param       none
*
* @return      none
*/
static void appReceiverTask(void) 
{
    uint8 numBytes=0;
    
    if(basicRfPacketIsReady()) {
        
        // Signal PC not to send on UART
        //halUartEnableRxFlow(FALSE);
        // Wait for PC to respond
        //halMcuWaitUs(1000);
        
        // Receive packet from air
        numBytes = basicRfReceive(pRxData, APP_PAYLOAD_LENGTH, NULL);
		
		// todo: suggest to indicate whether frame received by control the LED
        
        // If reception successful, send packet to UART
        /*if(numBytes>0) {
            // Send bytes to UART
            for(int i=0;i<numBytes;i++) {
                halUartBufferedWrite(&pRxData[i],1);
            }
        }*/
        // Signal RX flow on
        //halUartEnableRxFlow(TRUE);
    }
}


/******************************************************************************
* @fn          appSenderTask
*
* @brief       Checks if there are new bytes arrived from the UART. If there
*              are enough bytes to fill a maximal sized packet, or if UART 
*              is idled the arrived bytes are sent out on air. 
*
* @param       none
*
* @return      none
*/
static void appSenderTask(void) 
{
    uint8 ch=0;
    uint8 numBytes=0;
    uint8 payloadLength=0;
    uint8 bytesToRead=0;
	
	pTxData[i]=0x12; //just send a bytesize data:0x12
	payloadLength=1;
        
    for(int i=0; i<RETRIES;i++) 
	{
		if( (basicRfSendPacket(appRemoteAddr, pTxData, payloadLength)) == SUCCESS) 
		{
			break;
        } 
		else {
			// wait random time if sending is not successful
            halMcuWaitUs( (20000/255*halRfGetRandomByte()) + 20000 ); // From 20000 to
            // 40000 us random delay
        }    
	}
		
    // Restart idle timer
    halTimer32kRestart();
    halTimer32kIntEnable();
    // Reset idle fimer flag
    appUartRxIdle = FALSE;
	
    /*
    //numBytes = halUartGetNumRxBytes();
    
    //if(numBytes >= APP_PAYLOAD_LENGTH || (appUartRxIdle && numBytes>0) ) {
        // Signal PC not to send on UART, while sending on air. (basicRfSendPacket
        // disables interupt)
        //halUartEnableRxFlow(FALSE);
        // Wait for PC to respond
        //halMcuWaitUs(1000);
        
        //bytesToRead = min(numBytes, APP_PAYLOAD_LENGTH);
        for(int i=0;i<bytesToRead;i++) {
            if(halUartRead(&ch,1) == 1) {
                pTxData[i]= ch;
                payloadLength++;
            }
        } 
		pTxData[i]=0x12; //just send a bytesize data:0x12
		payloadLength=1;
        
        for(int i=0; i<RETRIES;i++) {
            if( (basicRfSendPacket(appRemoteAddr, pTxData, payloadLength)) == SUCCESS) {
                break;
            } else {
                // wait random time if sending is not successful
                halMcuWaitUs( (20000/255*halRfGetRandomByte()) + 20000 ); // From 20000 to
                // 40000 us random delay
            }    
        }
        
        // Signal RX flow on
        //halUartEnableRxFlow(TRUE);
	
        // Restart idle timer
        // halTimer32kRestart();
        // halTimer32kIntEnable();
        // Reset idle fimer flag
        // appUartRxIdle = FALSE;
    */
}


/***********************************************************************************
* @fn          appSelectRole
*
* @brief       Select application role
*
* @param       none
*
* @return      uint8 - Application role chosen
*/
static uint8 appSelectRole(void)
{
    uint8 index;
    //halLcdWriteLine(1, "Device Role: ");
    
    //index = utilMenuSelect(pDeviceMenu, DEVICES);
    //return deviceValues[index];
	return 1;
}

void spi_config(void)
{

  SPI_I2S_DeInit(SPI1);//Reset SPI2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;

GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

GPIO_Init(GPIOA, &GPIO_InitStructure);


  SPI_Cmd(SPI1, DISABLE);

	/* SPI1 configuration ------------------------------------------------------*/
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* SPI2 configuration ------------------------------------------------------*/
  //SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  //SPI_Init(SPI2, &SPI_InitStructure);

  /* Enable SPI1 CRC calculation */
  SPI_CalculateCRC(SPI1, ENABLE);
  /* Enable SPI2 CRC calculation */
  //SPI_CalculateCRC(SPI2, ENABLE);

  /* Enable SPI1 */
  SPI_Cmd(SPI1, ENABLE);
  /* Enable SPI2 */
  //SPI_Cmd(SPI2, ENABLE);

  /* Transfer procedure */
    /* Wait for SPI1 Tx buffer empty */
    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send SPI2 data */
    //SPI_I2S_SendData(SPI2, SPI2_Buffer_Tx[TxIdx]);
    /* Send SPI1 data */
    //SPI_I2S_SendData(SPI1, 0x1234);
    /* Wait for SPI2 data reception */
    //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    /* Read SPI2 received data */
    //SPI2_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI2);
    /* Wait for SPI1 data reception */
    //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* Read SPI1 received data */
    //SPI1_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI1);
  

  /* Wait for SPI1 Tx buffer empty */
  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  /* Wait for SPI2 Tx buffer empty */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send last SPI2_Buffer_Tx data */
  //SPI_I2S_SendData(SPI2, SPI2_Buffer_Tx[TxIdx]);
  /* Enable SPI2 CRC transmission */
  //SPI_TransmitCRC(SPI2);
  /* Send last SPI1_Buffer_Tx data */
  //SPI_I2S_SendData(SPI1, SPI1_Buffer_Tx[TxIdx]);
  /* Enable SPI1 CRC transmission */
  //SPI_TransmitCRC(SPI1);

  /* Wait for SPI1 last data reception */
  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  /* Read SPI1 last received data */
  //SPI1_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI1);

  /* Wait for SPI2 last data reception */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
  /* Read SPI2 last received data */
  //SPI2_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI2);

  /* Wait for SPI1 data reception: CRC transmitted by SPI2 */
  //while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  /* Wait for SPI2 data reception: CRC transmitted by SPI1 */
  //while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Check the received data with the send ones */
  //TransferStatus1 = Buffercmp(SPI2_Buffer_Rx, SPI1_Buffer_Tx, BufferSize);
  //TransferStatus2 = Buffercmp(SPI1_Buffer_Rx, SPI2_Buffer_Tx, BufferSize);
  /* TransferStatus1, TransferStatus2 = PASSED, if the data transmitted and received
     are correct */
  /* TransferStatus1, TransferStatus2 = FAILED, if the data transmitted and received
     are different */

  /* Test on the SPI1 CRC Error flag */
  //if ((SPI_I2S_GetFlagStatus(SPI1, SPI_FLAG_CRCERR)) == SET)
  //{
    //TransferStatus2 = FAILED;
  //}

  /* Test on the SPI2 CRC Error flag */
  //if ((SPI_I2S_GetFlagStatus(SPI2, SPI_FLAG_CRCERR)) == SET)
  //{
    //TransferStatus1 = FAILED;
  //}

  /* Read SPI1 received CRC value */
  //CRC1Value = SPI_I2S_ReceiveData(SPI1);
  /* Read SPI2 received CRC value */
  //CRC2Value = SPI_I2S_ReceiveData(SPI2);

}
