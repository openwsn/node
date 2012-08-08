/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/

#include "../hal_configall.h"
#include <string.h>
#include "../hal_foundation.h"
#include "../../rtl/rtl_debugio.h"
#include "../hal_assert.h"
#include "../hal_mcu.h"
#include "../hal_debugio.h"

#ifdef CONFIG_DEBUG
#define CONFIG_DBO_UART1

static bool g_dbio_init = false;
TiDebugIoAdapter g_dbio;

/*******************************************************************************
 * raw debug input/output device
 ******************************************************************************/

TiDebugIoAdapter * dbio_open( uint16 bandrate )
{
    USART_InitTypeDef USART_InitStructure;
    
	if (g_dbio_init)
		return  &g_dbio;

	memset( &g_dbio, 0x00, sizeof(TiDebugIoAdapter) );

	#ifdef CONFIG_DBO_UART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bandrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init( USART1,&USART_InitStructure);
	USART_Cmd( USART1,ENABLE);
	#endif

	#ifdef CONFIG_DBO_UART2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = bandrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init( USART2,&USART_InitStructure);
    USART_Cmd( USART2,ENABLE);
	#endif
	
	g_dbio_init = true;
    return &g_dbio;
}

void dbio_close( TiDebugIoAdapter * dbio )
{
    // @todo
	g_dbio.txlen = 0;
	g_dbio_init = false;
}

/* get a char from UART. if there's no input from UART, then this function will wait
 * until there's a input. 
 */
char dbio_getchar( TiDebugIoAdapter * dbio )
{
    // Loop until the USARTz Receive Data Register is not empty 
	#ifdef CONFIG_DBO_UART1
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {};
    return (USART_ReceiveData(USART1) & 0xFF); 
	#endif
	
	#ifdef CONFIG_DBO_UART2
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET) {};
    return (USART_ReceiveData(USART2) & 0xFF); 
    #endif
}

/* _dbio_putchar()
 * this function sends one character only through the UART hardware. 
 * 
 * @return
 *	0 means success, and -1 means failed (ususally due to the buffer is full)
 *  when this functions returns -1, you need retry.
 */
intx dbio_putchar( TiDebugIoAdapter * dbio, char ch )
{
    /* Loop until USARTy DR register is empty */
	#ifdef CONFIG_DBO_UART1
    while ( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) {};
    USART_SendData( USART1,ch);
	#endif
		
	#ifdef CONFIG_DBO_UART2
    while ( USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) {};
    USART_SendData( USART2,ch);
	#endif

	return 1;
}

/*
TiByteDeviceInterface * dbio_interface( TiByteDeviceInterface * intf )
{
    intf->&g_dbio;
    intf->putchar = dbio_putchar;
    intf->getchar = dbio_getchar;
    return intf;
}
*/

void dbio_init()
{
    dbio_open( 9600 );
    rtl_init( (void*)&g_dbio, (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, 
        hal_assert_report );
}

void dbo_open( uint16 baudrate )
{
    dbio_open( baudrate );
    rtl_init( (void*)&g_dbio, (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, 
        hal_assert_report );
}

#endif /* CONFIG_DEBUG */
