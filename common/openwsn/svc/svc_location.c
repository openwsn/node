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

#include "../hal/hal_foundation.h"
#include <assert.h>
#include <stdlib.h>
#include "svc_location.h"
#include "../hal/hal_led.h"
#include "../hal/hal_cc2420.h"
#include "../hal/hal_cc2420base.h"
#include "../hal/hal_spi.h"
#include "../global.h"
#include "start.h"

// @TODO: pls move these variables to the object definition

static TiCc2420AdapterFrame location_tx_packet;
static TiCc2420AdapterFrame location_rx_packet; 
    
static int8 length;
static uint8 n,temp;
static int16 anchor_1_x = 0,   anchor_1_y = 0;
static int16 anchor_2_x = 0,   anchor_2_y = 3.7;
static int16 anchor_3_x = 2.5, anchor_3_y = 3.5;
static uint16 dis_1 = 0,dis_2 = 0,dis_3 = 0;
    
static char * a1_string    = "signal from anchor node 1\n";
static char * a2_string    = "signal from anchor node 2\n";
static char * a3_string    = "signal from anchor node 3\n";
static char * rssi_string  = "the rssi value is : ";
static char * dis_string   = "the distance is : ";
static char * enter = "\n";
static char * location_string = "the node's location is: ";
static char * comma = ",";    

static void _reverse_matrix( int16 * array );
static void _compute_location( int16 *x,int16 *y,int16 *c2,int16 *d2 );
static void _localize( int16 *x,int16 *y,int16 x1,int16 y1,int16 d1,int16 x2,int16 y2,int16 d2,int16 x3,int16 y3,int16 d3);

/* 求逆矩阵 
 * @param
 * 	array		the start address of the array
 */
static void _reverse_matrix( int16 * array )
{
	int16 a,b,c,d,e;
	a = *array;
	b = *(array + 1);
	c = *(array + 2);
	d = *(array + 3);
	e = a*d - b*c;
	*array = d/e;
	*(array + 1) = -1 * b /e;
	*(array + 2) = -1 * c /e;
	*(array + 3) = a/e;
}

static void _compute_location( int16 *x,int16 *y,int16 *c2,int16 *d2 )
{
    _reverse_matrix(c2);
	*x =  c2[0] * d2[0] + c2[1] * d2[1];
	*y =  c2[2] * d2[0] + c2[3] * d2[1];
}

//通过三点二维定位
static void _localize( int16 *x,int16 *y,int16 x1,int16 y1,int16 d1,int16 x2,int16 y2,int16 d2,int16 x3,int16 y3,int16 d3)
{
        int16 c2[4],dis2[2];
	c2[0] = 2 * (x1 - x2);
	c2[1] = 2 * (y1 - y2);
	c2[2] = 2 * (x2 - x3);
	c2[3] = 2 * (y2 - y3);
	dis2[0] = d2 * d2 - d1 * d1 -x2 * x2 -y2 * y2 + x1 * x1 + y1 * y1;
	dis2[1] = d3 * d3 - d2 * d2 -x3 * x3 -y3 * y3 + x2 * x2 + y2 * y2;
        _compute_location(x,y,c2,dis2);
}

TiLocationService * lcs_construct(char * buf, uint16 size)
{
  	
  	TiLocationService * loc;
	char* out_string = "location service consturct succesful!\n";
	
	assert
	if (sizeof(TiLocationService) > size)
		loc = NULL;
	else
		lo = (TiLocationService *)buf;
		
	
	if (lo != NULL)
	{	
		
		memset( (char*)lo, 0x00, sizeof(TiLocationService) );
		lo->state = 0;
		lo->nodeid= 0;
		uart_write(g_uart, out_string,38, 0);
	}
        
	return lo;
	
}

void lcs_destroy( TiLocationService * lcs )
{
	
}

void lcs_rssi2dist( TiLocationService * lcs, uint8 rssi, uint16 * dis )
{
	if((rssi >= L_1_RSSI) || (rssi < 30))  {*dis = L_1;     return;}
    if(rssi >= L_2_RSSI)                   {*dis = L_2 -  (rssi - L_2_RSSI)   / (L_1_RSSI - L_2_RSSI) * LENGTH_UNIT;   return;}
    if(rssi >= L_3_RSSI)                   {*dis = L_3 -  (rssi - L_3_RSSI)   / (L_2_RSSI - L_3_RSSI) * LENGTH_UNIT;   return;}
    if(rssi >= L_4_RSSI)                   {*dis = L_4 -  (rssi - L_4_RSSI)   / (L_3_RSSI - L_4_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_5_RSSI)                   {*dis = L_5 -  (rssi - L_5_RSSI)   / (L_4_RSSI - L_5_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_6_RSSI)                   {*dis = L_6 -  (rssi - L_6_RSSI)   / (L_5_RSSI - L_6_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_7_RSSI)                   {*dis = L_7 -  (rssi - L_7_RSSI)   / (L_6_RSSI - L_7_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_8_RSSI)                   {*dis = L_8 -  (rssi - L_8_RSSI)   / (L_7_RSSI - L_8_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_9_RSSI)                   {*dis = L_9 -  (rssi - L_9_RSSI)   / (L_8_RSSI - L_9_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_10_RSSI)                  {*dis = L_10 - (rssi - L_10_RSSI)  / (L_9_RSSI - L_10_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_11_RSSI)                  {*dis = L_11 - (rssi - L_11_RSSI)  / (L_10_RSSI - L_11_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_12_RSSI)                  {*dis = L_12 - (rssi - L_12_RSSI)  / (L_11_RSSI - L_12_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_13_RSSI)                  {*dis = L_13 - (rssi - L_13_RSSI)  / (L_12_RSSI - L_13_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_14_RSSI)                  {*dis = L_14 - (rssi - L_14_RSSI)  / (L_13_RSSI - L_14_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_15_RSSI)                  {*dis = L_15 - (rssi - L_15_RSSI)  / (L_14_RSSI - L_15_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_16_RSSI)                  {*dis = L_16 - (rssi - L_16_RSSI)  / (L_15_RSSI - L_16_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_17_RSSI)                  {*dis = L_17 - (rssi - L_17_RSSI)  / (L_16_RSSI - L_17_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_18_RSSI)                  {*dis = L_18 - (rssi - L_18_RSSI)  / (L_17_RSSI - L_18_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_19_RSSI)                  {*dis = L_19 - (rssi - L_19_RSSI)  / (L_18_RSSI - L_19_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_20_RSSI)                  {*dis = L_20 - (rssi - L_20_RSSI)  / (L_19_RSSI - L_20_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_21_RSSI)                  {*dis = L_21 - (rssi - L_21_RSSI)  / (L_20_RSSI - L_21_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_22_RSSI)                  {*dis = L_22 - (rssi - L_22_RSSI)  / (L_21_RSSI - L_22_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_23_RSSI)                  {*dis = L_23 - (rssi - L_23_RSSI)  / (L_22_RSSI - L_23_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_24_RSSI)                  {*dis = L_24 - (rssi - L_24_RSSI)  / (L_23_RSSI - L_24_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_25_RSSI)                  {*dis = L_25 - (rssi - L_25_RSSI)  / (L_24_RSSI - L_25_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_26_RSSI)                  {*dis = L_26 - (rssi - L_26_RSSI)  / (L_25_RSSI - L_26_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_27_RSSI)                  {*dis = L_27 - (rssi - L_27_RSSI)  / (L_26_RSSI - L_27_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_28_RSSI)                  {*dis = L_28 - (rssi - L_28_RSSI)  / (L_27_RSSI - L_28_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_29_RSSI)                  {*dis = L_29 - (rssi - L_29_RSSI)  / (L_28_RSSI - L_29_RSSI) * LENGTH_UNIT;    return;}
    *dis = 50;
}

int8 lcs_read( TiLocationService * lcs, TiLocation * loc, uint8 opt )
{
	return 0;
}

int8 lcs_write( TiLocationService * lcs, TiLocation * loc, uint8 opt )
{
	return 0;
}

void lcs_init(TiLocationService * lcs,TiCc2420Adapter * cc )
{
    location_tx_packet.panid = 0x2420;
    location_rx_packet.panid = 0x2420;

    
    if(!(lcs->state & LOC_TYPE_ARCHOR))  //unknow node
    {	
      location_tx_packet.nodefrom = 0x1000;
      cc2420_configure( cc, CC2420_CONFIG_LOCALADDRESS, location_tx_packet.nodefrom, 0);
      for (n = 0; n < 2; n++) {
        location_tx_packet.payload[n] = 8;   //88 represents localize request
      }
    }
   
    
    if(lcs->state & LOC_TYPE_ARCHOR)  //anchor node
    {	
      
      if(lcs->nodeid == 1)
      {
        location_tx_packet.nodefrom = 0x1111;
        cc2420_configure( cc, CC2420_CONFIG_LOCALADDRESS, location_tx_packet.nodefrom, 0);
        location_tx_packet.payload[0] = 0;
        location_tx_packet.payload[1] = 0;   //payload[0,1] represent anchor's x and y
      }	
    
   
    	
      if(lcs->nodeid == 2)
      {
        location_tx_packet.nodefrom = 0x2222;
        cc2420_configure( cc, CC2420_CONFIG_LOCALADDRESS, location_tx_packet.nodefrom, 0);
        location_tx_packet.payload[0] = 10;
        location_tx_packet.payload[1] = 0;   //payload[0,1] represent anchor's x and y
      }	
    	
    
      if(lcs->nodeid == 3)
      {
        location_tx_packet.nodefrom = 0x3333;
        cc2420_configure( cc, CC2420_CONFIG_LOCALADDRESS, location_tx_packet.nodefrom, 0);
        location_tx_packet.payload[0] = 0;
        location_tx_packet.payload[1] = 10;   //payload[0,1] represent anchor's x and y
      }	
    }	

    
    cc2420_receive_on(cc);
    IRQEnable();
}

int8 lcs_evolve( TiLocationService * lcs, TiLocation * location, TiCc2420Adapter * cc )
{
    
    if(!(lcs->state & LOC_TYPE_ARCHOR))  //unknow node
    {	    
    	    //send request to anchor_1, if received infor of anchor_1, then do next
    	    location_tx_packet.nodeto = 0x1111;
    	    while(1)
    	    {
    	        led_twinkle(LED_RED,1);
    	        led_toggle(LED_GREEN);
    	        //length = cc2420_write( cc, &(location_tx_packet),2+11,0);
    	        location_tx_packet.length = 2+11;
    	        length = cc2420_write( cc, &(location_tx_packet), 0);
    	        if(length != -1) break;
    	    }
    	    
    	    
    	    while(1) 
    	    {
    	        led_twinkle(LED_GREEN,1);
    	        //length = cc2420_read( cc,&location_rx_packet,0,0);
    	        length = cc2420_read( cc,&location_rx_packet,0);
	        if(length > 11) 
	        {
	            uart_write(g_uart, a1_string,26, 0);
	            
	            //anchor_1_x     = location_rx_packet.Payload[0];
	            //anchor_1_y     = location_rx_packet.Payload[1];
	            
	            
	            lcs_rssi2dist(lcs,cc->rssi, &dis_1);
	            
	            //uart_putchar(g_uart,(char)anchor_1_x );
	            //uart_putchar(g_uart,(char)anchor_1_y );
	            
	            //uart_putchar(g_uart,(char)(location_rx_packet.srcAddr) );
	            //uart_putchar(g_uart,(char)(location_rx_packet.srcAddr >> 8) );
	            
	            uart_write(g_uart, rssi_string,20, 0);	            
	            temp = cc->rssi;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            
	            
	            uart_write(g_uart, dis_string,18, 0);
	            temp = (uint8)dis_1;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            uart_putchar(g_uart,(char)*enter );
	            break;
	        }
    	    }
    	    
    	    hal_delay(2000);
    	    
    	    //send request to anchor_2, if received infor of anchor_2, then do next
    	    location_tx_packet.nodeto = 0x2222;
    	    while(1)
    	    {
    	        led_twinkle(LED_RED,2);
    	        //length = cc2420_write( cc, &(location_tx_packet), 2+11,0);
    	        location_tx_packet.length = 2 + 11;
    	        length = cc2420_write( cc, &(location_tx_packet),0);
    	        if(length != -1) break;
    	    }
    	    
    	    while(1) 
    	    {
    	        led_twinkle(LED_GREEN,2);
    	        length = cc2420_read( cc,&location_rx_packet,0);
	        if(length > 11) 
	        {
	            uart_write(g_uart, a2_string,26, 0);
	            
	            //anchor_2_x     = location_rx_packet.Payload[0];
	            //anchor_2_y     = location_rx_packet.Payload[1];
	            
	            lcs_rssi2dist(lcs,cc->rssi, &dis_2);
	            
	            //uart_putchar(g_uart,(char)anchor_2_x );
	            //uart_putchar(g_uart,(char)anchor_2_y );
	            
	            //uart_putchar(g_uart,(char)(location_rx_packet.srcAddr) );
	            //uart_putchar(g_uart,(char)(location_rx_packet.srcAddr >> 8) );
	            
	            uart_write(g_uart, rssi_string,20, 0);	            
	            temp = cc->rssi;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            
	            
	            uart_write(g_uart, dis_string,18, 0);
	            temp = (uint8)dis_2;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            
	            uart_putchar(g_uart,(char)*enter );
	            uart_putchar(g_uart,(char)*enter );
	            break;
	        }
    	    }
    	    
    	    hal_delay(2000);
    	    
    	    //send request to anchor_3, if received infor of anchor_3, then do next
    	    location_tx_packet.nodeto = 0x3333;
    	    while(1)
    	    {
    	    	led_twinkle(LED_RED,3);
    	        //length = cc2420_write( cc, &(location_tx_packet), 2+11,0);
    	        location_tx_packet.length = 2 + 11;
    	        length = cc2420_write( cc, &(location_tx_packet), 0);
    	        if(length != -1) break;
    	    }
    	    while(1) 
    	    {
    	        length = cc2420_read( cc,&location_rx_packet,0);
	        if(length > 11) 
	        {
	            uart_write(g_uart, a3_string,26, 0);
	            
	            //anchor_3_x     = location_rx_packet.Payload[0];
	            //anchor_3_y     = location_rx_packet.Payload[1];
	           
	            lcs_rssi2dist(lcs,cc->rssi, &dis_3);
	            
	            //uart_putchar(g_uart,(char)anchor_3_x );
	            //uart_putchar(g_uart,(char)anchor_3_y );
	            
	            //uart_putchar(g_uart,(char)(location_rx_packet.srcAddr) );
	            //uart_putchar(g_uart,(char)(location_rx_packet.srcAddr >> 8) );
	            
	            uart_write(g_uart, rssi_string,20, 0);	            
	            temp = cc->rssi;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            
	            
	            uart_write(g_uart, dis_string,18, 0);
	            temp = (uint8)dis_3;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            uart_putchar(g_uart,(char)*enter );
	            break;
	        }
    	    }
    	    
    	
    	    
    	    _localize(&(location->x),&(location->y),anchor_1_x,anchor_1_y,dis_1,anchor_2_x,anchor_2_y,dis_2,anchor_3_x,anchor_3_y,dis_3);
    	    uart_write(g_uart, location_string,24, 0);
    	    
    	    temp = (uint8)location->x;
	    if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	    temp = temp % 100;
	    if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	    temp = temp % 10;
	    uart_putchar(g_uart,(char)(temp + 48));
    	    
    	    uart_putchar(g_uart,(char) (*comma));
    	    
    	    temp = (uint8)location->y;
	    if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	    temp = temp % 100;
	    if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	    temp = temp % 10;
	    uart_putchar(g_uart,(char)(temp + 48));
    	    
    	    uart_putchar(g_uart,(char)(*enter));
    	    uart_putchar(g_uart,(char)(*enter));
    	    uart_putchar(g_uart,(char)(*enter));
    	    
    	    hal_delay(2000);
    }
    
    if(lcs->state & LOC_TYPE_ARCHOR)  //anchor node	
    {
    	led_twinkle(LED_GREEN,1);
    	length = cc2420_read( cc,&location_rx_packet,0);
	if(length > 11) 
	//if(length != 0)
    	{
    	   if(location_rx_packet.payload[0] == 8 && location_rx_packet.payload[1] == 8)
    	   {
    	      	location_tx_packet.nodeto = 0x1000;//location_rx_packet.srcAddr;
    	      	while(1)
    	        {
    	            
    	            led_twinkle(LED_YELLOW,1);	
    	            //length = cc2420_write( cc, &(location_tx_packet), 2+11,0);
    	            location_tx_packet.length = 2 + 11;
    	            length = cc2420_write( cc, &(location_tx_packet),0);
    	            if(length != -1) break;
    	        }
    	   } 	
    	}
    }
    
    return 0;
}
