/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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
 
#ifndef _HAL_ADC_H_3467_
#define _HAL_ADC_H_3467_

/*******************************************************************************
 * @author MaKun on 200612
 * @note
 * 	for AD transformation
 * 
 * @history
 * @modified by zhangwei on 20070101
 *	- revise source code from MaKun
 * @modified by zhangwei on 20090715
 *	- revision. new interface now.
 * @modified by xiao-yuezhang(TongJi University) on 20090716
 *	- ported to atmega128 and GAINZ platform
 *  - tested by xu-xizhou(TongJi University)
 * @modified by openwsn in 2011.08
 *  - revision
 ******************************************************************************/
 
#include "hal_configall.h"
#include "hal_foundation.h"

#define CONFIG_ADC_INTERRUPT_DRIVEN
#undef  CONFIG_ADC_INTERRUPT_DRIVEN

#ifndef CONFIG_ADC_BUFSIZE
#define CONFIG_ADC_BUFSIZE 32
#endif

#ifdef __cplusplus
extern "C"{
#endif

/* TiAdcAdapter
 *  option[bit7..bit0]: 
 *					bit0 indicate whether this adc is driven by interrupt or not. 
 *	listener:		will be called when A/D conversion complete if interrupt enabled.
 *	lisowner:       used when call listener
 * 
 * @todo: Actually most of the ADC have multiple channels. This object only covers
 * the most simple case.
 */
typedef struct{
  uint8 id;
  uint8 option;
  TiFunEventHandler listener;
  void * lisowner;
  volatile uint8 len;
  volatile char data[CONFIG_ADC_BUFSIZE];
}TiAdcAdapter;

TiAdcAdapter * adc_construct( char * buf, uint8 size );
void adc_destroy( TiAdcAdapter * adc );
TiAdcAdapter * adc_open( TiAdcAdapter * adc, uint8 id, TiFunEventHandler listener, void * lisowner, uint8 option );
void adc_close( TiAdcAdapter * adc );
void adc_start( TiAdcAdapter * adc );
void adc_stop( TiAdcAdapter * adc );
uint16 adc_read(TiAdcAdapter * adc, char * buf, uint8 size, uint8 opt );

#ifdef __cplusplus
}
#endif

#endif
