/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
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
/*文件名: test3.c
功能：使用定时器0实现定时0.5秒的走马灯实验
说明：时钟：7.3728MHz
****************************************************/

#include <avr/io.h>
#include<util/delay.h>

#define uchar unsigned char
#define uint unsigned int

//PA0口输出低电平，黄灯亮
#define SET_YEL_LED PORTA&=~_BV(PA0) 
#define CLR_YEL_LED PORTA|=_BV(PA0) //PA0口输出高电平，黄灯灭

#define SET_GEN_LED PORTA&=~_BV(PA1)//PA1口输出低电平，绿灯亮
#define CLR_GEN_LED PORTA|=_BV(PA1)//PA1口输出高电平，绿灯灭

#define SET_RED_LED PORTA&=~_BV(PA2)//PA2口输出低电平，红灯亮
#define CLR_RED_LED PORTA|=_BV(PA2)//PA2口输出高电平，红灯灭

#define uchar unsigned char 

void Delay(void);
void FlashLedYel(void);
void FlashLedRed(void);
void FlashLedGen(void);

/***************************************************
*名称：main()
*功能：走马灯程序，让三个灯轮流闪烁
*入口参数：无
*出口参数：无
*****************************************************/

/***************************************************
*名称：Delay()
*功能：初始化I/O及定时器0,定时时间为0.5s
*入口参数：无
*出口参数：无
*****************************************************/
void Delay()
{
	uchar i=0;
	TCNT0=0; // T/C0 开始值
	TCCR0=_BV(CS02); // 预分频 ck/64 ,计数允许
 
	//查询定时器方式等待0.5秒
	//7372800 /64 /256 /225 = 2Hz
	for(i=0;i<225;i++)
	{
		loop_until_bit_is_set(TIFR,TOV0);
		//写入逻辑1 清零TOV0 位
		TIFR|=_BV(TOV0);
	}
}

/***************************************************
*名称： FlashLedYel()
*功能：控制黄灯闪烁
*入口参数：无
*出口参数：无
*****************************************************/
void FlashLedYel(void)
{
	SET_YEL_LED;
	Delay();
	CLR_YEL_LED;
	Delay();
}

/***************************************************
*名称： FlashLedRed()
*功能：控制红灯闪烁
*入口参数：无
*出口参数：无
*****************************************************/
void FlashLedRed(void)
{
	SET_RED_LED;
	Delay();
	CLR_RED_LED;
	Delay();
}

/***************************************************
*名称： FlashLedGen()
*功能：控制绿灯闪烁
*入口参数：无
*出口参数：无
*****************************************************/
void FlashLedGen(void)
{
	SET_GEN_LED;
	Delay();
	CLR_GEN_LED;
	Delay();
}

/**************************************************
文件名：test5.c
功能：串口通信程序，将发送给串口的字符串发还给串口
说明：时钟：7.3728MHz
*****************************************************/





/***************************************************
*名称putc()
*功能  向串口发送数据
*入口参数char c
*出口参数：无
*****************************************************/

void uart_putc(uchar c)
{
	while( !(UCSR0A & (1<<UDRE0)) );//数据发送已准备好
	UDR0=c;
	
}

/***************************************************
*名称getc()
*功能  从串口接收数据
*入口参数：无
*出口参数：UDR
*****************************************************/
uchar getc(void)
{
	while( !(UCSR0A & (1<<RXC0)) );//接收完成
	return UDR0;
}

/***************************************************
*名称main()
*功能  串口通信
*入口参数：无
*出口参数：UDR
*****************************************************/
int main(void)
{
	//uart 初始化
	PORTA=(_BV(PA0)) | (_BV(PA1)) | (_BV(PA2));        
	DDRA=_BV(PA0)|_BV(PA1)|_BV(PA2);   //设置PA0,PA1,PA2为输出口
	
	
		FlashLedRed();
		FlashLedYel();
		FlashLedGen();
	
	
	UBRR0L=12;//38.4k baud 8.00MHz error: 0.2%
	UCSR0B=_BV(RXEN0)|_BV(TXEN0);//使能发送和接受允许
	while(1)
		uart_putc(getc()+1);
	
	
	
		//输出从串口接收的数据
}