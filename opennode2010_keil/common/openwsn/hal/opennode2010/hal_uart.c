
/*
*目前用的都是查询方式，中断方式还没有解决
*/

#include "../hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_device.h"
#include "../hal_targetboard.h"
#include "../hal_led.h"
#include "../hal_assert.h"
#include "../hal_cpu.h"
#include "../hal_mcu.h"
#include "../hal_uart.h"



#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_rx0_interrupt_handler( void * uartptr, TiEvent * e );
void _uart_tx0_interrupt_handler( void * uartptr, TiEvent * e );
void _uart_rx1_interrupt_handler( void * uartptr, TiEvent * e );
void _uart_tx1_interrupt_handler( void * uartptr, TiEvent * e );
#endif


TiUartAdapter * uart_construct( char * buf, uint16 size )
{
    hal_assert( sizeof(TiUartAdapter) <= size );
    memset( buf, 0x00, size );
    return (TiUartAdapter *)buf;
}

void uart_destroy( TiUartAdapter * uart )
{
    uart_close( uart );
}

TiUartAdapter * uart_open( TiUartAdapter * uart, uint8 id, uint16 baudrate, uint8 databits, uint8 stopbits, uint8 option )
{
    USART_InitTypeDef USART_InitStructure;
    //uint8 tmpn;

	uart->id = id;
	uart->baudrate = baudrate;
	uart->databits = databits;
	uart->stopbits = stopbits;
	uart->option = option;

    #ifdef CONFIG_UART_INTERRUPT_DRIVEN
    uart->txlen = 0;
	uart->txidx = 0;
    uart->rxlen = 0;
    uart->listener = NULL;
	uart->lisowner = NULL;
    #endif

	switch (uart->id)
	{
	    case 1:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOA, &GPIO_InitStructure);


            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            USART_InitStructure.USART_BaudRate = uart->baudrate;//baudrate;
            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
            USART_InitStructure.USART_StopBits = USART_StopBits_1;
            USART_InitStructure.USART_Parity = USART_Parity_No;
            USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

            USART_Init( USART1,&USART_InitStructure);
            USART_Cmd( USART1,ENABLE);
		    break;
	    case 2:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOA, &GPIO_InitStructure);


            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            USART_InitStructure.USART_BaudRate = uart->baudrate;//baudrate;
            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
            USART_InitStructure.USART_StopBits = USART_StopBits_1;
            USART_InitStructure.USART_Parity = USART_Parity_No;
            USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

            USART_Init( USART2,&USART_InitStructure);
            USART_Cmd( USART2,ENABLE);
            break;
        case 3:
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOA, &GPIO_InitStructure);


            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOA, &GPIO_InitStructure);

            USART_InitStructure.USART_BaudRate = uart->baudrate;//baudrate;
            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
            USART_InitStructure.USART_StopBits = USART_StopBits_1;
            USART_InitStructure.USART_Parity = USART_Parity_No;
            USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
            USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

            USART_Init( USART3,&USART_InitStructure);
            USART_Cmd( USART3,ENABLE);
		    break;
	    default:
		    uart = NULL;
		    break;
	}

	return uart;
}

void uart_close( TiUartAdapter * uart )
{
    
}

uint8 uart_getchar( TiUartAdapter * uart, char * pc )
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    intx ret = -1;
    hal_atomic_begin();
    if (uart->rxlen > 0)
    {
        *pc = uart->rxbuf[0];
        uart->rxlen --;
        memmove( (void *)&(uart->rxbuf[0]), (void *)&(uart->rxbuf[1]), uart->rxlen );
        ret = 0;
    }
    hal_atomic_end();
    return ret;
#endif

#ifndef CONFIG_UART_INTERRUPT_DRIVEN
    uint8 ret=0;

    switch (uart->id)
    {
        case 1:
            if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
            {
                *pc = (USART_ReceiveData(USART1) & 0xFF); 
                ret = 1;
            }
            else
            {
                ret = 0;
            }
            break;


        case 2:
            if ( USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
            {
                *pc = (USART_ReceiveData(USART2) & 0xFF); 
                ret = 1;
            }
            else
            {
                ret = 0;
            }
            break;
            
        case 3:
            if ( USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET)
            {
                *pc = (USART_ReceiveData(USART3) & 0xFF); 
                ret = 1;
            }
            else
            {
                ret = 0;
            }
            break;

    default:
        ret = 0;
    }

    return ret;
#endif
}

char uart_getchar_wait( TiUartAdapter * uart )   
{
    #ifdef CONFIG_UART_INTERRUPT_DRIVEN
        char ch=0;
        while (uart->rxlen <= 0) {};
        uart_getchar( uart, &ch );
        return ch;
    #endif

    #ifndef CONFIG_UART_INTERRUPT_DRIVEN

        switch (uart->id)
        {
        case 1:
            while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
            {
            }
            break;
        case 2:
            while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
            {
            }
            break;
        case 3:
            while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
            {
            }
            break;
        }
    #endif

}

intx uart_putchar( TiUartAdapter * uart, char ch )
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    intx ret;

    // put the character to be send in the internal buffer "txbuf"
    //
    hal_atomic_begin();
    ret = CONFIG_UART_TXBUFFER_SIZE - uart->txlen;
    if (ret > 0)
    {
        uart->txbuf[uart->txlen] = ch;
        uart->txlen ++;
    }

    // if the background ISR sending doesn't active, then enable and trigger this interrupt
    //
    if (uart->txidx == 0)
    {
        switch (uart->id)
        {
        case 0:
            while (!(UCSR0A & (1<<UDRE0))) {};      // check whether UDR0 is available for the 
            // new character to be sent
            UCSR0B |= (1 << UDRIE);                 // enable sending interrupt. the ISR will
            // continue sending until uart->txbuf is emtpy
            break;
        case 1:
            UCSR1B |= (1 << UDRIE);
            break;
        }
    }	
    hal_atomic_end();

    return (ret > 0) ? 0 : -1;
#endif

#ifndef CONFIG_UART_INTERRUPT_DRIVEN
    /* wait for the transmit buffer empty */
    switch (uart->id)
    {
        case 1:
            USART_SendData( USART1,ch);
            while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
            {
            }
            break;
        case 2:
            USART_SendData( USART2,ch);
            while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
            {
            }
            break;
        case 3:
            USART_SendData( USART3,ch);
            while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
            {
            }
            break;
     }
        return 0;
#endif
}

uintx uart_read( TiUartAdapter * uart, char * buf, uintx size, uint8 opt )
{
    uint16 i;
    uintx ret;
    //intx copied=0;
    uint8 count =0;
#ifdef CONFIG_UART_INTERRUPT_DRIVEN

    hal_atomic_begin();
    copied = min( uart->rxlen, size );
    if (copied > 0)
    {
        memmove( (void *)buf, (void *)&(uart->rxbuf[0]), copied );
        uart->rxlen -= copied;
        if (uart->rxlen > 0)
            memmove( (void *)&(uart->rxbuf[0]), (void *)&(uart->rxbuf[copied]), uart->rxlen );
    }
    hal_atomic_end();

    return copied;
#endif

#ifndef CONFIG_UART_INTERRUPT_DRIVEN

    while (1)
    {
        for ( i = 0;i<0xffe;i++)
        {
            ret = uart_getchar( uart, buf );
            if ( ret)
            {
                buf++;
                count ++;
                break;
            }
        }
        if ( i>=0xffe)
        {
            break;
        }
    }
#endif

    return count;
}

uintx uart_write( TiUartAdapter * uart, char * buf, uintx len, uint8 opt )
{
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
    intx count = 0;

    hal_atomic_begin();
    count = min(CONFIG_UART_TXBUFFER_SIZE - uart->txlen, len);
    if (count > 0)
    {
        memmove( (void *)&(uart->txbuf[uart->txlen]), (void *)buf, count);
        uart->txlen += count;
    }

    // if the background ISR sending doesn't active, then enable and trigger this interrupt
    //
    if (uart->txidx == 0)
    {
        switch (uart->id)
        {
        case 0:
            while (!(UCSR0A & (1<<UDRE0))) {};      // check whether UDR0 is available for the 
            // new character to be sent
            UCSR0B |= (1 << UDRIE);                 // enable sending interrupt. the ISR will
            // continue sending until uart->txbuf is emtpy
            break;
        case 1:
            UCSR1B |= (1 << UDRIE);
            break;
        }
    }	
    hal_atomic_end();

    // default option is synchronous call. this function will return until all 
    // data being sent successfully.
    //
    if ((opt & 0x01) == 0)
    {
        // wait for sending complete
        while (uart->txlen > 0) {};

        // attention
        // if the parameter "len" is very large, then we "recursively" call uart_write()
        // to send all the data out.  
        //
        if (count < len)
        {
            uart_write( uart, (char *)buf+count, len-count, opt );
            while (uart->txlen > 0) {};
        }
        return len;
    }
    // if this is an asynchronous call, this function will return immediately 
    // and assume the "count" characters in the front of the "buf" has been 
    // sent successfully.
    else{
        return count;
    }
#endif

#ifndef CONFIG_UART_INTERRUPT_DRIVEN
    int16 count = 0;
    while (count < len)
    {
        if (uart_putchar(uart, buf[count]) < 0)
            break;
        count ++;
    }

    return count;
#endif
}

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_rx0_interrupt_handler( void * uartptr, TiEvent * e )
{ 
    TiUartAdapter * uart = (TiUartAdapter *)uartptr;
    if (CONFIG_UART_RXBUFFER_SIZE -  uart->rxlen> 0)
    {
        uart->rxbuf[uart->rxlen] = UDR0;
        uart->rxlen ++;
    } 
} 
#endif

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_tx0_interrupt_handler( void * uartptr, TiEvent * e )
{ 
    TiUartAdapter * uart = (TiUartAdapter *)uartptr;
    if (uart->txidx < uart->txlen)
    {
        UDR0 = uart->txbuf[uart->txidx];
        uart->txidx ++;
    }
    else{
        UCSR0B &= (~(1 << UDRIE));                 // disable sending interrupt. 
        uart->txlen = 0;
        uart->txidx = 0;
    }
} 
#endif

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_rx1_interrupt_handler( void * uartptr, TiEvent * e )
{ 
    TiUartAdapter * uart = (TiUartAdapter *)uartptr;
    if (CONFIG_UART_RXBUFFER_SIZE -  uart->rxlen> 0)
    {
        uart->rxbuf[uart->rxlen] = UDR1;
        uart->rxlen ++;
    } 
} 
#endif

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
void _uart_tx1_interrupt_handler( void * uartptr, TiEvent * e )
{ 
    TiUartAdapter * uart = (TiUartAdapter *)uartptr;
    if (uart->txidx < uart->txlen)
    {
        UDR1 = uart->txbuf[uart->txidx];
        uart->txidx ++;
    }
    else{
        UCSR0B &= (~(1 << UDRIE));                 // disable sending interrupt. 
        uart->txlen = 0;
        uart->txidx = 0;
    }
} 
#endif



TiBlockDeviceInterface * uart_get_blockinterface( TiUartAdapter * uart, TiBlockDeviceInterface * intf )
{
    hal_assert( intf != NULL );
    memset( intf, 0x00, sizeof(TiBlockDeviceInterface) );
    intf->provider = uart;
    intf->read = (TiFunBlockDeviceWrite)uart_read;
    intf->write = (TiFunBlockDeviceWrite)uart_write;
    intf->evolve = NULL;
    intf->switchtomode = NULL;
    return intf;
}

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


uint8 USART_Get( uint8 ch)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
    }
    ch = (USART_ReceiveData(USART2) & 0x7F); 

    return ch;
}




/***********************************************************************************
* @fn      halUartRxIntEnable
*
* @brief   Enable UART RX interrupt
*
* @param   none
*
* @return  none
*
static void halUartRxIntEnable(void)
*/

/***********************************************************************************
* @fn      halUartTxIntEnable
*
* @brief   Enable UART TX interrupt
*
* @param   none
*
* @return  none
*
#ifndef HAL_DIRECT_UART_TX
static void halUartTxIntEnable(void)
{
    //IE2 |= UCA0TXIE;
}
#endif
*/



/***********************************************************************************
* @fn      halUARTRxIntDisable
*
* @brief   Disable UART RX interrupt
*
* @param   none
*
* @return  none
*
static void halUartRxIntDisable(void)
*/

/***********************************************************************************
* @fn      halUARTTxIntDisable
*
* @brief   Disable UART TX interrupt
*
* @param   none
*
* @return  none
*
static void halUartTxIntDisable(void)
*/

/***********************************************************************************
* @fn      halUartRxGetByte
*
* @brief   Read byte from UART RX buffer
*
* @param   none
*
* @return  none
*
static uint8 halUartRxGetByte(void)
*/


/***********************************************************************************
* @fn      halUartInit
*
* @brief   Initalise UART. Supported baudrates are: 19220, 38400, 57600 and 115200
*
* @param   uint8 baudrate
*          uint8 options - this parameter is ignored
*
* @return  none
*
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


uint8 USART_Get( uint8 ch)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
    }
    ch = (USART_ReceiveData(USART2) & 0x7F); 

    return ch;
}
*/

/***********************************************************************************
* @fn      halUartWrite
*
* @brief   Write data buffer to UART
*
* @param   uint8* buf - buffer with data to write
*          uint16 length - number of bytes to write
*
* @return  uint16 - number of bytes written
*
uint16 halUartWrite(const uint8* buf, uint16 length)
*/

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
*/

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
*/


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
*/


/***********************************************************************************
* @fn      halUartEnableRxFlow
*
* @brief   Signal ready/not ready to receive characters on UART
*
* @param   uint8 enable - TRUE to signal ready to receive on UART
*                         FALSE to signal not ready to receive on UART
*
* @return  none
*
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
*/

/***********************************************************************************
* @fn      usart1Rx_ISR
*
* @brief   ISR framework for the USCI A0/B0 Receive component
*
* @param   none
*
* @return  none
#pragma vector=USCIAB0RX_VECTOR
__interrupt void usciA0Rx_ISR(void)
*
// todo interrupt ISR
void usciA0Rx_ISR(void)
{
*
    uint8 ch = halUartRxGetByte();
    bufPut(&rbRxBuf,&ch,1);
    __low_power_mode_off_on_exit();
*
}
*/

/***********************************************************************************
* @fn      usart1Tx_ISR
*
* @brief   ISR framework for the USCI A0/B0 transmit component
*
* @param   none
*
* @return  none
*
//#pragma vector=USCIAB0TX_VECTOR
//__interrupt void usciB0Tx_ISR(void)
void usciB0Tx_ISR(void)
{
*
    uint8 c;

    if (bufGet(&rbTxBuf,&c,1)==1) {
        UCA0TXBUF = c;
    }
    // If buffer empty, disable uart TX interrupt
    if( (bufNumBytes(&rbTxBuf)) == 0) {
        halUartTxIntDisable();
    }
    __low_power_mode_off_on_exit();
*	
}
*/
