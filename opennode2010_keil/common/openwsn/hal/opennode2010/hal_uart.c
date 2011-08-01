
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
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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
  Filename:     hal_uart.c

  Description:  hal UART library

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_uart.h"
#include "hal_targetboard.h"
#include "util_buffer.h"
#include "hal_cpu.h"
#include "hal_mcu.h"

/***********************************************************************************
* LOCAL VARIABLES
*/
static ringBuf_t rbRxBuf;

#ifndef HAL_DIRECT_UART_TX
static ringBuf_t rbTxBuf;
#endif

/***********************************************************************************
* @fn      halUartRxIntEnable
*
* @brief   Enable UART RX interrupt
*
* @param   none
*
* @return  none
*/
static void halUartRxIntEnable(void)
{
	// todo
	/*
    IE2 |= UCA0RXIE;
	*/
}


/***********************************************************************************
* @fn      halUartTxIntEnable
*
* @brief   Enable UART TX interrupt
*
* @param   none
*
* @return  none
*/
#ifndef HAL_DIRECT_UART_TX
static void halUartTxIntEnable(void)
{
    //IE2 |= UCA0TXIE;
}
#endif


/***********************************************************************************
* @fn      halUARTRxIntDisable
*
* @brief   Disable UART RX interrupt
*
* @param   none
*
* @return  none
*/
static void halUartRxIntDisable(void)
{
    //IE2 &= ~UCA0RXIE;
}


/***********************************************************************************
* @fn      halUARTTxIntDisable
*
* @brief   Disable UART TX interrupt
*
* @param   none
*
* @return  none
*/
#ifndef HAL_DIRECT_UART_TX
static void halUartTxIntDisable(void)
{
    //IE2 &= ~UCA0TXIE;
}
#endif

/***********************************************************************************
* @fn      halUartRxGetByte
*
* @brief   Read byte from UART RX buffer
*
* @param   none
*
* @return  none
*/
static uint8 halUartRxGetByte(void)
{
    return 0;//return UCA0RXBUF; 
}


/***********************************************************************************
* @fn      halUartInit
*
* @brief   Initalise UART. Supported baudrates are: 19220, 38400, 57600 and 115200
*
* @param   uint8 baudrate
*          uint8 options - this parameter is ignored
*
* @return  none
*/
void halUartInit(uint16 baudrate, uint8 options)
{
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	// Configure USART2 Rx (PA.3) as input floating 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init( USART2,&USART_InitStructure);
	USART_Cmd( USART2,ENABLE);
}

uint8 USART_Send( uint8 ch)
{
	USART_SendData( USART2,ch);
	while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
	{
	}
}



/***********************************************************************************
* @fn      halUartWrite
*
* @brief   Write data buffer to UART
*
* @param   uint8* buf - buffer with data to write
*          uint16 length - number of bytes to write
*
* @return  uint16 - number of bytes written
*/
uint16 halUartWrite(const uint8* buf, uint16 length)
{
/*
    uint16 i;
    for(i = 0; i < length; i++)
    {
        while(!(IFG2 & UCA0TXIFG));    // Wait for TX buffer ready to receive new byte
        UCA0TXBUF = buf[i];            // Output character
    }
    return (i+1);
*/
	return 0;
}

#ifndef HAL_DIRECT_UART_TX
/***********************************************************************************
* @fn      halUartBufferedWrite
*
* @brief   Write data buffered to UART. Data is written into a buffer, and the
*          buffer is emptied by UART TX interrupt ISR
*
* @param   uint8* buf - buffer with data to write
*          uint16 length - number of bytes to write
*
* @return  none
*
uint16 halUartBufferedWrite(const uint8* buf, uint16 length)
{
    uint16 nBytes=0;
    while ( (nBytes += bufPut(&rbTxBuf,buf,length)) == 0 )
        halMcuWaitMs(5);

    // Enable UART TX interrupt
    // UCA0TXIFG is set after Power Up Clear of MSP430
    halUartTxIntEnable();
    return nBytes;
} */
#endif

/***********************************************************************************
* @fn      halUartRead
*
* @brief   Read data from UART Rx buffer
*
* @param   uint8* buf - buffer with data to read in to
*          uint16 length - number of bytes to read
*
* @return  none
*
uint16 halUartRead(uint8* buf, uint16 length)
{
    return bufGet(&rbRxBuf, (uint8 *)buf, length);
}


/***********************************************************************************
* @fn      halUartGetNumRxBytes
*
* @brief   Returns number of bytes in RX buffer
*
* @param   none
*
* @return  uint8
*
uint16 halUartGetNumRxBytes(void)
{
    return bufNumBytes(&rbRxBuf);
}


/***********************************************************************************
* @fn      halUartEnableRxFlow
*
* @brief   Signal ready/not ready to receive characters on UART
*
* @param   uint8 enable - TRUE to signal ready to receive on UART
*                         FALSE to signal not ready to receive on UART
*
* @return  none
*/
void halUartEnableRxFlow(uint8 enable)
{
/*
    // Enable RX flow
    if(enable) {
        HAL_RTS_CLR();
    }
    // Disable RX flow
    else {
        HAL_RTS_SET();
    }
*/
}


/***********************************************************************************
* @fn      usart1Rx_ISR
*
* @brief   ISR framework for the USCI A0/B0 Receive component
*
* @param   none
*
* @return  none
*/
/*
#pragma vector=USCIAB0RX_VECTOR
__interrupt void usciA0Rx_ISR(void)
*/
// todo interrupt ISR
void usciA0Rx_ISR(void)
{
/*
    uint8 ch = halUartRxGetByte();
    bufPut(&rbRxBuf,&ch,1);
    __low_power_mode_off_on_exit();
*/
}

#ifndef HAL_DIRECT_UART_TX
/***********************************************************************************
* @fn      usart1Tx_ISR
*
* @brief   ISR framework for the USCI A0/B0 transmit component
*
* @param   none
*
* @return  none
*/
//#pragma vector=USCIAB0TX_VECTOR
//__interrupt void usciB0Tx_ISR(void)
void usciB0Tx_ISR(void)
{
/*
    uint8 c;

    if (bufGet(&rbTxBuf,&c,1)==1) {
        UCA0TXBUF = c;
    }
    // If buffer empty, disable uart TX interrupt
    if( (bufNumBytes(&rbTxBuf)) == 0) {
        halUartTxIntDisable();
    }
    __low_power_mode_off_on_exit();
*/	
}

#endif
