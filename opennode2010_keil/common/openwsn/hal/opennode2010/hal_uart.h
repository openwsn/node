
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
  Filename:     hal_uart.h

  Description:  hal UART library header file

***********************************************************************************

#ifndef HAL_UART_H
#define HAL_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************
* INCLUDES
*
#include "hal_configall.h"
#include "hal_foundation.h"

/***********************************************************************************
 * CONSTANTS AND DEFINES
 */
/* Serial Port Baudrate Settings *
#define HAL_UART_BAUDRATE_4800        0x01
#define HAL_UART_BAUDRATE_9600        0x02
#define HAL_UART_BAUDRATE_19200       0x03
#define HAL_UART_BAUDRATE_38400       0x04
#define HAL_UART_BAUDRATE_57600       0x05
#define HAL_UART_BAUDRATE_115200      0x06

/* Stop Bits *
#define HAL_UART_ONE_STOP_BIT         0x01
#define HAL_UART_TWO_STOP_BITS        0x02

/* Parity settings *
#define HAL_UART_NO_PARITY            0x04
#define HAL_UART_EVEN_PARITY          0x08
#define HAL_UART_ODD_PARITY           0x10

/* Number of bits in data field *
#define HAL_UART_7_BIT_DATA           0x20
#define HAL_UART_8_BIT_DATA           0x40

/***********************************************************************************
 * GLOBAL FUNCTIONS
 *
void   halUartInit(uint16 baudrate, uint8 options);
uint16 halUartWrite(const uint8* buf, uint16 length);
uint16 halUartRead(uint8* buf, uint16 length);
uint16 halUartGetNumRxBytes(void);
void   halUartEnableRxFlow(uint8 enable);
uint16 halUartBufferedWrite(const uint8* buf, uint16 length);
uint8 USART_Send( uint8 ch);
uint8 USART_Get( uint8 ch);

#ifdef  __cplusplus
}
#endif

/**********************************************************************************
#endif
*/

#ifndef _HAL_UART_H_2130_
#define _HAL_UART_H_2130_
#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_device.h"

//#define CONFIG_UART_INTERRUPT_DRIVEN
//#undef  CONFIG_UART_INTERRUPT_DRIVEN

#define CONFIG_UART_TXBUFFER_SIZE 0x7F
#define CONFIG_UART_RXBUFFER_SIZE CONFIG_UART_TXBUFFER_SIZE


#ifdef __cplusplus
extern "C" {
#endif

    typedef struct{
        uint8 id;
        uint16 baudrate;
        uint8 databits;
        uint8 stopbits;
        uint8 option;
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
        volatile uint8 txlen;
        volatile uint8 txidx;
        volatile uint8 rxlen;
        volatile char txbuf[CONFIG_UART_TXBUFFER_SIZE];
        volatile char rxbuf[CONFIG_UART_RXBUFFER_SIZE];
        TiFunEventHandler listener;
        void * lisowner;
#endif
    }TiUartAdapter;

    TiUartAdapter * uart_construct( char * buf, uint16 size );
    void uart_destroy( TiUartAdapter * uart );
    TiUartAdapter * uart_open( TiUartAdapter * uart, uint8 id, uint16 baudrate, uint8 databits, uint8 stopbits, uint8 option );
    void uart_close( TiUartAdapter * uart );


    uint8 uart_getchar( TiUartAdapter * uart, char * pc );
    char uart_getchar_wait( TiUartAdapter * uart ); 
    intx uart_putchar( TiUartAdapter * uart, char ch );
    uintx uart_read( TiUartAdapter * uart, char * buf, uintx size, uint8 opt );
    uintx uart_write( TiUartAdapter * uart, char * buf, uintx len, uint8 opt ); 
    //TiBlockDeviceInterface * uart_get_blockinterface( TiUartAdapter * uart, TiBlockDeviceInterface * intf );
    //以下三个函数临时保留
    void   halUartInit(uint16 baudrate, uint8 options);//uart_open( TiUartAdapter * uart,2, uint16 baudrate, uint8 databits, uint8 stopbits, uint8 option );
    uint8 USART_Send( uint8 ch);//intx uart_putchar( TiUartAdapter * uart, char ch );
    uint8 USART_Get( uint8 ch); //uint8 uart_getchar( TiUartAdapter * uart, char * pc );


#ifdef __cplusplus
}
#endif

#endif