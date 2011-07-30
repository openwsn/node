//----------------------------------------------------//
//-------Institute Of  Computing Technology-----------//
//------------Chinese Academic  Science---------------//
//-----中国科学院计算技术研究所先进测试技术实验室-----//
//----------------------------------------------------//

/**
 * www.wsn.net.cn
 * @copyright:nbicc_lpp
 * @data:2005.11.22
 * @version:0.0.1
 * @updata:$
 *
 */
#include "uart0.h"

int usart_putchar(char c)
{
	if(c=='\n')
		usart_putchar('\r');
	loop_until_bit_is_set(UCSR0A,UDRE0);
	UDR0=c;
	return 0;
}
//-----------------------------------------------------------
int usart_putbyte(uint8_t  c)
{
	
	loop_until_bit_is_set(UCSR0A,UDRE0);
	UDR0=c;
	return 0;
}

//-----------------------------------------------------------
int usart_getchar(void)
{
	loop_until_bit_is_set(UCSR0A,RXC0);
	return UDR0;
}

void IoInit(void)
{
	//UART初始化
	UCSR0B=_BV(RXEN0)|_BV(TXEN0);/*(1<<RXCIE)|(1<<TXCIE)|*/
	UBRR0L=12;	//57600 baud 
	
	//i/o流UART连接
	fdevopen(usart_putchar,usart_getchar,0);	
}