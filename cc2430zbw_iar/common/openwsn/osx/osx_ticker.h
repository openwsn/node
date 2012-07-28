#ifndef _OSX_TIMER_H_4353_
#define _OSX_TIMER_H_4353_
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

/*******************************************************************************
 * TiOsxTicker
 * the osx timer is used to drive the evolution of the whole osx kernel by periodically
 * calling the osx_hardevolve() function. it can also be used to drive some inaccurate
 * sampling tasks. if there's no osx timer, then the system can also drive the
 * osx kernel by infinite loop run. refer to the osx_execute() function. the infinite
 * loop run or osx_execute() can be placed into a separate thread of other RTOS.
 *
 * the MCU's hardware timers are usually very powerful. furthermore, they maybe
 * varies from chip suppliers. however, the OS core only needs a relatively simple
 * timer. it should be lightweight and efficient.
 *
 * for some MCU such as the ARM Cortex M3, the MCU core has already provide a
 * hardware SysTimer to help implement a OS core.
 ******************************************************************************/

//#define TiOsxTicker TiTickerAdapter
#define TiOsxTicker TiSysTimer

/*******************************************************************************
 * For developers
 * - TiOsxTicker is actually an timer/ticker interface required by the osx kernel.
 *   Any component providing this interface can be used to drive the osx kernel.
 * - Usually the TiOsxTicker can be implemented based on TiTimerAdapter, TiRtcAdapter,
 *   TiSysTimer or even the TiTimer component. The default is based on TiSysTimer
 *   as the following. System timer is a special timer which is used to drive
 *   the operating systems only.
 * 
 * @todo
 * - low power features can be added to this component in the future.
 ******************************************************************************/

inline TiOsxTicker * _osx_ticker_construct( char * buf, uint8 size )
{
	return systm_construct( buf, size );
}

inline void _osx_ticker_destroy( TiOsxTicker * timer )
{
	systm_destroy( timer );
}

inline TiOsxTicker * _osx_ticker_open( TiOsxTicker * timer, tm_value_t interval, TiFunEventHandler listener, void * lisowner )
{
	return systm_open( timer, interval, listener, lisowner );
}

inline void _osx_ticker_close( TiOsxTicker * timer )
{
	systm_close( timer );
}

inline void _osx_ticker_start( TiOsxTicker * timer )
{
	systm_start( timer );
}

inline void _osx_ticker_stop( TiOsxTicker * timer )
{
	//void systm_stop( timer );
}

inline bool _osx_ticker_expired( TiOsxTicker * timer )
{
	//return systm_expired( timer );
    return true;
}

#endif
