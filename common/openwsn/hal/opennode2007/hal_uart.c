/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 *****************************************************************************/ 

#include "hal_foundation.h"
#include "hal_cpu.h"
#include "hal_lpc213x.h"
#include "hal_interrupt.h"
#include "hal_uart.h"
#include "hal_global.h"
#include "hal_led.h"

/****************************************************************************** 
 * @author zhangwei on 2006-07-20
 * TiUartAdapter object
 * Essentially, this is the software mapping of the UART hardware 
 * 
 * @modified by zhangwei on 2006-07-20
 * modify the prototype of uart_configure()
 * update some comments of the functions.
 *
 * @modified by zhangwei on 2006-08-18
 * add support to RS232 and RS485 by using macros
 * the configure macro CONFIG_UART_RS232 and CONFIG_UART_RS485 should be defined
 * in "configall.h" in the application.
 * 
 * @modified by zhangwei on 20061102
 * change "baudrate" from type uint16 to uint32
 * because the value of baudrate may be "115200". it exceeds the maximum value 
 * of uint16 65536.
 * 
 * @modified by zhangwei on 20070410
 * revision. format the source file 
 * eliminate led_twinkle() in interrupt disable state. because it may cause data
 * loss in fast communication.
 * 
 * @modified by makun on 20070412
 * correct the error of data loss
 * modified interrupt service routine
 * 
 *****************************************************************************/

#ifdef CONFIG_UART_RS232
#define UART232
#endif

#ifdef CONFIG_UART_RS485
#define UART485
#endif

#if ((!defined(UART232)) && (!defined(UART485)))
  #define UART232
#endif

#if ((defined(UART232)) && (defined(UART485)))
  #undef UART485
#endif  

extern TiUartAdapter * g_uart0;
extern TiUartAdapter * g_uart1;

//void __irq uart_interrupt0( void );
//void __irq uart_interrupt1( void );
static void __irq uart_interrupt( void );
static void uart_interrupt_init( TiUartAdapter * uart );

/****************************************************************************** 
 * initialze the UART hardware and object
 * @param
 * 	id		0	UART0
 * 			1 or other values	UART1
 * @return 
 * 	0		success
 *  -1		failed
 * 
 * @modified by zhangwei on 20061010
 * @TODO
 * zhangwei kept the old declaration of the function in order to keep other modules 
 * running. you should call uart_configure() after uart_construct()
 *****************************************************************************/
TiUartAdapter * uart_construct( uint8 id, char * buf, uint16 size )
{
	TiUartAdapter * uart;
     
        
	if (sizeof(TiUartAdapter) > size)
		uart = NULL;
	else 
		uart = (TiUartAdapter *)buf;
	
		
	if (uart != NULL)
	{
		memset( (char*)uart, 0x00, sizeof(TiUartAdapter) );
		//led_on(LED_GREEN);
		uart->id = id;
		
		uart->baudrate = 9600;
		uart->databits = 8;
		uart->stopbits = 1;
		uart->parity = 0;
		//uart_configure(uart, baudrate, databits, stopbits, parity );
		//uart_configure (uart, 9600, 8, 1, 0 );
		
		/***** 使能UART0中断 ****************/
		VICIntSelect = 0x00000000;//设置所有的通道为IRQ中断；	

		if (id ==0)
		{
			VICVectCntl0 = 0x20|0x06;//UART0分配到IRQ slot0,即最高优先级；
			VICVectAddr0 = (uint32)uart_interrupt;//设置UART0向量地址
			
			/** 使能FIFO，并设置触发点为8字节 ****/
			//U0FCR = 0x81;
			U0FCR = 0x01;//1字节触发中断
			/*****允许RBR中断，即接收中断 ********/
			U0IER = 0x01;	
			VICIntEnable = 1<<0x06;//使能UART0中断；
		}
		else
		{
			VICVectCntl1 = 0x20|0x06;//UART0分配到IRQ slot0,即最高优先级；
			VICVectAddr1 = (uint32)uart_interrupt;//设置UART0向量地址；
					/** 使能FIFO，并设置触发点为8字节 ****/
			//U0FCR = 0x81;
			U1FCR = 0x01;//1字节触发中断
			/*****允许RBR中断，即接收中断 ********/
			U1IER = 0x01;	
			VICIntEnable |= 1<<0x07;//使能UART1中断；??地址待定。	
		}
	}

	// by default, the UART interrupt is disabled after construction. 
	// you should call uart_configure() to enable the interrupt in your application.
	VICVectAddr = 0x00;	
	return uart;
}

void uart_destroy( TiUartAdapter * uart )
{
	NULL;
}


/****************************************************************************** 
 * @TODO 20061013
 * you should enable the interrupt in configure function
 *****************************************************************************/
int16 uart_configure (TiUartAdapter * uart,uint32 baudrate, uint8 databits, uint8 stopbits, 
	uint8 parity, uint8 optflag )
{
	uint32 bak;
	uart->baudrate = baudrate;

	if(( stopbits == 0 )||( stopbits > 2 ))
	{
		return (-1);
	}
	
	uart ->stopbits = stopbits;
	
	if( parity > 4 )
	{	
		return (-1);
	}
	
	uart ->parity = parity;

	bak  = (Fpclk >>4) / (baudrate);		 
    
	/**      设置波特率     **/
	if (uart->id == 0)
	{
		PINSEL0 = (PINSEL0 & (~0x0F)) | 0x00000005;	
		U0LCR = 0x80;							
		U0DLM = bak >>8;
		U0DLL = bak &0xFF;

	// @TODO
	// you may deleted the following		
    U0LCR = 0x03;                               /* Disable to access the frequenc regecter 禁止访问分频因子寄存器 */
                                                /* set to 8,1,n 且设置为8,1,n */
		U0IER = 0x00;                               /* Disable interrupt禁止中断 */
    U0FCR = 0x00;                               /* initial FIFO 初始化FIFO */

	}
    else
    {
		PINSEL0 = (PINSEL0 & (~(0x0F<<16))) | (0x05<<16);	
		U1LCR = 0x80;							
		U1DLM = bak >>8;
		U1DLL = bak &0xFF;
    }

   	/**   设置串口模式 **/
	bak = 3;

	if (stopbits == 2)
	{
		bak|=0x04;
	}
    
	if (parity !=0)
	{
		parity = parity -1;
		bak|=0x08;
	}
	
	bak |= parity<<4;

    if (uart->id == 0)
    {
    	U0LCR = bak;
    }	
	else
	{
	
	    U1LCR = bak;
	}	
	
	if (optflag & 0x01)
	{
		// @TODO
		// enable interrupt here
		uart_interrupt_init( uart );
	}
	
	return (0);
}

/****************************************************************************** 
 * clear the TiUartAdapter internal buffer
 *****************************************************************************/
void uart_reset( TiUartAdapter * uart )
{
	hal_enter_critical();
	uart->txlen = 0;
	uart->rxlen = 0;
	hal_leave_critical();
}

/* @modified by zhangwei on 20061014
 * 新增函数，在这里完成ISR初始化工作 */
void uart_interrupt_init( TiUartAdapter * uart )
{
}

/****************************************************************************** 
 * read data out from the TiUartAdapter's internal buffer. 
 * this is a non-block operation. it will return 0 if no data received.
 * 
 * @attention
 * @modified by zhangwei on 20070411
 * you should call led_twinkle() before hal_leave_critical(), so i eliminate it.
 * 
 * @param
 * 	buf		the memory buffer to received the data
 * 	size	the capacity of the buffer
 * @return
 * 	the character count wroten to the buf
 * 	return 0 means there's no data in the TiUartAdapter buffer. i.e. no data received.
 *****************************************************************************/
#ifdef CONFIG_UART_READ_ENABLE
uint16 uart_read( TiUartAdapter * uart, char * buf, uint16 size, uint16 opt )
{
	uint16 copied;
	
	hal_enter_critical();
	copied = min( size, uart->rxlen );
	if (copied > 0)
	{
		memmove( buf, &(uart->rxbuf[0]), copied );
	    //把后面的数据向前提，保证寄存器中的数据是从rxbuffer[0]开始的；
	    memmove( &(uart->rxbuf[0]), (char*)(&(uart->rxbuf[0]))+copied, uart->rxlen - copied );
		uart->rxlen -= copied;
	}
	
	hal_leave_critical();
	//led_twinkle(LED_YELLOW,3);
	return copied;
}
#endif

/****************************************************************************** 
 * write the data in the buffer to UART to sent out
 * this is a non-block operation. 
 * @param
 * 	buf		where's the data
 * 	len		length of the data in the buffer
 * @return
 * 	the count of characters actually sent
 *****************************************************************************/
#ifdef CONFIG_UART_WRITE_ENABLE
uint16 uart_write( TiUartAdapter * uart, char * buf, uint16 len, uint16 opt )
{
	int16 count = 0;
	while (count < len)
	{
		if (uart_putchar(uart, buf[count]) < 0)
			break;
		count ++;
	}
	
	return count;
}
#endif

/****************************************************************************** 
 * this function is hardware related
 * you should change the register in this function
 * 
 * @return
 * 	0		success, *ch is char just read from UART
 *  -1		failed
 *****************************************************************************/
int16 uart_getchar( TiUartAdapter * uart, char * pc )
{
	int16 ret;
	
	if (uart->rxlen > 0)
	{
		*pc = uart->rxbuf[0];
		uart->rxlen --;
	    memmove( &(uart->rxbuf[0]), (char*)(uart->rxbuf[0])+1, uart->rxlen );
		ret = 0;
	}
	else
		ret = -1;

	return ret;
}

int16 uart_putchar( TiUartAdapter * uart, char ch )
{
	if(!(uart->id))
	{
		U0THR = ch;//周立功arm
		while((U0LSR & 0x20) == 0);

	}
	else{
		U1THR = ch;//周立功arm
		while((U1LSR & 0x40) == 0);

	}
	return 0;
}

// 黄欢提供的新代码 20061010
// @modified by zhangwei on 20061013
// @TODO
// 错误
// 这里应该读register判断是UART0还是UART1中断，判g_uart->id是无效的，因为g_uart未知
//

/****************************************************************************** 
 * @attention
 * @modified by zhangwei on 20070411
 * you should call led_twinkle() before hal_leave_critical(), so i eliminate it.
 *****************************************************************************/

void __irq uart_interrupt( void )
{
	char c;	
	
	if (g_uart->id == 0)
	//if (1)
	{
		while ((U0IIR & 0x01)==0)
		{
			if ((U0IIR & 0x0F) == 0x04)
			{		
				c = U0RBR;			
				if (g_uart0->rxlen < UART_RXBUFFER_SIZE)
				{
				   g_uart0->rxbuf[g_uart0->rxlen++] = c;	
				}
			}
		}	
              
	}
	else{
		while ((U1IIR & 0x01)==0)
		{
			if ((U1IIR & 0x0F) == 0x04)
			{		
				c = U1RBR;
				if (g_uart1->rxlen < UART_RXBUFFER_SIZE)
				{
					   g_uart1->rxbuf[g_uart1->rxlen++] = c;	
				}
			}
		}	
	}
	
	VICVectAddr = 0x00;	
}

#ifdef UART232
#undef UART232
#endif

#ifdef UART485
#undef UART485
#endif

