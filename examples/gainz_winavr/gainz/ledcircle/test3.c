/**************************************************
文件名: test3.c
功能：使用定时器0实现定时0.5秒的走马灯实验
说明：时钟：7.3728MHz
***************************************************/

#include <avr/io.h>

#define SET_YEL_LED PORTA&=~_BV(PA0)//PA0口输出低电平，黄灯亮
#define CLR_YEL_LED PORTA|=_BV(PA0)//PA0口输出高电平，黄灯灭

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
int main( void )
{
	PORTA=_BV(PA0)|_BV(PA1)|_BV(PA2);        
	DDRA=_BV(PA0)|_BV(PA1)|_BV(PA2);   //设置PA0,PA1,PA2为输出口
	while(1)
	{
		FlashLedRed();
		FlashLedYel();
		FlashLedGen();
	};

}

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
