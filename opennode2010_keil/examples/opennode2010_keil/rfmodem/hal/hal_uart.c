
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
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
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
#include "hal_types.h"
#include "hal_uart.h"
#include "hal_board.h"
#include "util_buffer.h"
#include "hal_int.h"
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
    IE2 |= UCA0TXIE;
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
    IE2 &= ~UCA0RXIE;
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
    IE2 &= ~UCA0TXIE;
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
    return UCA0RXBUF;
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
void halUartInit(uint8 baudrate, uint8 options)
{
    // For the moment, this UART implementation only
    // supports communication settings 115200 8N1
    // i.e. ignore baudrate and options arguments.

    UCA0CTL1 |= UCSWRST;                   // Keep USART1 in reset state

    UCA0CTL1 |= UCSSEL1;                  // Set clock source SMCLK
    UCA0CTL1 &= ~UCSSEL0;

    P3SEL |= BIT4;                    // P3.4 = USART1 TXD
    P3SEL |= BIT5;                    // P3.5 = USART1 RXD

    switch (baudrate) {
    case HAL_UART_BAUDRATE_9600:
        UCA0BR0 = 0x41;                     // 8MHz 9600
        UCA0BR1 = 0x03;                     // 8MHz 9600
        break;

    case HAL_UART_BAUDRATE_19200:
        UCA0BR0 = 0xA0;                     // 8MHz 19200
        UCA0BR1 = 0x01;                     // 8MHz 19200
        break;

    case HAL_UART_BAUDRATE_38400:
        UCA0BR0 = 0xD0;                     // 8MHz 38400
        UCA0BR1 = 0x00;                     // 8MHz 38400
        break;

    case HAL_UART_BAUDRATE_57600:
        UCA0BR0 = 0x8A;                     // 8MHz 57600
        UCA0BR1 = 0x00;                     // 8MHz 57600
        break;

    case HAL_UART_BAUDRATE_115200:
        UCA0BR0 = 0x45;                     // 8MHz 115200
        UCA0BR1 = 0x00;                     // 8MHz 115200
        break;

    default:
        break;
    }

    UCA0CTL0 &= ~UCPEN;                   // No parity
    UCA0CTL0 &= ~UCSPB;                   // 1 stop bit
    UCA0CTL0 &= ~UC7BIT;                  // 8 data bits

    UCA0CTL1 &= ~UCSWRST;                   // Initialize USART1 state machine

    // Enable RX interrupt
    halUartRxIntEnable();

    // Set RTS pin to output
    HAL_RTS_DIR_OUT();
    // Enable RX Flow
    halUartEnableRxFlow(TRUE);

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
    uint16 i;
    for(i = 0; i < length; i++)
    {
        while(!(IFG2 & UCA0TXIFG));    // Wait for TX buffer ready to receive new byte
        UCA0TXBUF = buf[i];            // Output character
    }
    return (i+1);
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
*/
uint16 halUartBufferedWrite(const uint8* buf, uint16 length)
{
    uint16 nBytes=0;
    while ( (nBytes += bufPut(&rbTxBuf,buf,length)) == 0 )
        halMcuWaitMs(5);

    // Enable UART TX interrupt
    // UCA0TXIFG is set after Power Up Clear of MSP430
    halUartTxIntEnable();
    return nBytes;
}
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
*/
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
*/
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
    // Enable RX flow
    if(enable) {
        HAL_RTS_CLR();
    }
    // Disable RX flow
    else {
        HAL_RTS_SET();
    }
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
    uint8 ch = halUartRxGetByte();
    bufPut(&rbRxBuf,&ch,1);
    __low_power_mode_off_on_exit();
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
#pragma vector=USCIAB0TX_VECTOR
__interrupt void usciB0Tx_ISR(void)
{
    uint8 c;

    if (bufGet(&rbTxBuf,&c,1)==1) {
        UCA0TXBUF = c;
    }
    // If buffer empty, disable uart TX interrupt
    if( (bufNumBytes(&rbTxBuf)) == 0) {
        halUartTxIntDisable();
    }
    __low_power_mode_off_on_exit();
}
#endif
