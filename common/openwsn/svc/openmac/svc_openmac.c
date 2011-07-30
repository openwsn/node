/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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
 *****************************************************************************/ 

#include "svc_configall.h"
#include "svc_foundation.h"
#include "../hal/hal_led.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_global.h"
#include "svc_openmac.h"

#define MAC_DURATION_WAIT_CTS 200

/* ACK frame
 * [2B frame control][1B seqid][2B addrfrom][2B addrto]
 * 
 * RTS/CTS frame
 * [2B frame control][1B seqid][2B addrfrom][2B addrto][1B cmdtype]
 */
#define WLS_ACK_LENGTH 7
static char m_ackframe[7] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static char m_rtsframe[10] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static char m_ctsframe[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 

TOpenMAC * mac_construct( char * buf, uint16 size )
{
	TOpenMAC * mac = (TOpenMAC *)buf;
	//assert( sizeof(TOpenMAC) <= size );
	memset( mac, 0x00, sizeof(TOpenMAC) );
	return mac;
}

void  mac_destroy( TOpenMAC * mac )
{
	timer_stop( mac->timer );
}

void  mac_open( TOpenMAC * mac, TCc2420 * hdl, TActionScheduler * actsche, TTimer * timer, 
	TOpenAddress * addr ); 
{
	#ifndef GDEBUG
	char * msg = "mac_open() run...\n";
	uart_write( g_uart, msg, strlen(msg), 0x00 );
	#endif
	
	mac->state = MAC_STATE_IDLE;
	mac->event = MAC_EVENT_NULL;
	mac->retry = 0;
	mac->phy = hdl;
	//mac->actsche = actsche;
	mac->timer = timer;
	mac->seqno = 0;
	mac->txlen = 0;
	mac->rxlen = 0;
	mac->txframe = NULL;
	mac->rxframe = NULL;
	mac->backoff = 10;
	mac->backoff_rule = 2;
	mac->sleepduration = 0;
	memset( &(mac->txbuf[0]), 0x00, OPF_FRAME_SIZE );
	memset( &(mac->rxbuf[0]), 0x00, OPF_FRAME_SIZE );
	memset( mac->rxheader, 0x00, 7 );
	mac->timer = timer;
	timer_init( timer, 1, 0 );

}

void  mac_configure( TOpenMAC * mac, uint8 ctrlcode, uint16 value )
{
	cc2420_configure( mac->phy, ctrlcode, value, 0);
}

uint8 mac_read( TOpenMAC * mac, TOpenFrame * frame, uint8 size, uint8 opt )
{
	return mac_rawread( mac, (char *)frame, size, opt );
}

/* @param
 * @attention
 * 	the buffer "frame" will be used to hold the frame. if the "frame" buffer is 
 * not large enough to hold the whole frame, then this function will just drop 
 * the additional data.
 * 
 * @return
 * 	= 0		success. no frame was sent.
 * 	> 0		success. the value is the data actually sent. 
 * 	< 0		failed. however, this will never occur in current implementation.
 */
uint8 mac_rawread( TOpenMAC * mac, char * framebuffer, uint8 size, uint8 opt )
{
	uint8 copied = 0;
	mac_evolve( mac );
	if ((mac->state == MAC_STATE_IDLE) && (mac->rxlen))
	{
		copied = min( size, mac->rxlen );
		if (copied > 0)
		{
		        
			memmove( framebuffer, &(mac->rxbuf[0]), copied );
			memmove( &(mac->rxheader[0]), &(mac->rxbuf[0]), 9 ); // backup the header of the  														  
                                                   			// current frame for later using
			
			mac->rxlen = 0;
		}
	}
	//uart_putchar(g_uart,copied);	
	return copied;
}

uint8 mac_write( TOpenMAC * mac, TOpenFrame * frame, uint8 len, uint8 opt )
{
	return mac_rawwrite( mac, (char*)frame, len, opt );
}

/* @attention
 * 	the framebuffer's len should be less than MAC_FRAMEBUFFER_SIZE, so that 
 * the entire frame can be accept by mac layer. or some of the data will be lost!
 */  
uint8 mac_rawwrite( TOpenMAC * mac, char * framebuffer, uint8 len, uint8 opt )
{
	uint8 copied = 0;
	
	if ((mac->state == MAC_STATE_IDLE) && (mac->txlen == 0))
	{
		copied = min( OPF_FRAME_SIZE, len );
		if (copied > 0)
		{
			memmove( (char*)(mac->txbuf), framebuffer, copied );
			mac->txlen = copied;
		}
	}
	mac_evolve( mac );

	return copied;
}

/* if the mac layer is safe to sleep, then force it goto sleep state. 
 * sometimes, the mac layer cannot go to sleep due to active data, then this 
 * function does nothing.
 */
uint8 mac_sleep( TOpenMAC * mac )
{
	if ((mac->state == MAC_STATE_IDLE) && (mac->txlen ==0) && (mac->rxlen == 0))
	{
		_hdl_sleep( mac->phy );
		mac->state = MAC_STATE_PAUSE;
	}
	return 0;
}

/* wake up the mac layer if it is in sleep mode */
uint8 mac_wakeup( TOpenMAC * mac )
{
	if (mac->state == MAC_STATE_PAUSE)
	{
		_hdl_wakeup( mac->phy );
		mac->state = MAC_STATE_IDLE;
	}
	return 0;
}

/* evolve is a state transition function. it do state transition and perform 
 * necessary actions. however, it won't do these itself. the master module
 * should call it periodically (no matter timer driven of by a simple while loop) 
 * to drive it on.
 * 
 * @attention 20070130
 * some argue that the MAC software and the transceiver should goto SLEEP when
 * it received a RTS/CTS pair if the transmission destination is the current node
 * itself.  this may be true, but openwsn would NOT accept this due to hardware
 * restrictions. though such sleep seems resever energy, the transceive may not 
 * stable to perform so frequently switchings between sleep and active modes.
 * => more experiments needed to confirm this. 
 *  
 * the current design in OpenMAC is: the node not the destination will go to a 
 * PAUSE state. while, the transceive is still ON in PAUSE state so that the 
 * system can read/write frames as fast as possible. in this case, there's no 
 * transceiver startup time. this feature can improve the performance. 
 * the design of OpenMAC (openwsn@gmail.com) thinks that the SLEEP/WAKEUP mechanism
 * should be implemented by a separate module svc_energy. this service will 
 * control the energy behavior of whole system rather than the OpenMAC itself does.
 */

#ifdef CONFIG_OPENMAC_SIMPLE
int8 mac_evolve( TOpenMAC * mac )
{
	boolean done = TRUE;
	uint16 addr;
	uint8 id;
	int8 count = 0, ret = 0;
	int8 failed;
	
	do{
		switch (mac->state)
		{	
		// IDLE is the initial state of the state machine.
		case MAC_STATE_IDLE:
		
			if (mac->txlen > 0)
			{
				mac->state = MAC_STATE_SENDING;
				done = FALSE;
			}
			else{
				
				mac->state = MAC_STATE_RECVING;
				done = FALSE;
			}
			break;
			
		// start try to receving data.
		// this state is not a stable state. it will transite to IDLE or WAITDATA
		// state quickly.   
		//
		// if there's no frame received, then go back to IDLE state. this feature
		// enable the user be able to continue call rawread()/rawwrite() successfully.
		//
		// the source code in the following is actually part of the IDLE state.
		// though the source code in this state can be merged with the state IDLE, 
		// but i still keep them here because this separation facilitates 
		// the interaction with the interrupt routine. the interrupt can simply change 
		// state variable to MAC_STATE_RECEIVING to trigger the following 
		// processings without introducing other side effects.
		// 
		// assume: the ISR place the received packet in the buffer.
		// 
		case MAC_STATE_RECVING:
		   
			// the interrupt sevice routine will place the received frame into
			// the RX buffer and change the value of mac->rxlen.
			//
			// if the master module hasn't read the data out, then MAC layer
			// will pause to read more data from PHY layer. this improves the 
			// efficiency. however, this may also lead data loss in the PHY layer. 
			// but we have no good idea to solve this, unless the master module
			// can call mac_read() more frequently. the possibility of data loss
			// will always exists! you cannot eliminate it by only allocate  
			// large buffer.
			//
		if (mac->rxlen == 0)
		{
			
			count = _hdl_rawread( mac->phy, mac->rxbuf, OPF_FRAME_SIZE, 0x00 );
			
			mac->rxlen = count;
			if (count == 0)
			{
				led_twinkle(LED_YELLOW,1);
				mac->state = MAC_STATE_IDLE;
				done = true;
			}
			
		}
		mac->state = MAC_STATE_IDLE;
		done = true;
		break;
		
		
		// start frame sending process
		// this state is only a transition state. the system will perform some 
		// actions in this state and then quickly goes into TX_DELAY state. 
		// the state machine will not stay in this state.
		//
		// before you try to start sending, you must delay for a random time to 
		// avoid collison. this is also called "backoff time". 
		//
		case MAC_STATE_SENDING:
			if (mac->txlen == 0)
			{
				mac->state = MAC_STATE_IDLE;
			}
			else{
				mac->state = MAC_STATE_TX_DELAY;
				mac->backoff = timer_getvalue( g_timer1 );
				uart_putchar(g_uart,(char)(mac->backoff));
				done = false;
			}
			break;
				
		case MAC_STATE_TX_DELAY:
		                hal_delay(mac->backoff);
				_hdl_rawwrite( mac->phy, mac->txbuf, mac->txlen, 0x00 );
				mac->state = MAC_STATE_IDLE;
				done = true;		
			break;
		
		// if the state machine is in any other state, then the following code
		// will drag it to IDLE state.
		//
		default:
			mac->state = MAC_STATE_IDLE;
			break;
		}
	}while (!done);
	
	return ret; 
}
#endif

#ifdef CONFIG_OPENMAC_FULL
int8 mac_evolve( TOpenMAC * mac )
{
	boolean done = TRUE;
	uint16 addr;
	uint8 id;
	int8 count = 0, ret = 0;
	int8 failed;
	
	do{
		switch (mac->state)
		{
		// @attention: in current implementation of OpenMAC, the transceiver 
		// will be still ON in PAUSE mode. and the MCU chip is active too.
		// 
		case MAC_STATE_PAUSE:
			if (timer_expired(mac->timer))
			{
				_hdl_wakeup( mac->phy );
				mac->state = MAC_STATE_IDLE;
				done = FALSE;
			}
			break;
			
		// IDLE is the initial state of the state machine.
		case MAC_STATE_IDLE:
		
			if (mac->txlen > 0)
			{
				mac->state = MAC_STATE_SENDING;
				done = FALSE;
			}
			else{
				
				mac->state = MAC_STATE_RECVING;
				done = FALSE;
			}
			break;
			
		// start try to receving data.
		// this state is not a stable state. it will transite to IDLE or WAITDATA
		// state quickly.   
		//
		// if there's no frame received, then go back to IDLE state. this feature
		// enable the user be able to continue call rawread()/rawwrite() successfully.
		//
		// the source code in the following is actually part of the IDLE state.
		// though the source code in this state can be merged with the state IDLE, 
		// but i still keep them here because this separation facilitates 
		// the interaction with the interrupt routine. the interrupt can simply change 
		// state variable to MAC_STATE_RECEIVING to trigger the following 
		// processings without introducing other side effects.
		// 
		// assume: the ISR place the received packet in the buffer.
		// 
		case MAC_STATE_RECVING:
		   
			// the interrupt sevice routine will place the received frame into
			// the RX buffer and change the value of mac->rxlen.
			//
			// if the master module hasn't read the data out, then MAC layer
			// will pause to read more data from PHY layer. this improves the 
			// efficiency. however, this may also lead data loss in the PHY layer. 
			// but we have no good idea to solve this, unless the master module
			// can call mac_read() more frequently. the possibility of data loss
			// will always exists! you cannot eliminate it by only allocate  
			// large buffer.
			//
		if (mac->rxlen == 0)
		{
			
			count = _hdl_rawread( mac->phy, mac->rxbuf, OPF_FRAME_SIZE, 0x00 );
			
			mac->rxlen = count;
			if (count == 0)
			{
				led_twinkle(LED_YELLOW,1);
				mac->state = MAC_STATE_IDLE;
				done = true;
			}
			
			else{
				switch (opf_type(mac->rxbuf))
				{
				/*
				case OPF_TYPE_DATA:
					// this is a data frame and do nothing now. just wait for the 
					// master program retrieve the data out.
					// @warning: the newest incoming frame may override the last 
					// frame. so the mater should retrieve the frame as fast as 
					// possible.
					//
					// @TODO
					// do check sum here
					// if checksum successfully, then return ACK or else NAK
					// do checksum for ACK frame
					//
					// @attention
					// the PHY layer 2420 chip can decide whether sending ACK 
					// or not. so we can elimate such operations. 
					//
					// m_ackframe[1] |= 0x01;
					// m_ackframe[2] = mac->rxbuf[2]; // update the sequence number
					// m_ackframe[3] = 0x00; // @TODO checksum
					// m_ackframe[4] = 0x00;
					// cc2420_rawwrite( mac->phy, (char*)(&(m_ackframe[0])),12, 0x00 );  //12 is len, please modify
					//
					mac->state = MAC_STATE_IDLE;
					break;
					
				case OPF_TYPE_ACK:
					// ACK frame received. 
					// if the sequence id in the ACK packet equal to or larger than 
					// the sequence id of the frame in the sending buffer.
					// (rxbuf[2] and txbuf[2] contain the sequence id)
					//
					if (mac->rxbuf[2] >= mac->txbuf[2])
					{
						mac->seqno = txbuf[2]; //?
						mac->retry = 0;
						mac->txlen = 0;
						//acts_cancelaction( mac->actsche, mac->waitack_action );
						mac->state = MAC_STATE_IDLE;
					}
					break;
					
				case 0x03:
					// NAK frame received. then try to re-transmission the frame.
					if (mac->rxbuf[2] == mac->txbuf[2])
					{
						mac->retry ++;
						//acts_cancelaction( mac->actsche, mac->waitack_action );
						mac->nextstate = MAC_STATE_SENDING;
						done = FALSE;
					}
					break;
				*/
					
				/*
				case OPF_TYPE_MACCMD:
					break;
					
				case OPF_TYPE_RTS:
					addr = opf_addrto(mac->rxbuf);
					if (addr == mac_getshortid(&(mac->localaddr)))
					{  
						mac->state = MAC_STATE_RECVING;
						done = FALSE;
					}
					else{
						mac->sleepduration = (uint8)(* opf_msdu( &(m_rtsframe[0]) ));
						mac->rxlen = 0;
						timer_setinterval( mac->timer, mac->sleepduration, 0x00 );
						mac->state = MAC_STATE_PAUSE;
					}
					break;
					
				case OPF_TYPE_CTS:
					break;
				*/	
				default:
					NULL;
				}
				
				done = true;
			}
		}
		mac->state = MAC_STATE_IDLE;
		done = true;
		break;
			
		// IDEL => RECVING: event = RTS arrival
		// MAC will goto RECVING state when it received a RTS frame
		// 
		case MAC_STATE_RX_SENDCTS:
			// assume you have received the RTS frame, then you should reply 
			// CTS back
			//
			opf_setaddrfrom( m_ctsframe, mac_getshortid(&(mac->localaddr)) );
			opf_setaddrto( m_ctsframe, opf_addrto(m_rtsframe) );
			
			count = _hdl_rawwrite( mac->phy, m_ctsframe, sizeof(m_ctsframe), 0x00 );
			if (count > 0)
			{
				mac->sleepduration = 0;		
				mac->state = MAC_STATE_RX_WAITDATA;
				timer_stop( mac->timer );
				timer_setinterval( mac->timer, 2000, 0 ); // maximum duration to wait data frame
				done = FALSE;		
			}
			
		case MAC_STATE_RX_WAITDATA:
			failed = false;
			count = _hdl_rawread( mac->phy, mac->rxbuf, OPF_FRAME_SIZE, 0x00 );
			if (count == 0)
			{
				if (timer_expired(mac->timer))
					failed = true;
			}			
			
			if ((!failed) && (count > 0))
			{
				//timer_stop( mac->timer );

				id = opf_seqid(mac->rxbuf);
				if ((id <= mac->seqno) || ((id == 0xFF) && (mac->seqno == 0)))
				{
					// just drop the packet and continue wait
				}
				else{
					mac->seqno = id;
					mac->rxlen = count;					
					//send ACK/NAK if necessary
					mac->state = MAC_STATE_IDLE;
				}		
			}
			break;
		
		// start frame sending process
		// this state is only a transition state. the system will perform some 
		// actions in this state and then quickly goes into TX_DELAY state. 
		// the state machine will not stay in this state.
		//
		// before you try to start sending, you must delay for a random time to 
		// avoid collison. this is also called "backoff time". 
		//
		case MAC_STATE_SENDING:
		uart_putchar(g_uart,1);
			if (mac->txlen == 0)
			{
				mac->state = MAC_STATE_IDLE;
			}
			else if (mac->retry > OPENMAC_RETRY_LIMIT)
			{
				// this case should better not to happen! because this means a frame
				// relly lost during transmission. your master application should 
				// consider this problem!
				mac->txlen = 0;
				mac->state = MAC_STATE_IDLE;
			}
			else{
			        uart_putchar(g_uart,2);
				mac->state = MAC_STATE_TX_DELAY;
				// 指数退避算法
				mac->backoff = mac->backoff << 2;
				//actsche->inputaction ?;
				timer_stop( mac->timer );
				timer_setinterval( mac->timer, mac->backoff, 0 );
				timer_start( mac->timer );
				done = false;
			}
			break;
				
		case MAC_STATE_TX_DELAY:
		uart_putchar(g_uart,3);
			if (timer_expired( mac->timer ))
			{
				uart_putchar(g_uart,4);
				//_hdl_rawwrite( mac->phy, m_rtsframe, sizeof(m_rtsframe), 0x00 );
				_hdl_rawwrite( mac->phy, mac->txbuf, mac->txlen, 0x00 );
				//timer_setinterval( mac->timer, MAC_DURATION_WAIT_CTS, 0 );	
				//mac->state = MAC_STATE_TX_WAITCTS;
				mac->state = MAC_STATE_IDLE;
				done = true;
			}			
			break;
		/*				
		case MAC_STATE_TX_WAITCTS:
			if (!timer_expired(mac->timer))
			{
				memset( mac->rxbuf, 0x00, OPENMAC_BUFFER_SIZE );
				count = _hdl_rawread( mac->phy, mac->rxbuf, OPENMAC_BUFFER_SIZE, 0x00 );
				// @TODO if mac->txbuf is a valid CTS
				if ((count > 0) && (true))
				{
#ifdef OPENMAC_SUPPORT_ACK
					count = _hdl_rawwrite( mac->phy, mac->txbuf, mac->txlen, 0x00 );
					// post assertion: the packet should always be sent successfully 
					// by hal driver.
					assert( count > 0 );
					timer_stop( mac->timer );
					timer_setinterval( MAC_INTERVAL_WAITACK );
					mac->state = MAC_STATE_TX_WAITACK;
					done = FALSE;
#endif

#ifndef OPENMAC_SUPPORT_ACK
					// @attention: for our cc2420 transceive, it can support ACK
					// mechanism in the hardware. so faciliate the developing of 
					// MAC software. 
					count = _hdl_rawwrite( mac->phy, mac->txbuf, mac->txlen, 0x00 | 0x01 );
					if (count <= 0)
					{
						mac->state = MAC_STATE_SENDING;
					}
					else{
						mac->state = MAC_STATE_IDLE;
					}
					done = FALSE;
#endif
				}
			}
			break;
			
		case MAC_STATE_TX_WAITACK:
			id = 0;
			memset( mac->rxbuf, 0x00, OPENMAC_BUFFER_SIZE );
			count = _hdl_rawread( mac->phy, mac->rxbuf, OPENMAC_BUFFER_SIZE, 0x00 );
			// if this is a ACK frame, other case NAK frame
			if (true)
			{
				id = 1;
			}
			else if (false)
			{
				id = 2;
			}
			else{
				if (timer_expired(mac->timer))
					id = 3;
			}
			
			switch (id)
			{
			case 1:
				timer_stop( mac->timer );
				mac->txlen = 0;
				mac->state = MAC_STATE_IDLE;
				break;
			case 2:
			case 3:
				timer_stop( mac->timer );
				mac->retry ++;
				mac->state = MAC_STATE_SENDING;
				break;
			}
			done = FALSE;
			break;
                       */
		// if the state machine is in any other state, then the following code
		// will drag it to IDLE state.
		//
		default:
			mac->state = MAC_STATE_IDLE;
			break;
		}
	}while (!done);
	
	return ret; 
}
#endif

uint8 mac_state( TOpenMAC * mac )
{
	return mac->state;
}
/*

int8 mac_state_machine_evolve( TOpenMAC * mac )
{
	boolean done = TRUE;
	int8 ret = 0;
	
	do{
		switch (mac->transtate)
		{
		case MAC_IDLE_STATE:
			if (mac->nextstate == MAC_RECVING_STATE)
			{
				// @TODO
				// start the data receiving process
				mac->state = MAC_RECVING_STATE;
				done = FALSE;
			}
			else if (mac->nextstate == MAC_SENDING_STATE)
			{
				mac->state = MAC_SENDING_STATE;
				done = FALSE;
			}
			break;
			
		case MAC_RECVING_STATE:
			// @TODO
			// try to receive data from the wireless driver
			// and judge whether an entire frame received
			cc2420_read()
			
			if (entire frame received)
			{
				if DATA frame
				{
					// prepare ACK or NAK frame
					mac->state = MAC_SENDING_ACK
					done = FALSE;
				}
				else if CTS Frame
				{
					mac->state = MAC_SENDING_DATA
					done = FALSE;
				}
				else if ACK frame
				{
					//clear 有关变量
					mac->state = MAC_IDLE;
				}
				else if NAK frame
				{
					mac->state = MAC_SENDING_DATA
					done = FALSE;
				}
			}
			break;
			
		// 这个状态其实是启动发送事务，但是不能立刻发送数据，还要等待一段随机时候后才允许
		// 发送RTS
		case MAC_SENDING_STATE:
			if (mac->retry_count) >= 3
			{
				取消各计时action
				reset各有关变量
				mac->state = MAC_IDLE_STATE;
			}
			else{
				//启动RTS发送延迟，延迟时间根据backoff_rule计算
				// for random backoff
				backoff = random( 0, backoff_rule );
				// for square backoff
				backoff = backoff * 2;
				if (backoff == 0) 立即重传
				{
					设置各变量准备发送;
					mac->state = MAC_SENDING_RTS;
					done = FALSE;
				}
				else{
					mac->rts_delay_action = acts_inputactioin( mac->actsche, 发送RTS延迟 )
					设置各变量准备发送;
					mac->state = MAC_SENDING_RTS:
					done = FALSE;
				}
			}
			 
		case MAC_SENDING_RTS:
			if (RTS发送延迟时间到)
			{
				cc2420_write 发送RTS
				启动CTS等待计时
				mac->state = MAC_WAIT_CTS;
				done = FALSE;
			}
			break;
			
		case MAC_WAIT_CTS:
			cc2420_read 查是否收到CTS
			if yes
			{
				acts_cancel( CTS等待Action );
				mac->state = MAC_SENDING_DATA;
				done = FALSE;
			}
			else if acts_expired(CTS等待计时)
			{
				mac->state = MAC_SENDING_RTS;
				done = FALSE;
			}
			break;
		
		case MAC_SENDING_DATA:
			cc2420_write 发送数据
			if (发送完毕)
			{
				mac->state = MAC_RECVING;
				done = FALSE;
			}
			
		default
			mac->state = MAC_RECVING;
		}

		
	}
	while (!done)
}
*/

uint8 mac_setrmtaddress( TOpenMAC * mac, TOpenAddress * addr )
{
	return 0;
}

uint8 mac_setlocaladdress( TOpenMAC * mac, TOpenAddress * addr )
{
	return 0;
}

uint8 mac_getrmtaddress( TOpenMAC * mac, TOpenAddress * addr )
{
	return 0;
}

uint8 mac_getlocaladdress( TOpenMAC * mac, TOpenAddress * addr )
{
	return 0;
}

uint8 mac_installnotify( TOpenMAC * mac, TEventHandler * callback, void * owner )
{
	return 0;
}
