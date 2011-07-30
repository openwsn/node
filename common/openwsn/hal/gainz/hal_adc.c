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
#include "../hal_configall.h"
#include <stdlib.h>
//#include <power.h>
#include <string.h>
#include <avr/io.h>
#include "../hal_foundation.h"
#include "../hal_cpu.h"
#include "../hal_interrupt.h"
#include "../hal_adc.h"
#include "../hal_uart.h"
#include "../hal_led.h"
#include "../hal_debugio.h"
#include "../hal_assert.h"

static void _adc_interrupt_handler( void * adcptr, TiEvent * e );


TiAdcAdapter * adc_construct( char * buf, uint8 size )
{  
	hal_assert( sizeof(TiAdcAdapter) <= size );
	memset( buf, 0x00, size );
	return (TiAdcAdapter *)buf;
}

void adc_destroy( TiAdcAdapter * adc )
{
	return;
}

/* adc_open
 * this function will power on the adc component and prepare all the required 
 * operations for A/D conversion. 
 */
TiAdcAdapter * adc_open( TiAdcAdapter * adc, uint8 id, TiFunEventHandler listener, void * lisowner, uint8 option )
{
	hal_atomic_begin();

	adc->id = id;
	adc->listener = listener;
	adc->lisowner = lisowner;
	adc->option = option;
	adc->len = 0;

	switch (adc->id)
	{
	case 0:
		// provide 3.3V power supply
		HAL_MAKE_INT1_OUTPUT();
		HAL_SET_INT1_PIN();

		// power_adca_enable();

		// clear the ADIE and ADSC bit in ADCSRA
		ADCSRA &= 0x86;

		// set referene voltage. use default settings. 
		ADMUX = 0;

		hal_attachhandler( INTNUM_ADC_COMPLETE, _adc_interrupt_handler, adc );
		break;

	case 1:
		HAL_MAKE_INT1_OUTPUT();
		HAL_SET_INT1_PIN();
		//power_adcb_enable();
		// clear the ADIE and ADSC bit in ADCSRA
		ADCSRA &= 0x86;

		// set referene voltage. use default settings. 
		ADMUX = 0x01;

		hal_attachhandler( INTNUM_ADC_COMPLETE, _adc_interrupt_handler, adc );
		break;

	case 2:
		HAL_MAKE_INT1_OUTPUT();
		HAL_SET_INT1_PIN();
		//power_adcb_enable();
		// clear the ADIE and ADSC bit in ADCSRA
		ADCSRA &= 0x86;

		// set referene voltage. use default settings. 
		ADMUX = 0x02;

		hal_attachhandler( INTNUM_ADC_COMPLETE, _adc_interrupt_handler, adc );
		break;

	case 3:
		HAL_MAKE_INT1_OUTPUT();
		HAL_SET_INT1_PIN();
		//power_adcb_enable();
		// clear the ADIE and ADSC bit in ADCSRA
		ADCSRA &= 0x86;

		// set referene voltage. use default settings. 
		ADMUX = 0x03;

		hal_attachhandler( INTNUM_ADC_COMPLETE, _adc_interrupt_handler, adc );
		break;

	case 4:
		HAL_MAKE_INT1_OUTPUT();
		HAL_SET_INT1_PIN();
		//power_adcb_enable();
		// clear the ADIE and ADSC bit in ADCSRA
		ADCSRA &= 0x86;

		// set referene voltage. use default settings. 
		ADMUX = 0x04;

		hal_attachhandler( INTNUM_ADC_COMPLETE, _adc_interrupt_handler, adc );
		break;

	case 5:
		HAL_MAKE_INT1_OUTPUT();
		HAL_SET_INT1_PIN();
		//power_adcb_enable();
		// clear the ADIE and ADSC bit in ADCSRA
		ADCSRA &= 0x86;

		// set referene voltage. use default settings. 
		ADMUX = 0x05;

		hal_attachhandler( INTNUM_ADC_COMPLETE, _adc_interrupt_handler, adc );
		break;

	case 6:
		HAL_MAKE_INT1_OUTPUT();
		HAL_SET_INT1_PIN();
		//power_adcb_enable();
		// clear the ADIE and ADSC bit in ADCSRA
		ADCSRA &= 0x86;

		// set referene voltage. use default settings. 
		ADMUX = 0x06;

		hal_attachhandler( INTNUM_ADC_COMPLETE, _adc_interrupt_handler, adc );
		break;

	case 7:
		HAL_MAKE_INT1_OUTPUT();
		HAL_SET_INT1_PIN();
		//power_adcb_enable();
		// clear the ADIE and ADSC bit in ADCSRA
		ADCSRA &= 0x86;

		// set referene voltage. use default settings. 
		ADMUX = 0x07;

		hal_attachhandler( INTNUM_ADC_COMPLETE, _adc_interrupt_handler, adc );
		break;
	}

	hal_atomic_end();
	return adc; 
}

/* adc_close
 * will power off the A/D conversion and force it in low power mode if possible 
 */
void adc_close( TiAdcAdapter * adc )
{
	switch (adc->id)
	{
	case 0:
		//power_adca_disable();
		hal_detachhandler( INTNUM_ADC_COMPLETE );
		break;
	case 1:
		//power_adcb_disable();
		break;
	}
	return;
}

void adc_start( TiAdcAdapter * adc )
{
	switch (adc->id)
	{
	case 0:
		// set ADMUX
		// Notice: this is to set the channel to be 1 not 0, so this logic
		// might lead to some problem --Yan
		ADMUX = 0x01;

		// enable A/D. start it
		ADCSRA |= 0xE7;
		break;
	case 1:
		break;
	}
}

void adc_stop( TiAdcAdapter * adc )
{
	switch (adc->id)
	{
	case 0:
         //清除ADC中断标志
          ADCSRA |=0x10;
         //关闭ADC转换使能
          //ADCSRA &=0x7f;
		break;
	case 1:
		break;
	}
}


#ifdef CONFIG_ADC_INTERRUPT_DRIVEN
uint16 adc_read(TiAdcAdapter * adc, char * buf, uint8 size, uint8 opt )
{
	uint16 count=0;

	if (adc->id = 0)
	{
		hal_atomic_begin();
		if (adc->len > 0)
		{
			count = min(size, adc->len);
			memmove( buf, &(adc->buf[0]), count );
			adc->len = 0;
		}
		hal_atomic_end();
	}

	return count;
}
#endif

#ifndef CONFIG_ADC_INTERRUPT_DRIVEN
uint16 adc_read(TiAdcAdapter * adc, char * buf, uint8 size, uint8 opt )
{
	uint16 count=0;

	if (adc->id == 0)
	{
		// set ADMUX
		ADMUX = 0x01;

		// enable A/D. start it
		ADCSRA |= 0xE7;

		// todo: 请在这里while中查询ADC状态寄存器以判断ADC是否结束
        // todo the old source code here has the while loop
		//while (g_adc_interrupt_flag = 0)
		
		//while(!(ADCSRA & 0x10));
		
        hal_delay( 10 );

		buf[0] = ADCL;
		buf[1] = ADCH;
        count = 1;
	}

	return count;
}
#endif

/* _adc_interrupt_handler
 * this interrupt handler will be called when the A/D conversion completed.
 */
void _adc_interrupt_handler( void * adcptr, TiEvent * e )
{
	TiAdcAdapter * adc = (TiAdcAdapter *)adcptr;

	// read values in ADCL and ADCH
	// todo: what's meaning of the following two line? seems a mistake 
	// (copied from ICT GAINZ example)
	adc->data[0] = ADCL;
	adc->data[0] += (ADCH << 8);
	adc->len = 1;

	if (adc->listener != NULL)
		adc->listener( adc->lisowner, e );
}


/*
uint8_t  adc_init(void)
{
     { 
           hal_atomic_t _nesc_atomic = hal_atomic_start();
		  {
           //ADC控制寄存器中ADIE位和ADSC位清零 
                  ADCSRA &=0x86;
           //设置参考电压，配置为默认情形
                  ADMUX=0;
           	}
          hal_atomic_end(_nesc_atomic);
     }
    ADC_INTERRUPT_FLAG=0;
	return SUCCESS;
}

uint8_t  sensor_init(void)
{
  { hal_atomic_t _nesc_atomic = hal_atomic_start();

    {
     
      
      HAL_MAKE_INT1_OUTPUT();
      HAL_SET_INT1_PIN();
      //whether to configure  the INT2 pin???
    }

    hal_atomic_end(_nesc_atomic); }           
    	return SUCCESS;
      
}

void read_data_task(void)
{
  uint8_t loopnum=0;
  while (loopnum<10)
  {
  
  SENSOR_DATA_VALUE[loopnum]=0;
  { hal_atomic_t _nesc_atomic = hal_atomic_start();

    {
     //设置ADMUX寄存器
         ADMUX |=0x01;  
     //AD使能开始AD转换
         ADCSRA |=0xe7; 
     }

    hal_atomic_end(_nesc_atomic); }       

     //查询ADCSRA中断，
  
   while(ADC_INTERRUPT_FLAG==0)
   	{    
   	      ADC_INTERRUPT_FLAG=(ADCSRA&0x10);
   	}
  
    //读取ADCH和ADCL
    
    SENSOR_DATA_VALUE[loopnum]=ADCL;
    SENSOR_DATA_VALUE[loopnum]+=(ADCH<<8);
    printf("sensor is %04x\n",SENSOR_DATA_VALUE[loopnum]);
  { hal_atomic_t _nesc_atomic = hal_atomic_start();

    {
     //清除ADC中断标志
          ADCSRA |=0x10;
     //关闭ADC转换使能
          //ADCSRA &=0x7f;
    	}
      hal_atomic_end(_nesc_atomic); }  
	  
   //printf("sensor is %04x\n",SENSOR_DATA_VALUE);
  // SENSOR_DATA_VALUE=0;
   adc_init();
   //
   //HAL_CLR_INT1_PIN();
   ADC_INTERRUPT_FLAG=0;
   //sensor_init();
loopnum++;
  } 
	 
}
*/
