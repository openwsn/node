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
 *
 * @modified by shizhirong on 2012.6.27
 * - add irqhandler	and listener/owner
 * - tested
 */ 

#include "../hal_configall.h"
#include "../hal_mcu.h"
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_cpu.h"
#include "../hal_debugio.h"
#include "../hal_uart.h"
#include "../hal_led.h"
#include "../hal_assert.h"
#include "../hal_rtc.h"
#include "../hal_common.h"

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
	// @todo
	rtc=rtc;
}

/* rtc_open.  option->whether use interrupt or not. 1->use. 0->not use
* id->the way of the interrupt for rtc. 1->second interrupt;2->overflow interrupt;3->alarm interrupt.
*/
TiRtcAdapter * rtc_open( TiRtcAdapter * rtc, TiFunEventHandler listener, void * object,uint8 id, uint8 option )
{
	rtc->listener = listener;
  	rtc->lisowner = object;
  	rtc->option = option;
    rtc->id = id;
    rtc->currenttime = 0;
    rtc->prescaler = 32767;
//	rtc->irqhandler=irqhandler; // @todo
	return rtc;
}

void rtc_close( TiRtcAdapter * rtc )
{
	//hal_detachhandler( INTNUM_TIMER0_OVF );
}

void rtc_setprscaler( TiRtcAdapter *rtc,uint16 prescaler)
{
   rtc->prescaler = prescaler;
}


void rtc_setalrm_count( TiRtcAdapter *rtc,uint16 count,uint8 repeat)//interval = count +1;
{
    rtc->alarm_counter = count;
    RTC_WaitForLastTask();
    RTC_SetAlarm( RTC_GetCounter()+count);
}

void rtc_setoverflow_count( TiRtcAdapter *rtc,uint16 count,uint8 repeat)
{
    rtc->overflow_counter = count;
    RTC_SetCounter( count);
}

uint32 rtc_get_counter( uint32 count)
{
    count = RTC_GetCounter();
    return RTC_GetCounter();
}

void rtc_setlistener( TiRtcAdapter *rtc, TiFunEventHandler listener, void * object )
{
    rtc->listener = listener;
    rtc->lisowner = object;
}

void rtc_setinterval( TiRtcAdapter * rtc, uint16 interval, uint16 scale, uint8 repeat )
{
    /*
	rtc->scale = scale;
	rtc->interval = interval;
	//@todo
	rtc->option = repeat;//rtc->repeat = repeat;
	rtc->scale_counter = scale;
	rtc->interval_counter = interval;
    */
}

void rtc_start( TiRtcAdapter * rtc )
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    //RCC_PCLK2Config(RCC_HCLK_Div8);
    //RCC_PCLK1Config(RCC_HCLK_Div8);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    if ( rtc->option)//use interrupt
    {
        if ( rtc->id ==3)//alarm interrupt
        {
            //configuration for alarm interrupt
            EXTI_ClearITPendingBit(EXTI_Line17);
            EXTI_InitStructure.EXTI_Line = EXTI_Line17;
            EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		   
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
            EXTI_InitStructure.EXTI_LineCmd = ENABLE;
            EXTI_Init(&EXTI_InitStructure);

            NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
            /* Enable the RTC Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
        }
        else
        {
            NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
            /* Enable the RTC Interrupt */
            NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
        }
    }

    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
    /* Reset Backup Domain */
    BKP_DeInit(); //记录0XA5A5 来确定是否重置时间
    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}
    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    if ( rtc->option)
    {
        if ( rtc->id ==3)
        {
			hal_attachhandler(INTNUM_RTCALARM,_rtcalarm_interrupt_handler,rtc);	  
        }

        if ( rtc->id==2) 
        {
			hal_attachhandler(INTNUM_RTC,_rtcsec_interrupt_handler,rtc);		
	        RTC_SetCounter( rtc->overflow_counter);
        }
		else if(rtc->id==1)
		{	
			hal_attachhandler(INTNUM_RTC,_rtcsec_interrupt_osx_handler,rtc);  
		}
    }
    RTC_WaitForLastTask();
    /* Enable the RTC Alarm */
    if ( rtc->option)
    {
        switch (rtc->id)
        {
        case 1:
            RTC_ITConfig(RTC_IT_SEC, ENABLE);
        	break;
        case 2:
            RTC_ITConfig(RTC_IT_OW, ENABLE);
            break;
        case 3:
            RTC_ITConfig(RTC_IT_ALR, ENABLE);
            break;
        default:
            hal_assert(0);
            break;
        }
    }
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(rtc->prescaler);//RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

void rtc_stop( TiRtcAdapter * rtc )
{
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, DISABLE);	
}

//void rtc_restart( TiRtcAdapter * rtc )
//{
//	rtc_start( rtc );
//
//}

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
    bool ret = false;
    if ( RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        ret = true;
    }

    return ret;
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

bool rtc_getclockinterface( TiClockInterface * clock )
{
    memset( clock, 0x00, sizeof(TiClockInterface) );
    clock->start = rtc_start;
    // @todo
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
//    bool ret;//后加的，不知道对不对？
//	ret = (TIFR & _BV(TOV0));
//    if (ret)
//    {
//	    // clear the TOV0 bit by writing 1
//		TIFR |= _BV(OCF0);
//    }
} 


TiBasicTimerInterface * rtc_basicinterface( TiRtcAdapter * rtc, TiBasicTimerInterface * intf )//提供的是alarm中断接口
{
    intf->provider = rtc;
    intf->setinterval = rtc_setalrm_count;
    intf->setscale = rtc_setprscaler;
    intf->setlistener = rtc_setlistener;
    intf->start = rtc_start;
    intf->stop = rtc_stop;
    intf->expired = rtc_expired;
    return intf;
}
TiLightTimerInterface * rtc_lightinterface( TiRtcAdapter * rtc, TiLightTimerInterface * intf )//提供的是alarm中断接口
{
    intf->provider = rtc;
    intf->setinterval = rtc_setalrm_count;
    intf->setscale = rtc_setprscaler;
    intf->setlistener = rtc_setlistener;
    intf->start = rtc_start;
    intf->stop = rtc_stop;
    intf->expired = rtc_expired;
    return intf;
}


void _rtcalarm_interrupt_handler( void * object, TiEvent * e )
{
	TiRtcAdapter * rtc=	(TiRtcAdapter *) object;
    EXTI_ClearITPendingBit(EXTI_Line17);
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_ALR);
		led_toggle( LED_RED);//for testing

		if((rtc->listener!=NULL)&&(rtc->lisowner!=NULL))
		{
			rtc->listener(rtc->lisowner,NULL);
		}

    }

}

void _rtcsec_interrupt_handler( void * object, TiEvent * e )
{
	TiRtcAdapter * rtc=	(TiRtcAdapter *) object;

	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);

		if((rtc->listener!=NULL)&&(rtc->lisowner!=NULL))
		{
			rtc->listener(rtc->lisowner,NULL);
		}

	}
}

void _rtcsec_interrupt_osx_handler( void * object, TiEvent * e )
{
	TiRtcAdapter * rtc=	(TiRtcAdapter *) object;

   if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		/* Clear the RTC Second interrupt */
		RTC_ClearITPendingBit(RTC_IT_SEC);
		USART_Send( 0x00);					 //0705

		if((rtc->listener!=NULL)&&(rtc->lisowner!=NULL))
		{
			rtc->listener(rtc->lisowner,NULL);
		}
		//led_toggle( LED_RED);//for testing
	}
}




