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

/*
 * @modified by zhangwei on 2010.11.25
 * - revision
 */ 

#include "../hal_configall.h"
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_cpu.h"
#include "../hal_interrupt.h"
#include "..//hal_timer.h"
#include "../hal_debugio.h"
#include "../hal_uart.h"
#include "../hal_led.h"
#include "../hal_assert.h"
#include "../hal_targetboard.h"
#include "../hal_rtc.h"

//@todo
#define CONFIG_INTERRUPT_MODE_LISTENER_ENABLE 0x87
/* attention:
 *  - atmega128L actually use TIMER0 hardware as RTC. so if you use RTC, then you 
 * cannot use TIMER0. the mainly difference between general TIMER0 and RTC are their 
 * clock source. the RTC uses the special RTC oscillator (usually 32768 KHz).
 */
static inline void _rtc_interrupt_handler( void * object, TiEvent * e );

//static void _rtc_interrupt_disable( TiRtcAdapter * rtc );
//static void _rtc_interrupt_enable( TiRtcAdapter * rtc );

static int _caltime_compare( TiCalTime * left, TiCalTime * right );
//@todo
static char _caltime_not_leap( TiCalTime * curtime);//static char _caltime_not_leap( TiRtcAdapter * rtc );
static void _caltime_forward( TiCalTime * curtime, uint16 ms );

/*******************************************************************************
 * TiCalTime
 ******************************************************************************/

/**
 * compare two TiCalTime variables. 
 * 
 * @return 
 *  0 when equal
 *  -1 when left < right
 *  1 when left > right
 */
int _caltime_compare( TiCalTime * left, TiCalTime * right )
{
    // @todo 下面这一句assert干什么用的？
    //hal_assert( false );
    
    return 0;
}

/**
 * check for leap year
 */
char _caltime_not_leap( TiCalTime * curtime ) 
{
	if (!(curtime->year % 100))
		return (char)(curtime->year % 400);
	else
		return (char)(curtime->year % 4);
}

/* Foward the current time forward for milliseconds.
 * @warning: This function doesn't check forward overflow. because we think this is
 * almost impossible.
 */ 
void _caltime_forward( TiCalTime * curtime, uint16 ms )
{
    uint8 ms_count = ms / 1000;
    uint8 ms_left = ms % 1000;

    if (ms_left > 0)
    {
        curtime->msec += ms_left;
        if (curtime->msec >= 1000)
        {
            curtime->msec -= 1000;
            ms_count ++;
        }
    }

    // @todo
    // bad style in the following source code. not very safe

    curtime->sec+= ms_count;
	if (curtime->sec >= 60)
	{
		curtime->sec=0;
		if (++curtime->min==60)
		{
			curtime->min=0;
			if (++curtime->hour==24)
			{
				curtime->hour=0;
				if (++curtime->day==32)
				{
					curtime->month++;
					curtime->day=1;
				}
				else if (curtime->day==31)
				{
					if ((curtime->month==4) || (curtime->month==6) || (curtime->month==9) ||(curtime->month==11))
					{
						curtime->month++;
						curtime->day=1;
					}
				}
				else if (curtime->day==30)
				{
					if(curtime->month==2)
					{
						curtime->month++;
						curtime->day=1;
					}
				}
				else if (curtime->day==29)
				{
					if((curtime->month==2) && (_caltime_not_leap(curtime)))
					{
						curtime->month++;
						curtime->day=1;
					}
				}
				if (curtime->month==13)
				{
					curtime->month=1;
					curtime->year++;
				}
			}
		}
	}
}

/*******************************************************************************
 * TiRtcAdapter
 ******************************************************************************/

TiRtcAdapter * rtc_construct( char * buf, uint8 size )
{
	hal_assert( sizeof(TiRtcAdapter) <= size );
	memset( buf, 0x00, size );
	return (TiRtcAdapter *)buf;
}

void rtc_destroy( TiRtcAdapter * rtc )
{
	
}

TiRtcAdapter * rtc_open( TiRtcAdapter * rtc, TiFunEventHandler listener, void * object, uint8 option )
{
	rtc->listener = listener;
  	rtc->lisowner = object;
  	rtc->option = option;

    // todo
    // you should initialize the following variables 
	//@todo 下面那一句assert是干什么用的？
    //hal_assert( false );
/*

	rtc->scale = scale;
	rtc->interval = interval;
	rtc->repeat = repeat;
	rtc->scale_counter = scale;
	rtc->interval_counter = interval;
*/	
	hal_attachhandler( INTNUM_TIMER0_OVF, _rtc_interrupt_handler, rtc );
	//hal_attachhandler( INTNUM_TIMER0_COMP, _rtc_interrupt_handler, rtc );

	return rtc;
}

void rtc_close( TiRtcAdapter * rtc )
{
	hal_detachhandler( INTNUM_TIMER0_OVF );
}

void rtc_setinterval( TiRtcAdapter * rtc, uint16 interval, uint16 scale, uint8 repeat )
{
	rtc->scale = scale;
	rtc->interval = interval;
	//@todo
	rtc->option = repeat;//rtc->repeat = repeat;
	rtc->scale_counter = scale;
	rtc->interval_counter = interval;
}

void rtc_start( TiRtcAdapter * rtc )
{
    // allow timer0's overflow flag
	TIFR = (1 << TOV0);

    // bit0: TOIE0 timer0 overflow interrupt enable. 
    // bit1: OCIE0 timer0 output compare match interrupt enable.
    //
    // @attention:
    // the following source code will disable these two interrupts to avoid unexpected 
    // exception. if you have already used timer0 somewhere, the following code will 
    // affect its settings. 
	TIMSK &= ~((1<<TOIE0)|(1<<OCIE0));

	rtc->curtime.sec = 0;
	rtc->curtime.min = 0;
	rtc->curtime.hour = 0;
	rtc->curtime.day = 1;
	rtc->curtime.month = 1;
	rtc->curtime.year = 2000;


    // TCCR: timer control register
    // TCNT: timer counter(8bit for timer0, which is also RTC)
    // ASSR: time sychronous state register

	ASSR |= (1<<AS0);//将定时器0置为异步方式，使用独立晶振。
	TCNT0 = 0x00;

    // 设置预分频器CLK/128，当晶振为32.768K时，溢出周期为1s。
    // attention the RTC time(timer0) is a 8bit timer and will be expired after 255.
    // @attention: shall we keep the value of the high 5 bits in TCCR0?
	TCCR0 = 0x05;

    //等待定时器0各寄存器更新结束，切换到异步状态。
    while(ASSR & 0x07) {};

    // enable the TOV0 flag.
	TIFR = (1 << TOV0);
    //打开定时器0溢出中断，开始计时。
	TIMSK |= (1<<TOIE0);
}

void rtc_stop( TiRtcAdapter * rtc )
{
	TIFR = (1 << TOV0);
	TIMSK &= ~(1<<TOIE0);//关闭定时器0溢出中断。

    // clear the AS0 bit (bit3) in ASSR register. this will cause timer0 drived by
    // CLK I/O. if AS0 is 1, then the RTC is driven by the oscillator connected to 
    // the TOSC1 pin of the MCU. attention the TCNT, OCR0 and TCCR0 maybe destroy 
    // when changing the AS0.
    // 
	ASSR &= ~(1<<AS0);//将定时器0置为普通方式。
	
	TCNT0 = 0x00;
	TCCR0 = 0x00;
    while(ASSR & 0x07) {};//判断同步、异步切换结束，保证后续程序正常运行。
}

void rtc_restart( TiRtcAdapter * rtc )
{
	rtc_start( rtc );

}
/*
void _rtc_interrupt_disable( TiRtcAdapter * rtc )
{
	TIFR = (1 << TOV0);
	TIMSK &= ~(1<<TOIE0);//关闭定时器0溢出中断。
}
	
void _rtc_interrupt_enable( TiRtcAdapter * rtc )
{
	TIFR = (1 << TOV0);
	TIMSK |= (1<<TOIE0);//打开定时器0溢出中断。
}
*/
void rtc_setvalue( TiRtcAdapter * rtc, TiCalTime * caltime )
{
    hal_disable_interrupts();
    memmove( &(rtc->curtime), caltime, sizeof(TiCalTime) );
    hal_enable_interrupts();
}

void rtc_getvalue( TiRtcAdapter * rtc,  TiCalTime * caltime )
{
    hal_disable_interrupts();
    memmove( caltime, &(rtc->curtime), sizeof(TiCalTime) );
    hal_enable_interrupts();
}

void rtc_setexpire( TiRtcAdapter * rtc, TiCalTime * deadline, uint8 repeat )
{
	memmove( &(rtc->deadline), deadline, sizeof(TiCalTime) );
	//@todo
	//rtc->repeat =  repeat;
	//rtc->option = 0x01;
	rtc->option = repeat;//之前变量的repeat就是用option代替的
}

/*
void rtc_active( TiRtcAdapter * rtc )
{
	if(rtc->option == 0x01)
	{
		if ((rtc->curtime.year == rtc->deadline.year) &&
            (rtc->curtime.month == rtc->deadline.month) &&
            (rtc->curtime.day == rtc->deadline.day) &&
            (rtc->curtime.hour == rtc->deadline.hour) &&
            (rtc->curtime.min == rtc->deadline.min) &&
            (rtc->curtime.sec == rtc->deadline.sec))
		{
			rtc->expired = true;
			if (rtc->listener != NULL)
			{
				rtc->listener( rtc->lisowner, NULL );
				rtc->expired = false;
			}
			if (rtc->repeat == 0x00)
			{
				rtc->option = 0x00;
			}
		}
	}
}
*/

/* judge whether the rtc timer is expired
 */
bool rtc_expired( TiRtcAdapter * rtc )
{   

	//@todo
	bool ret = 1;//后加的不知道对不对？
    // Clear the second bit in option settings. If this bit is 1, then indicate the last
    // interval has been expired.
    rtc->option &= 0xFD;

    #ifndef CONFIG_INTERRUPT_MODE_LISTENER_ENABLE
    if (rtc->listener != NULL)
		//@todo
        rtc->listener( rtc->lisowner,NULL );//rtc->listener( rtc->lisowner );
    #endif

    // restart the timer if the expired restart timer is configured
    if ((rtc->option & 0x01) == 0)
    {
        rtc_stop( rtc );
    }

    return ret;

/*
	if( rtc->interval_counter == 0 )
	{
		rtc->scale_counter --;
		if (rtc->scale_counter == 0)
		{
			rtc->expired = true;
			if (rtc->listener != NULL)
			{
				rtc->listener( rtc->lisowner, NULL );
				rtc->expired = false;
				if (rtc->repeat == 0x01)
				{
					rtc->scale_counter = rtc->scale;
					rtc->interval_counter = rtc->interval;
				}
			}
		}
		else rtc->interval_counter = rtc->interval;
	}
	else
	{
		rtc->interval_counter --;
	}
*/
}

void rtc_forward( TiRtcAdapter * rtc, uint16 ms )
{
    hal_disable_interrupts();
	//@todo
    _caltime_forward(&(rtc->curtime), ms );//_caltime_forward( ms );
    hal_enable_interrupts();
}

void rtc_backward( TiRtcAdapter * rtc, uint16 ms )
{
    hal_disable_interrupts();
    // todo
    hal_assert( false );
    hal_enable_interrupts();
}


/**
 * This function is actually the interrupt handler. it will be called everytime the
 * RTC interrupt occured. This interrupt can be overflow interrupt or comparable 
 * interrupt, depends on how you configure the RTC timer.
 */
void _rtc_interrupt_handler( void * object, TiEvent * e )
{
	TiRtcAdapter * rtc = (TiRtcAdapter *)object;
	
    // update RTC internal software clock and check whether it's expired.
    // recommended interval value is 20-100 depends on the MCU's workload.
	_caltime_forward( &(rtc->curtime), 1000 );

	 //@todo
    if (!_caltime_compare(&rtc->curtime, &rtc->deadline) )//if (!_caltime_compare(&rtc->curtime, &rtc->deadline) == 0)
    {    

        // set the expired flag in the option
        rtc->option |= 0x02;//这一句应不应该删掉？option 代表repeat！

        #ifdef CONFIG_INTERRUPT_MODE_LISTENER_ENABLE
        if (rtc->listener != NULL)
			//@todo
            rtc->listener( rtc->lisowner,NULL );//rtc->listener( rtc->lisowner );
        #endif
    }

    // restart the hardware timer and let it continue to run
    // use (TIFR & _BV(TOV0)) for timer overflow flag
	// use (TIFR & _BV(OCF0)) for timer CTC flag 
	//@todo
    bool ret;//后加的，不知道对不对？
	ret = (TIFR & _BV(TOV0));
    if (ret)
    {
	    // clear the TOV0 bit by writing 1
		TIFR |= _BV(OCF0);
    }
} 
