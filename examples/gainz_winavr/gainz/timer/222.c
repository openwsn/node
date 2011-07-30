#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <avr/signal.h>

#define uchar unsigned char

#define set_led PORTA=0x01
#define clr_led PORTA=0x02

int main (void)
{ 
  uchar i,j=0;
  DDRA=0xff;
  PORTA=0x00;

  TCNT0=0;
  TCCR0=5;

  while(1)
  {
    for (i=0;i<15;i++)
	{loop_until_bit_is_set(TIFR,TOV0);
	 //sbi(TIFR,TOV0);
	TIFR=TIFR|0x01;
	}
	if(j)
	set_led,j=0;
    else
	clr_led,j=1;
  }
}
