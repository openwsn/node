#ifndef _HAL_RTC_H_3888_
#define _HAL_RTC_H_3888_
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


/******************************************************************************
 * @author Yan Shixing on 2009.12.09
 * TiRtcAdapter 
 * This object is the software encapsulation of the MCU's Real-time clock.
 * 
 * @modified by LiXin at TongJi University on 2010.05.25
 *  - upgraded. bug fixed. released. at least the interrupt interface is tested.
 * @modified by ZhangWei at TongJi University on 2010.11.25
 *  - revision. bug fix.
 *****************************************************************************/
 
#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_clock.h"
#include "hal_common.h"


/* TiRtcAdapterTime
 * represent the int type used by the RTC hardware. it varies from 16bit to 32 bit 
 * on different hardware platform. 
 */
#ifdef CONFIG_TARGETBOARD_GAINZ 
  #define TiRtcAdapterTime uint16
#elif defined( CONFIG_TARGETBOARD_OPENNODE2010)
  #define TiRtcAdapterTime uint32
#else
  #error "you should choose correct TiRtcAdapterTime type on your hardware architecture!"
#endif

/**
 * CONFIG_INTERRUPT_MODE_LISTENER_ENABLE
 * If this macro is defined, then the interrupt service routine will call the listerner
 * function. This will be faster but also a lot of restrictions in interrupt mode.
 * If this macro is undefined, then the listener function will be called when the
 * master function invokes rtc_expired() function. So in the latter case, the master
 * must check for rtc_expired() periodically and as often as possible.
 */
#undef CONFIG_INTERRUPT_MODE_LISTENER_ENABLE

/* TiCalendarTime
 * this's a universal time type. an calendar time variable occupies 10 bytes in the 
 * memory. it can be transmitted over the network from one device to another device.
 */

#define TiRealtimeClock TiRtcAdapter  
#define TiRtc TiRtcAdapter
//#define TiCalendarTime TiCalTime
//#define ctime_t TiCalTime

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * TiRtcAdapter
 * 
 *****************************************************************************/

/* TiCalTime
 * 
 * 10 byte representation of accurate timestamp
 * [year 2B][month 1B][day 1B][hour 1B][min 1B][sec 1B][msec 2B][usec 2B]
 * 
 * control: highest 2bit. always 00
 * year: 0-9999 14b 
 * reserved 4b
 * month: 1-12, 4b
 * day: 1-31: 5b
 * hour: 0-23: 5b
 * min: 6b
 * sec: 6b
 * msec: 0-999, 10b  (or using the highest 6b to represent usec. each unit represent 2^4=16us)
 * us: 0-999, 10b
 */
#pragma pack(1) 
typedef struct{
  uint16 year;
  uint8  month;
  uint8  day;
  uint8  hour;
  uint8  min;
  uint8  sec;
  uint16 msec;
}TiCalTime;

/* TiRtcAdapter
 * RTC hardware adapter component.
 */
#pragma pack(1) 
typedef struct{
  TiCalTime curtime;
  TiCalTime deadline;
  uint32 currenttime;

  TiFunEventHandler listener;
  void * lisowner;

  uint8 id;//the way of the interrupt for rtc. 1->second interrupt;2->overflow interrupt;3->alarm interrupt.
  uint16 scale;
  uint16 interval;
  uint32 overflow_counter;
  uint32 alarm_counter;
  uint8 option;//whether use interrupt or not.
  uint32 prescaler;
}TiRtcAdapter;


/**
 * Construct an TiRtcAdapter component on specified memory block.
 */
TiRtcAdapter * rtc_construct( char * buf, uint8 size );
void rtc_destroy( TiRtcAdapter * rtc );

/**
 * Open an rtc component and do initializations.
 */
TiRtcAdapter * rtc_open( TiRtcAdapter * rtc, TiFunEventHandler listener, void * object,uint8 id, uint8 option );

void rtc_setoverflow_count( TiRtcAdapter *rtc,uint32 count);
void rtc_setalrm_count( TiRtcAdapter *rtc,uint32 count);
void rtc_setprscaler( TiRtcAdapter *rtc,uint32 prescaler);
uint32 rtc_get_counter( uint32 count);

void rtc_close( TiRtcAdapter * rtc );
void rtc_setinterval( TiRtcAdapter * rtc, uint16 interval, uint16 scale, uint8 repeat );

/**
 * Start the RTC clock and the hardware will run from now on.
 */
void rtc_start( TiRtcAdapter * rtc );
void rtc_stop( TiRtcAdapter * rtc );

/**
 * Restart the RTC using the last configuations. This function is often used after
 * checking the expired flag if you don't configure the RTC to repeat automatically.
 */
void rtc_restart( TiRtcAdapter * rtc );

/**
 * Set current time inside the TiRtcAdapter component 
 */
void rtc_setvalue( TiRtcAdapter * rtc, TiCalTime * caltime );

/**
 * Get current time
 */
void rtc_getvalue( TiRtcAdapter * rtc, TiCalTime * caltime );

/** 
 * Configure when the RTC will be expired. The value is often much longer than the 
 * RTC hardware can supported. For example, you can configure the TiRtcAdapter component to
 * expired everey 24 hours, while the RTC hardware only support maximum 2 seconds.
 * During this long period, there maybe many times low level timer expire interrupts, 
 * but only one time high level expire event.
 */
void rtc_setexpire( TiRtcAdapter * rtc, TiCalTime * caltime, uint8 repeat);

/**
 * Check for the RTC component to see whether the timer is expired. 
 * 
 * @attention: the expired flag inside the RTC component will be clearly by this function
 * only. So if you call this function and get true this time, then the next time it
 * will be false.
 */
bool rtc_expired( TiRtcAdapter * rtc );

void rtc_forward( TiRtcAdapter * rtc, uint16 sec );
void rtc_backward( TiRtcAdapter * rtc, uint16 sec );

bool rtc_getclockinterface( TiClockInterface * clock );

TiBasicTimerInterface * rtc_basicinterface( timer, TiBasicTimerInterface * intf );
TiLightTimerInterface * rtc_lightinterface( timer, TiLightTimerInterface * intf );


#ifdef __cplusplus
}
#endif

#endif /* _HAL_RTC_H_3888_ */

