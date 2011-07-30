/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available

 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 
#include "hal_foundation.h"
#include "hal_led.h"
#include "hal_target.h"

/*****************************************************************************
 * @modified by zhangwei on 20070626
 * revision the old version source code
 * add support to opennode-3.0 hardware
 * 
 * @modified by openwsn on 20070626
 * correct the error in led_toggle. you should use IO0PIN rather than IO0SET.
 * thanks for Jiang. using IO0SET may lead to wrong results in LPCxxx chips.
 * 
 * @modified by openwsn on 20070626
 * moved the LED initialization instructions from "target.c" to "led_init()"
 * in this file.
 *
 ****************************************************************************/ 

void led_init()
{
	#ifdef CONFIG_TARGET_OPENNODE_10
	#endif
	
	#ifdef CONFIG_TARGET_OPENNODE_20
	#endif
	
	#ifdef CONFIG_TARGET_OPENNODE_30
	#endif
	
	#ifdef CONFIG_TARGET_WLSMODEM_11
	#endif

	#ifdef CONFIG_TARGET_DEFAULT
	#endif
}

void led( uint8 id, bool state )
{
	if (state)
		led_on( id );
	else
		led_off( id );
}

void led_off( uint8 id )
{
	switch(id)
	{
	case LED_GREEN:    
		#if LED_GREEN_PORT == 0 
	    IO0SET  = BM(LED_GREEN_PIN);    break;
	    #endif
	         
	    #if LED_GREEN_PORT == 1 
	    IO1SET  = BM(LED_GREEN_PIN);    break;
	    #endif
	         
	case LED_RED:    
		#if LED_RED_PORT == 0 
	    IO0SET  = BM(LED_RED_PIN);      break;
	    #endif
	         
	    #if LED_RED_PORT == 1 
	    IO1SET  = BM(LED_RED_PIN);      break;
	    #endif
	         
	case LED_YELLOW:   
		#if LED_YELLOW_PORT == 0 
	    IO0SET  = BM(LED_YELLOW_PIN);   break;
	    #endif
	         
		#if LED_YELLOW_PORT == 1 
	    IO1SET  = BM(LED_YELLOW_PIN);   break;
	    #endif
	
	case LED_ALL:
		#if LED_GREEN_PORT == 0 
	    IO0SET  = BM(LED_GREEN_PIN);    
	    #endif
	    #if LED_GREEN_PORT == 1 
	    IO1SET  = BM(LED_GREEN_PIN);    
	    #endif

  		#if LED_RED_PORT == 0 
	    IO0SET  = BM(LED_RED_PIN);      
	    #endif
	    #if LED_RED_PORT == 1 
	    IO1SET  = BM(LED_RED_PIN);      
	    #endif

	  	#if LED_YELLOW_PORT == 0 
	    IO0SET  = BM(LED_YELLOW_PIN);   
	    #endif
		#if LED_YELLOW_PORT == 1 
	    IO1SET  = BM(LED_YELLOW_PIN);   
	    #endif
	    break;
	}
}

void led_on( uint8 id )
{
	switch(id)
	{
	case LED_GREEN:    
		#if LED_GREEN_PORT == 0 
	    IO0CLR  = BM(LED_GREEN_PIN);   break;
	    #endif
	         
	    #if LED_GREEN_PORT == 1 
	    IO1CLR  = BM(LED_GREEN_PIN);   break;
	    #endif
	         
	case LED_RED:    
		#if LED_RED_PORT == 0 
	    IO0CLR  = BM(LED_RED_PIN);     break;
	    #endif
	         
	    #if LED_RED_PORT == 1 
	    IO1CLR  = BM(LED_RED_PIN);     break;
	    #endif
	         
	case LED_YELLOW:   
		#if LED_YELLOW_PORT == 0 
	    IO0CLR  = BM(LED_YELLOW_PIN);   break;
	    #endif
	         
	    #if LED_YELLOW_PORT == 1 
	    IO1CLR  = BM(LED_YELLOW_PIN);   break;
	    #endif
	    
	case LED_ALL: 
		#if LED_GREEN_PORT == 0 
	    IO0CLR  = BM(LED_GREEN_PIN);   
	    #endif
	    #if LED_GREEN_PORT == 1 
	    IO1CLR  = BM(LED_GREEN_PIN);   
	    #endif

		  #if LED_RED_PORT == 0 
	    IO0CLR  = BM(LED_RED_PIN);     
	    #endif
	         
	    #if LED_RED_PORT == 1 
	    IO1CLR  = BM(LED_RED_PIN);     
	    #endif
	         
  		#if LED_YELLOW_PORT == 0 
	    IO0CLR  = BM(LED_YELLOW_PIN);   
	    #endif
	         
	    #if LED_YELLOW_PORT == 1 
	    IO1CLR  = BM(LED_YELLOW_PIN);   
	    #endif
		break;
	}
}

void led_toggle( uint8 id )
{
	switch(id)
	{
	case LED_GREEN:    
		#if LED_GREEN_PORT == 0
	    if(IO0PIN & BM(LED_GREEN_PIN))  IO0CLR  = BM(LED_GREEN_PIN); 
		else                            IO0SET  = BM(LED_GREEN_PIN);   	     
	    #endif
	         	
	    #if LED_GREEN_PORT == 1
	    if(IO1PIN & BM(LED_GREEN_PIN))  IO1CLR  = BM(LED_GREEN_PIN); 
	    else                            IO1SET  = BM(LED_GREEN_PIN);   	     
	    #endif
	    break;

	case LED_RED:      
		#if LED_RED_PORT == 0
	    if(IO0PIN & BM(LED_RED_PIN))  IO0CLR  = BM(LED_RED_PIN); 
	    else                          IO0SET  = BM(LED_RED_PIN);   	     
	    #endif
	         	
	    #if LED_RED_PORT == 1
	    if(IO1PIN & BM(LED_RED_PIN))  IO1CLR  = BM(LED_RED_PIN); 
	    else                          IO1SET  = BM(LED_RED_PIN);   	     
	    #endif
	    break;

	case LED_YELLOW:   
		#if LED_YELLOW_PORT == 0
	    if(IO0PIN & BM(LED_YELLOW_PIN))  IO0CLR  = BM(LED_YELLOW_PIN); 
	    else                             IO0SET  = BM(LED_YELLOW_PIN);   	     
	    #endif
	         	
		#if LED_YELLOW_PORT == 1
	    if(IO1PIN & BM(LED_YELLOW_PIN))  IO1CLR  = BM(LED_YELLOW_PIN); 
	    else                             IO1SET  = BM(LED_YELLOW_PIN);   	     
	    #endif
	    break;
	}
}

void led_twinkle( uint8 id ,uint16 interval )
{
  	switch(id)
	{
	case LED_GREEN:
		led_toggle(LED_GREEN);
	    hal_delay( interval );	
	    led_toggle(LED_GREEN);
	    break;

	case LED_RED:
		led_toggle(LED_RED);
	    hal_delay( interval );	
	    led_toggle(LED_RED);
	    break;

	case LED_YELLOW:
		led_toggle(LED_YELLOW);
	    hal_delay( interval );	
	    led_toggle(LED_YELLOW);
	    break;
	}
}
