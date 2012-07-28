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

#ifndef _HAL_SYSTICKER_H_3214_
#define _HAL_SYSTICKER_H_3214_


#include "hal_configall.h"
#include "hal_foundation.h"
#include "../rtl/rtl_time.h"

/*******************************************************************************
 * hal_systicker
 * TiSysTimer is used by and should used by the osx kernel only. It drives the osx 
 * kernel to run. Besides the TiSysTimer, the osx kernel can also be drived by
 * a infinite loop.
 *
 * @author zhangwei in 2006.10
 * @modified by zhangwei on 2009.05.20
 *	- revision
 * 
 * @modified by openwsn in 2010.11
 *  - upgraded the former module hal_systimer as hal_systicker. Now it's the ticker 
 *    component to generate pulse to drive the osx kernel to run.
 ******************************************************************************/

/*
#ifdef CONFIG_TARGETBOARD_GAINZ
  #define tm_value_t uintx
#else
  #define tm_value_t uintx
#endif
*/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Q: what is TiSysTimer used for?
 * R: support Embedded OS or hardware drived schedulers
 * 
 * Q: why not using a TiTimerAdapter instead of a new TiSysTimer
 * R: the TiSysTimer is usually much simpler than TiTimeAdapter. it doesn't need
 * multi-channel and capture support. a simple TiSysTimer will make the upper layer
 * embedded os more easier to port to other platforms.
 *
 * Q: what's Tick?
 * R: Tick is an time unit. The system timer generates an expire event every tick
 *
 ******************************************************************************/

// this implements the TiBasicTimerInterface which is used to drive the OS and 
// some simple timer requirements.

#define tm_value_t uint16  

typedef struct{
  uint8 state;
  //uint8 prescale_selector;
  //uint16 prescale;

  TiSystemTime time;//todo  目前单位应该是ms

  tm_value_t interval;
  uint16 reginterval;
  TiFunEventHandler listener;
  void * lisowner;
  //uint16 TCCR;
  uint8 TCCR;//todo
  uint16 OCR3;
}TiSysTimer; // will be upgraded to TiTickerAdapter

TiSysTimer * systm_construct( char * buf, uint8 size );
void systm_destroy( TiSysTimer * timer );
TiSysTimer * systm_open( TiSysTimer * timer, tm_value_t interval, TiFunEventHandler listener, void * lisowner );
void systm_close( TiSysTimer * timer );
void systm_start( TiSysTimer * timer );
void systm_stop( TiSysTimer * timer );
bool systm_expired( TiSysTimer * timer );

// TiBasicTimerInterface * ticker_basicinterface( TiTimerAdapter * timer, TiBasicTimerInterface * intf );


#ifdef __cplusplus
}
#endif

#endif /* _HAL_SYSTICKER_H_3214_ */

