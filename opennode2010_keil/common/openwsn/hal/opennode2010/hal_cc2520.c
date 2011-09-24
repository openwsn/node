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

#include "../hal_configall.h"
#include <string.h>
#include <stdio.h>
#include "../hal_foundation.h"
#include "../hal_cpu.h"
#include "../hal_interrupt.h"
#include "../hal_assert.h"
#include "../hal_targetboard.h"
#include "../hal_led.h"
#include "../hal_uart.h"
#include "../hal_debugio.h"
#include "../hal_mcu.h"
#include "../hal_digitio.h"
#include "../hal_interrupt.h"
//#include "../rtl/rtl_iobuf.h"
//#include "../rtl/rtl_ieee802frame154.h"

/* In "hal_cc2520base.h", we implement the most fundamental cc2420 operating functions.
 * If you want to port hal_cc2520 to other platforms, you can simply revise the 
 * hal_cc2520vx.h. The other part inside hal_cc2520.h can keep unchanged.
 */
#include "hal_cc2520vx.h"
#include "hal_cc2520base.h"
#include "../hal_cc2520.h"

static void _cc2520_fifop_handler(void * object, TiEvent * e);
static uint8 _cc2520_write_txbuf( TiCc2520Adapter *cc, char * buf, uintx len );
static uint8 _cc2520_read_rxbuf( TiCc2520Adapter *cc, char * buf, uintx capacity );

TiCc2520Adapter * cc2520_construct( void * mem, uint16 size )
{
	hal_assert( sizeof(TiCc2520Adapter) <= size );
	memset( mem, 0x00, size );
	return (TiCc2520Adapter *)mem;
}

void cc2520_destroy( TiCc2520Adapter * cc )
{
	cc2520_close( cc );
}

/**
 * @attention Suggest disable global interrupt before calling this function.
 * 
 * @param id Require always 0 now. (Because currently we have only one cc2520 in 
 *           the board)
 */
TiCc2520Adapter * cc2520_open( TiCc2520Adapter * cc, uint8 id, TiFunEventHandler listener, 
	void * lisowner, uint8 option )
{
    cc->id = 0;
    cc->state = 0; //CC2420_STATE_RECVING;
    cc->listener = listener;
    cc->lisowner = lisowner;
    cc->option = option;
	//cc->rssi = 0;
	//cc->lqi = 0;
	//cc->spistatus = 0;
	cc->rxlen = 0;

    CC2520_ACTIVATE();

	CC2520_SPI_OPEN();


	halRfInit();//todo 设置相应的寄存器

	halRfSetPower( TXPOWER_4_DBM);
	halRfSetChannel( CC2520_DEF_CHANNEL);
	halRfSetShortAddr( CC2520_DEF_LOCAL_ADDRESS);
	halRfSetPanId( CC2520_DEF_PANID);

	CC2520_SRFOFF();
	CC2520_SRXON();
    
    // Map the cc2520 FIFOP interrupt to cc2520 FIFOP handler. This is done inside
    // hal_interrupt module and hal_foundation module.
	//
    // @modified by zhangwei on 2011.09.24
    // - You can also use macro constant INTNUM_FIFOP. It's equal to INTNUM_FRAME_ACCEPTED.
	hal_attachhandler( INTNUM_FRAME_ACCEPTED, _cc2520_fifop_handler, cc );
    
    // By default, the TiCc2520Adapter is interrupt driven. The interrupt handler
    // _cc2520_fifop_handler() will retrieve the frame from the transceiver hardware
    // into the adapter's internal RX buffer. 
    // 
    // In order to make the receiveing process work, you should enable the FIFOP
    // interrupt so that the FIFOP request can activate the handler function _cc2520_fifop_handler().
    
	CC2520_ENABLE_FIFOP();

    return cc;
}

/**
 * Send the data in the buffer. The data should already be organized as an complete
 * frame required by cc2520 transceiver. The first byte should be the length byte
 * according to 802.15.4 frame format. So buf[0] should equal to (len-1).
 * 
 * @param buf The data frame.
 * @param len The data length in the buffer. 
 * @param option
 *      1: require ACK. (default)
 *      0: No ACK require.
 *
 * @return 
 *      > 0     the counts of bytes really sent
 *      = 0     nothing is sent
 *      < 0     I/O error.
 */
intx cc2520_send( TiCc2520Adapter * cc, char * buf, uintx len, uint8 option )
{
	intx count;
	uint8 status;
    TiCpuState cpu_state;

	hal_assert( len > 0 );

	if (option)
	{
		buf[1] = buf[1]|0x20;
	}
    
    // set the frame length according to 802.15.4 frame format. Attention we'd better
    // set buf[0] here, which is the frame length. This is because this byte may
    // not be set by the above layer.
    buf[0] = len-1;
	
    // @todo
    // Wait for the last sending finished. Delay isn't recommend here.
	CC2520_SFLUSHTX();
	hal_delayus(50); // todo
				
	// todo: check whether the last sending is complete
	
    // @attention You should use critical area management here because the sending
    // process doesn't hope other interrupt to disturbe it.
	hal_enter_critical();
    CC2520_TXBUF( len, buf);
	//count = _cc2420_writetxfifo( cc, (char*)&(buf[0]), len, option );
	hal_leave_critical();
  	
    hal_delayms(1);
	CC2520_STXON();
	hal_delayms(1);
	
	return len;
}

intx cc2520_broadcast( TiCc2520Adapter * cc, char * buf, uintx len, uint8 option )
{
	intx count;
	
	TiCpuState cpu_state;

	hal_assert( len > 0 );

    // Set the frame control for broadcast. Bit 5 in frame control byte 0 is cleared.
    // 1 in this bit means ACK required.

	buf[1] = buf[1] & 0xdf;
    buf[0] = len-1;

    // @todo
    // Wait for the last sending finished. Delay isn't recommend here.
	CC2520_SFLUSHTX();
	hal_delayus( 50);

	//count = _cc2420_writetxfifo( cc, (char*)&(buf[0]), len, option );
	hal_enter_critical();
	CC2520_TXBUF( len,buf);
	hal_leave_critical();
	CC2520_STXON();
	hal_delayms(1);

	count = len;
	return count;
}

uint8 _cc2520_write_txbuf( TiCc2520Adapter *cc, char * buf, uintx len )
{
    return 0;
}

intx cc2520_recv( TiCc2520Adapter * cc, char * buf, uintx size, uint8 option )
{
	intx ret = 0;
    TiCpuState cpu_state=0;
		
    // Read data out from the cc->rxbuf. Usually the FIFOP interrupt service routine
    // place accepted frame into the rxbuf for this reading.
    
	hal_enter_critical();
	// cc->rxlen should equal to cc->rxbuf[0] + 1 for correct frames
	if (cc->rxlen > 0)
	{
        // cc->rxlen includes the frame length byte in the cc->rxbuf[0], so it should 
        // equal to rxbuf[0]+1 for correct frames. but the frame data maybe incorrect.
		if (cc->rxlen <= size)
		{
			memmove( (void *)buf, (void *)(&(cc->rxbuf[0])), cc->rxlen );
			ret = cc->rxlen;
			cc->rxlen = 0;
		}
		else{
			ret = 0;
			cc->rxlen = 0;
		}
    }
    hal_leave_critical();

    // If the rxbuf is empty, then we should check for the transceiver to see whether
    // there's frame pending for reading.
    
    hal_enter_critical();
    if (ret == 0)
    {
		ret = (intx)_cc2520_read_rxbuf(cc, buf, size);
    }
    hal_leave_critical();

	return ret;
}

uint8 _cc2520_read_rxbuf( TiCc2520Adapter *cc, char * buf, uintx capacity )
{
	intx ret;
	uint8 state;

	ret = 0;

    //if rxfifo overflow.
	if (CC2520_REGRD8(CC2520_EXCFLAG0) & 0x40)
	{
		buf[0] = CC2520_REGRD8( CC2520_RXFIFOCNT );
		if (buf[0] > 0)
		{
			CC2520_RXBUF(buf[0], (uint8*)(buf+1));
			ret = buf[0]+1;
		}

		CC2520_SFLUSHRX();
		CC2520_SFLUSHRX();

        // clear the exception flag manually
		CC2520_REGWR8(CC2520_EXCFLAG0, 0x00);
    }
    else{
        // judge whether the frame arrived
		state = CC2520_REGRD8(CC2520_EXCFLAG1);
		if (state & 0x01) //if (( state && 0x01) && ( state && 0x10)) 
		{
            // buf[0] is the frame length.
			buf[0] = CC2520_RXBUF8();
			if (buf[0] > 0)
			{
				CC2520_RXBUF(buf[0], (uint8 *)(buf+1));
				ret = buf[0]+1;
			}
			else{
				CC2520_SFLUSHRX();
				CC2520_SFLUSHRX();
			}

            // clear the exception manually
			CC2520_REGWR8(CC2520_EXCFLAG1, 0x00);
		}
	}
	return ret;
}

TiFrameTxRxInterface * cc2520_interface( TiCc2520Adapter * cc, TiFrameTxRxInterface * intf )
{
	memset( intf, 0x00, sizeof(TiFrameTxRxInterface) );
	intf->provider = cc;
	intf->send = (TiFunFtrxSend)cc2520_send;
	intf->recv = (TiFunFtrxRecv)cc2520_recv;
	intf->evolve = (TiFunFtrxEvolve)cc2520_evolve;
	
	intf->switchtomode = (TiFunFtrxSwitchToMode)cc2520_switchtomode;
	intf->ischnclear = (TiFunFtrxIsChannelClear)cc2520_ischannelclear;
	intf->enable_autoack = (TiFunFtrxEnableAutoAck)cc2520_enable_autoack;
	intf->disable_autoack = (TiFunFtrxDisableAutoAck)cc2520_disable_autoack;
	intf->enable_addrdecode = (TiFunFtrxEnableAddrDecode)cc2520_enable_addrdecode;
	intf->disable_addrdecode = (TiFunFtrxDisableAddrDecode)cc2520_disable_addrdecode;
	intf->setchannel = (TiFunFtrxSetChannel)cc2520_setchannel;
	intf->setpanid = (TiFunFtrxSetPanId)cc2520_setpanid;
	intf->getpanid = (TiFunFtrxGetPanId)cc2520_getpanid;
	intf->setshortaddress = (TiFunFtrxSetShortAddress)cc2520_setshortaddress;
	intf->getshortaddress = (TiFunFtrxGetShortAddress)cc2520_getshortaddress;
	intf->settxpower = (TiFunFtrxSetTxPower)cc2520_settxpower;
	intf->getrssi = (TiFunFtrxGetRssi)cc2520_rssi;
    intf->setlistener = ( TiFunFtrxSetlistener)cc2520_setlistener;//todo for testing
	return intf;
}


/*
void EXTI0_IRQHandler(void)//fifop中断函数  fifphandler指向该中断,不知道handler（）函数还能不能用？
{

	TiCc2520Adapter * cc = &m_cc;

	hal_delayms(1);
	__disable_irq();
    */

	/*

	if ( CC2520_REGRD8( CC2520_EXCFLAG0)&0x40)//if rxfifo overflow.
	{
		m_cc.rxlen =  CC2520_REGRD8( CC2520_RXFIFOCNT);
		m_cc.rxbuf[0] = m_cc.rxlen;
        
		CC2520_RXBUF( m_cc.rxlen,m_cc.rxbuf+1);
		CC2520_SFLUSHRX();
		CC2520_SFLUSHRX();
		CC2520_REGWR8(CC2520_EXCFLAG0,0x00);
	}

	if (CC2520_REGRD8( CC2520_EXCFLAG1)&0x11)
	{
		USART_Send( 0xc0);
		m_cc.rxlen = CC2520_RXBUF8();
        if ( m_cc.rxlen>0)
        {
			USART_Send( 0xd0);
			m_cc.rxbuf[0] = m_cc.rxlen;

			CC2520_RXBUF( m_cc.rxlen,m_cc.rxbuf+1);
        }
		else
		{
			CC2520_SFLUSHRX();
			CC2520_SFLUSHRX();
		}
		
        CC2520_REGWR8(CC2520_EXCFLAG1,0x00);//todo clear the exception
	}*/
    /*
	cc->rxlen = (uint8)_cc2520_read_rxbuf( cc, &cc->rxbuf[0], 128 );


	__enable_irq();
	
    EXTI_ClearITPendingBit(EXTI_Line0);
}
*/

void cc2520_evolve( TiCc2520Adapter * cc )
{
	/*
	if (cc->rxlen > 0) 
	{
		if (cc->listener != NULL)
			cc->listener( cc->lisowner,NULL );
	}
	*/
	// the following section will check flag variable and try to read data from 
	// cc2420 hardware to MCU's memory when there's incomming frame pending inside
	// cc2420
	//
	//if ((m_fifop_request > 0) && (m_rxbuf_len == 0))
	//{
		// @todo
		//cc2420_readrxfifo( cc );
		//m_fifop_request --;
		//m_fifop_request = 0;
	//}
}

void cc2520_default_listener( void * ccptr, TiEvent * e )
{

}

void  cc2520_switchtomode( TiCc2520Adapter * cc, uint8 mode )
{

}

void cc2520_setlistener(TiCc2520Adapter * cc, TiFunEventHandler listener, void * lisowner )
{
    cc->listener = listener;
    cc->lisowner = lisowner;
}

void cc2520_close( TiCc2520Adapter * cc )//LPM2
{
    CC2520_SRES();
    GPIO_ResetBits( GPIOB,GPIO_Pin_5);//reset the VREG_EN
}

void cc2520_sleep( TiCc2520Adapter * cc )//LPM1
{
    CC2520_SXOSCOFF();	
}

void cc2520_restart( TiCc2520Adapter * cc )
{
     GPIO_SetBits( GPIOB,GPIO_Pin_5);//set the VREG_EN
     GPIO_ResetBits( GPIOB,GPIO_Pin_1);//reset the cc2520 nRST
     hal_delayms( 5);//wait for the regulator to be stable.
     GPIO_SetBits( GPIOB,GPIO_Pin_1);//set the cc2520 nRST
     GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
     while ( !GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14));
     GPIO_SetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
}

void cc2520_wakeup( TiCc2520Adapter * cc )
{
    CC2520_SXOSCON();
    CC2520_SNOP();
    GPIO_ResetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
    while ( !GPIO_ReadInputDataBit( GPIOB,GPIO_Pin_14));
    GPIO_SetBits( GPIOB,GPIO_Pin_12);//reset the cc2520 CSn
}

uint8 cc2520_state( TiCc2520Adapter * cc )
{
	return cc->state;
}

/* turn on the cc2520 VREF
 * attention you should wait long enough to guarantee the voltage is stable and ok 
 */
inline uint8 cc2520_vrefon( TiCc2520Adapter * cc )
{
    GPIO_SetBits( GPIOB,GPIO_Pin_5);//set the VREG_EN                    //turn-on  
    hal_delayus( 1800 );    
	return 0;
}

/* turn off the cc2520 VREF */
inline uint8 cc2520_vrefoff( TiCc2520Adapter * cc )
{
	 GPIO_ResetBits( GPIOB,GPIO_Pin_5);//reset the VREG_EN                    //turn-off  
    hal_delayus( 1800 );  
	return 0;
}

inline uint8 cc2520_powerdown( TiCc2520Adapter * cc )
{
	return cc2520_vrefoff( cc );
}

/* Reference
 *	- cc2520 datasheet: state machine of cc2520.
 */
inline uint8 cc2520_powerup( TiCc2520Adapter * cc )// can be placed by cc2520_wakeup or cc2520_restart.
{
	return cc2520_vrefon( cc );
}

void  cc2520_setcoordinator( TiCc2520Adapter * cc, bool flag )
{
    if (flag)//Coordinator 
        CC2520_BSET(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT0, 1));
    else    //other device type 
        CC2520_BCLR(CC2520_MAKE_BIT_ADDR(CC2520_FRMFILT0, 1));
}

uint8 cc2520_ischannelclear( TiCc2520Adapter * cc )
{
   uint8 cca;
   if ( CC2520_REGRD8( CC2520_FSMSTAT1)&0x10)
   {
       cca = 0x01;
   }
   else
   {
       cca = 0x00;
   }
   return cca;
}

/*The statusbit RSSI_VALID should be checked before reading the RSSI value register. RSSI_VALID 
*indicates that the RSSI value in the register is in fact valid, which means that the receiver has been enabled 
*for at least 8 symbol periods. 
*/
uint8 cc2520_rssi( TiCc2520Adapter * cc )
{
     return CC2520_REGRD8( CC2520_RSSI);
}

uint8 cc2520_calibrate( TiCc2520Adapter * cc )
{
      return CC2520_STXCAL();
}

uint8 cc2520_snop( TiCc2520Adapter * cc )
{
    return CC2520_SNOP();
}

uint8 cc2520_oscon( TiCc2520Adapter * cc )
{
    return CC2520_SXOSCON();
}

uint8 cc2520_oscoff( TiCc2520Adapter * cc )
{
    return CC2520_SXOSCOFF();
}


uint8 cc2520_rxon( TiCc2520Adapter * cc )
{
    return CC2520_SRXON();
}

uint8 cc2520_txon( TiCc2520Adapter * cc )
{
    return CC2520_STXON();
}

uint8 cc2520_txoncca( TiCc2520Adapter * cc )
{
    return CC2520_STXONCCA();
}

uint8 cc2520_rfoff( TiCc2520Adapter * cc )
{
    return CC2520_SRFOFF();
}

uint8 cc2520_flushrx( TiCc2520Adapter * cc )
{
    CC2520_SFLUSHRX();
	return 0;
}

uint8 cc2520_flushtx( TiCc2520Adapter * cc )
{
    CC2520_SFLUSHRX();
	return 0;
}

uint8 cc2520_writeregister( TiCc2520Adapter * cc, uint8 addr, uint8 data)
{
     CC2520_REGWR8(addr,data);
     return 0;
}

uint8 cc2520_readregister( TiCc2520Adapter * cc, uint8 addr)
{
    return CC2520_REGRD8( addr);
}


uint8 cc2520_enable_autoack( TiCc2520Adapter * cc )
{
    return CC2520_BSET(CC2520_MAKE_BIT_ADDR(CC2520_FRMCTRL0, 5));
}

uint8 cc2520_disable_autoack( TiCc2520Adapter * cc )
{
    return CC2520_BCLR( CC2520_MAKE_BIT_ADDR( CC2520_FRMCTRL0,5));
}

uint8 cc2520_enable_addrdecode( TiCc2520Adapter * cc )
{
    return CC2520_BSET( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT0,0));
}

uint8 cc2520_disable_addrdecode( TiCc2520Adapter * cc )
{
    return CC2520_BCLR( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT0,0));
}

uint8 cc2520_enable_filter( TiCc2520Adapter * cc )
{
     return CC2520_BSET( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT0,0));
}

//以下5组函数只有在filter功能实现的时候才有意义
//define whether the reserved frames are accepted or rejected
uint8 cc2520_reserved_accept( TiCc2520Adapter * cc )
{
     return CC2520_BSET( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,7));
}
uint8 cc2520_reserved_reject( TiCc2520Adapter * cc )
{
    return CC2520_BCLR( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,7));
}

//define whether the CMD frames are accepted or rejected
uint8 cc2520_cmd_accept( TiCc2520Adapter * cc )
{
    return CC2520_BSET( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,6));
}
uint8 cc2520_cmd_reject( TiCc2520Adapter * cc )
{
    return CC2520_BCLR( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,6));
}

//define whether the ack frames are accepted or rejected
uint8 cc2520_ack_accept( TiCc2520Adapter * cc )
{
    return CC2520_BSET( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,5));
}
uint8 cc2520_ack_reject( TiCc2520Adapter * cc ) 
{
    return CC2520_BCLR( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,5));
}

//define whether the data frames are accepted or rejected 
uint8 cc2520_data_accept( TiCc2520Adapter * cc )
{
    return CC2520_BSET( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,4));
}
uint8 cc2520_data_reject( TiCc2520Adapter * cc ) 
{
    return CC2520_BCLR( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,4));
}

//define whether the beacon frames are accepted or rejected 
uint8 cc2520_beacon_accept( TiCc2520Adapter * cc )
{
    return CC2520_BSET( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,3));
}
uint8 cc2520_beacon_reject( TiCc2520Adapter * cc )
{
    return CC2520_BCLR( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT1,3));
}

uint8 cc2520_disable_filter( TiCc2520Adapter * cc ) 
{
      return CC2520_BCLR( CC2520_MAKE_BIT_ADDR( CC2520_FRMFILT0,0));
}

uint8 cc2520_setchannel( TiCc2520Adapter * cc, uint8 chn )
{
    CC2520_REGWR8(CC2520_FREQCTRL, 0x0B + ((chn - 11) * 5));
    return 0;
}

uint8 cc2520_setshortaddress( TiCc2520Adapter * cc, uint16 addr )
{
    CC2520_MEMWR16(CC2520_RAM_SHORTADDR, addr);
    return 0;
}

uint8 cc2520_getshortaddress( TiCc2520Adapter * cc, uint16 * addr )
{
    *addr = CC2520_MEMRD16( CC2520_RAM_SHORTADDR);
    return 0;
}

uint8 cc2520_setpanid( TiCc2520Adapter * cc, uint16 id )
{
     CC2520_MEMWR16(CC2520_RAM_PANID, id);
     return 0;
}

uint8 cc2520_getpanid( TiCc2520Adapter * cc, uint16 * id )
{
    *id = CC2520_MEMRD16( CC2520_RAM_PANID);
    return 0;
}

/* power           current
*  0xF7            33.6mA
*  0xF2            31.3mA
*  0xAB            28.7mA
*  0x13            27.9mA
*  0x32            25.8mA
*  0x81            24.9mA
*  0x88            23.1mA
*  0x2C            19.9mA
*  0x03            16.2mA
*/
uint8 cc2520_settxpower( TiCc2520Adapter * cc, uint8 power )
{
    CC2520_REGWR8(CC2520_TXPOWER, power);
    return 0;
}


/***********************************************************************************
  Filename:     hal_rf.c

  Description:  hal radio interface

***********************************************************************************/



#define SUCCESS 0
#define FAILED 1
#define HAL_INT_ON() hal_enable_interrupts()
#define HAL_INT_OFF() hal_disable_interrupts()


/***********************************************************************************
* CONSTANTS AND DEFINES
*/
// Security
#define ADDR_RX                   0x200
#define ADDR_TX                   ADDR_RX + PKT_LEN_MAX
#define ADDR_NONCE_RX             0x310                 // Nonce for incoming packets
#define ADDR_NONCE_TX             0x320                 // Nonce for outgoing packets
#define ADDR_K                    0x340                 // Key address
#define TXBUF_START               0x100
#define RXBUF_START               0x180
#define AUTHSTAT_H_BM             0x08                  // AUTHSTAT_H bit of DPUSTAT
// register
#define TX_FRM_DONE_BM            0x02                  // TX_FRM_DONE bit of EXCFLAG0
// register

#define HIGH_PRIORITY             1
#define LOW_PRIORITY              0
#define PKT_LEN_MAX               127

// CC2520 RSSI Offset
#define RSSI_OFFSET               76

/***********************************************************************************
* LOCAL FUNCTIONS
*/

static void halRfSecurityInitRAM(uint8* key, uint8* nonceRx, uint8* nonceTx);
static void halRfWriteReg8(uint8 addr, uint8 value);
static HAL_RF_STATUS halRfWriteMem8(uint16 addr, uint8 value);
static HAL_RF_STATUS halRfWriteMem16(uint16 addr, uint16 value);
static HAL_RF_STATUS halRfStrobe(uint8 cmd);


/***********************************************************************************
* @fn      halRfSecurityInitRAM
*
* @brief   Write key and nonce to Radio RAM
*
* @param   none
*
* @return  uint8 - SUCCESS or FAILED
*/
static void halRfSecurityInitRAM(uint8* key, uint8* nonceRx, uint8* nonceTx)
{
    // Write key
    CC2520_MEMWR(ADDR_K,KEY_LENGTH,key);

    // Write nonce RX
    CC2520_MEMWR(ADDR_NONCE_RX,NONCE_LENGTH,nonceRx);

    // Write nonce TX
    CC2520_MEMWR(ADDR_NONCE_TX,NONCE_LENGTH,nonceTx);

    // Reverse key
    CC2520_MEMCPR(HIGH_PRIORITY,KEY_LENGTH,ADDR_K,ADDR_K);

    // Reverse nonces
    CC2520_MEMCPR(HIGH_PRIORITY,NONCE_LENGTH,ADDR_NONCE_RX,ADDR_NONCE_RX);
    CC2520_MEMCPR(HIGH_PRIORITY,NONCE_LENGTH,ADDR_NONCE_TX,ADDR_NONCE_TX);
}


/***********************************************************************************
* @fn      halRfWriteReg8
*
* @brief   Write byte to register
*
* @param   uint8 addr - address
*          uint8 value
*
* @return  none
*/
static void halRfWriteReg8(uint8 addr, uint8 value)
{
    CC2520_REGWR8(addr, value);
}


/***********************************************************************************
* @fn      halRfWriteMem8
*
* @brief   Write byte to memory
*
* @param   uint16 addr - address
*          uint8 value
*
* @return  HAL_RF_STATUS - chip status byte
*/
static HAL_RF_STATUS halRfWriteMem8(uint16 addr, uint8 value)
{
    return CC2520_MEMWR8(addr, value);
}


/***********************************************************************************
* @fn      halRfWriteMem16
*
* @brief   Write two bytes to memory
*
* @param   uint16 addr - address
*          uint16 value
*
* @return  HAL_RF_STATUS - chip status byte
*/
static HAL_RF_STATUS halRfWriteMem16(uint16 addr, uint16 value)
{
    return CC2520_MEMWR16(addr, value);
}


/***********************************************************************************
* @fn      halRfStrobe
*
* @brief   Send strobe command
*
* @param   uint8 cmd - command
*
* @return  HAL_RF_STATUS - chip status byte
*/
static HAL_RF_STATUS halRfStrobe(uint8 cmd)
{
    return(CC2520_INS_STROBE(cmd));
}


/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn      halRfPowerUp
*
* @brief   Power up
*
* @param   none
*
* @return  none
*/
void halRfPowerUp(void)
{
    // Power up CC2520
    CC2520_VREG_EN_OPIN(1);
    hal_delayus(CC2520_VREG_MAX_STARTUP_TIME);
    CC2520_RESET_OPIN(1);
    hal_delayus(CC2520_XOSC_MAX_STARTUP_TIME);
}


/***********************************************************************************
* @fn      halRfResetChip
*
* @brief   Reset chip
*
* @param   none
*
* @return  none
*/
void halRfResetChip(void)
{
    // Controlled reset of CC2520
    // Toggle RESETn
    CC2520_VREG_EN_OPIN(0);
    CC2520_RESET_OPIN(0);
    CC2520_VREG_EN_OPIN(1);

    hal_delayus(CC2520_VREG_MAX_STARTUP_TIME);  // us
    CC2520_RESET_OPIN(1);
    halRfWaitXoscStable();
}


/***********************************************************************************
* @fn      halRfInit
*
* @brief   Power up, sets default tuning settings, enables autoack and configures
*          chip IO
*
* @param   none
*
* @return  SUCCESS if the radio has started, FAILURE otherwise
*/
HAL_RF_STATUS halRfInit(void)
{
	// todo
    // Avoid GPIO0 interrupts during reset
    // halDigioIntDisable(&pinRadio_GPIO0);
    /*todo for testing 临时注释掉
    // Make sure to pull the CC2520 RESETn and VREG_EN pins low
    CC2520_RESET_OPIN(0);
    CC2520_SPI_END();
    CC2520_VREG_EN_OPIN(0);
    hal_delayus(1100);

    // Enable the voltage regulator and wait for it (CC2520 power-up)
    CC2520_VREG_EN_OPIN(1);
    hal_delayus(CC2520_VREG_MAX_STARTUP_TIME);

    // Release reset
    CC2520_RESET_OPIN(1);

    // Wait for XOSC stable to be announced on the MISO pin
    if (halRfWaitXoscStable()==FAILED)
        return FAILED;
     临时注释掉*/
    // Write tuning settings
    halRfWriteReg8(CC2520_TXPOWER, 0x32);  // Max TX output power
    halRfWriteReg8(CC2520_CCACTRL0, 0xF8); // CCA treshold -80dBm

    // Recommended RX settings
    halRfWriteMem8(CC2520_MDMCTRL0, 0x85);//推荐85
    halRfWriteMem8(CC2520_MDMCTRL1, 0x14);
    halRfWriteMem8(CC2520_RXCTRL, 0x3F);
    halRfWriteMem8(CC2520_FSCTRL, 0x5A);
	hal_delayms(1);
    halRfWriteMem8(CC2520_FSCAL1, 0x2B);//这一句输出结果不对
	hal_delayms(1);
    halRfWriteMem8(CC2520_AGCCTRL1, 0x11);
    halRfWriteMem8(CC2520_ADCTEST0, 0x10);
    halRfWriteMem8(CC2520_ADCTEST1, 0x0E);
    halRfWriteMem8(CC2520_ADCTEST2, 0x03);

	halRfWriteReg8(CC2520_FRMCTRL0, 0x60);
	halRfWriteReg8(CC2520_FRMCTRL1, 0x01);
	halRfWriteMem8(CC2520_EXTCLOCK, 0x00);
	halRfWriteReg8(CC2520_GPIOCTRL0, 1 + CC2520_EXC_RX_FRM_DONE);//这一句可能有问题，应该不仅仅是写寄存器那么简单
	halRfWriteReg8(CC2520_GPIOCTRL1, CC2520_GPIO_SAMPLED_CCA);
	halRfWriteReg8(CC2520_GPIOCTRL2, CC2520_GPIO_RSSI_VALID);
	halRfWriteReg8(CC2520_GPIOCTRL3, CC2520_GPIO_SFD);
	halRfWriteReg8(CC2520_GPIOCTRL4, CC2520_GPIO_SNIFFER_DATA);
	halRfWriteReg8(CC2520_GPIOCTRL5,  CC2520_GPIO_SNIFFER_CLK);


    // CC2520 setup
    CC2520_BSET(CC2520_MAKE_BIT_ADDR(CC2520_FRMCTRL0, 5));  // Enable AUTOACK
    /*todo for testing临时注释掉
    // CC2520 GPIO setup
    CC2520_CFG_GPIO_OUT(0, 1 + CC2520_EXC_RX_FRM_DONE);
    CC2520_CFG_GPIO_OUT(1,     CC2520_GPIO_SAMPLED_CCA);
    CC2520_CFG_GPIO_OUT(2,     CC2520_GPIO_RSSI_VALID);
    CC2520_CFG_GPIO_OUT(3,     CC2520_GPIO_SFD);
    CC2520_CFG_GPIO_OUT(4,     CC2520_GPIO_SNIFFER_DATA);
    CC2520_CFG_GPIO_OUT(5,     CC2520_GPIO_SNIFFER_CLK);
    临时注释掉*/
    return SUCCESS;
}


/***********************************************************************************
* @fn      halRfWaitXoscStable
*
* @brief   Wait for the crystal oscillator to stabilise.
*
* @param   none
*
* @return  SUCCESS if oscillator starts, FAILED otherwise
*/
HAL_RF_STATUS halRfWaitXoscStable(void)
{
    uint8 i;

    // Wait for XOSC stable to be announced on the MISO pin
    i= 100;
    CC2520_CSN_OPIN(0);
    while (i>1 && !CC2520_MISO_IPIN) {
        hal_delayus(10);
        --i;
    }
    CC2520_CSN_OPIN(1);

    return (i>1) ? SUCCESS : FAILED;
}


/***********************************************************************************
* @fn      halRfGetChipId
*
* @brief   Get chip id
*
* @param   none
*
* @return  uint8 - result
*/
uint8 cc2520_chipid(void)
{
    return(CC2520_MEMRD8(CC2520_CHIPID));
}


/***********************************************************************************
* @fn      halRfGetChipVer
*
* @brief   Get chip version
*
* @param   none
*
* @return  uint8 - result
*/
uint8 cc2520_chipver(void)
{
    return(CC2520_MEMRD8(CC2520_VERSION));
}


/***********************************************************************************
* @fn      halRfGetStatusByte
*
* @brief   Get chip status byte
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfGetStatusByte(void)
{
    return halRfStrobe(CC2520_INS_SNOP);
}


/***********************************************************************************
* @fn      halRfGetRandomByte
*
* @brief   Return random byte
*
* @param   none
*
* @return  uint8 - random byte
*/
uint8 halRfGetRandomByte(void)
{
    return CC2520_RANDOM8();
}


/***********************************************************************************
* @fn      halRfGetRssiOffset
*
* @brief   Return RSSI Offset
*
* @param   none
*
* @return  uint8 - RSSI offset
*/
uint8 halRfGetRssiOffset(void)
{
    return RSSI_OFFSET;
}


/***********************************************************************************
* @fn      halRfSetChannel
*
* @brief   Set RF channel. Channel must be in the range 11-26
*
* @param   none
*
* @return  none
*/
void halRfSetChannel(uint8 channel) {
    halRfWriteReg8(CC2520_FREQCTRL, 0x0B + ((channel - 11) * 5));
}


/***********************************************************************************
* @fn      halRfSetShortAddr
*
* @brief   Write short address to chip
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfSetShortAddr(uint16 shortAddr) {
    return halRfWriteMem16(CC2520_RAM_SHORTADDR, shortAddr);
}


/***********************************************************************************
* @fn      halRfSetPanId
*
* @brief   Write PAN Id to chip
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfSetPanId(uint16 panId) {
    return halRfWriteMem16(CC2520_RAM_PANID, panId);
}


/***********************************************************************************
* @fn      halRfSetPower
*
* @brief   Set TX output power
*
* @param   uint8 power - power level: TXPOWER_MIN_4_DBM, TXPOWER_0_DBM or
*                        TXPOWER_4_DBM
*
* @return  uint8 - SUCCESS or FAILED
*/
uint8 halRfSetPower(uint8 power)
{
    uint8 n;

    switch(power)
    {
    case TXPOWER_MIN_4_DBM: n = CC2520_TXPOWER_MIN_4_DBM; break;
    case TXPOWER_0_DBM: n = CC2520_TXPOWER_0_DBM; break;
    case TXPOWER_4_DBM: n = CC2520_TXPOWER_4_DBM; break;

    default:
        return FAILED;
    }
    CC2520_REGWR8(CC2520_TXPOWER, n);
    return SUCCESS;
}


/***********************************************************************************
* @fn      halRfRecvFrame
*
* @brief   Read RX buffer
*
* @param   uint8* data - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfRecvFrame(uint8* data, uint8 length)
{
    return CC2520_RXBUF(length, data);
}


/***********************************************************************************
* @fn      halRfWriteTxBuf
*
* @brief   Write to TX buffer
*
* @param   uint8* data - buffer to write
*          uint8 length - number of bytes
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfWriteTxBuf(uint8* data, uint8 length)
{
    // Copy packet to TX FIFO
    return CC2520_TXBUF(length,data);
}


/***********************************************************************************
* @fn      halRfReadRxBuf
*
* @brief   Read RX buffer
*
* @param   uint8* data - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfReadRxBuf(uint8* data, uint8 length)
{
    return CC2520_RXBUF(length, data);
}


/***********************************************************************************
* @fn      halRfTransmitCCA
*
* @brief   Transmit frame with Clear Channel Assessment.
*
* @param   none
*
* @return  uint8 - SUCCESS or FAILED
*/
uint8 halRfTransmitCCA(void)
{
    uint16 timeout = 2500; // 2500 x 20us = 50ms
    uint8 status=0;

    // Wait for RSSI to become valid
    while(!CC2520_RSSI_VALID_PIN);

    // Reuse GPIO2 for TX_FRM_DONE exception
    //HAL_INT_OFF();
	hal_disable_interrupts();
    CC2520_CFG_GPIO_OUT(2, 1 + CC2520_EXC_TX_FRM_DONE);
	//HAL_INT_ON();
	hal_enable_interrupts();

    // Wait for the transmission to begin before exiting (makes sure that this function cannot be called
    // a second time, and thereby cancelling the first transmission.
    while(--timeout > 0) {
        HAL_INT_OFF();
        halRfStrobe(CC2520_INS_STXONCCA);
        HAL_INT_ON();
        if (CC2520_SAMPLED_CCA_PIN) break;
        hal_delayus(20);
    }
    if (timeout == 0) {
        status = FAILED;
        halRfStrobe(CC2520_INS_SFLUSHTX);
    }
    else {
        status = SUCCESS;
        // Wait for TX_FRM_DONE exception
        while(!CC2520_TX_FRM_DONE_PIN);		  
        HAL_INT_OFF();
        CC2520_CLEAR_EXC(CC2520_EXC_TX_FRM_DONE);
        HAL_INT_ON();
    }

    // Reconfigure GPIO2
    HAL_INT_OFF();
    CC2520_CFG_GPIO_OUT(2, CC2520_GPIO_RSSI_VALID);
    HAL_INT_ON();
    return status;
}


/***********************************************************************************
* @fn      halRfSecurityInit
*
* @brief   Security init. Write nonces and key to chip.
*
* @param   none
*
* @return  none
*/
void halRfSecurityInit(uint8* key, uint8* nonceRx, uint8* nonceTx)
{
    halRfSecurityInitRAM(key, nonceRx, nonceTx);
}


/***********************************************************************************
* @fn      halRfRecvFrameUCCM
*
* @brief   Decrypts and reverse authenticates with CCM then reads out received
*          frame
*
* @param   uint8* data - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*          uint8 encrLength - number of bytes to decrypt
*          uint8 authLength - number of bytes to reverse authenticate
*          uuint8 m - ets length of integrity code (m=1,2,3 gives lenght of integrity
*                   field 4,8,16)
*
* @return  uint8 - SUCCESS or FAILED
*/
HAL_RF_STATUS halRfRecvFrameUCCM(uint8* data, uint8 length, uint8 encrLength, uint8 authLength, uint8 m)
{
    uint8 dpuStat;

    CC2520_RXBUFMOV(HIGH_PRIORITY, ADDR_RX, length, NULL);
    WAIT_DPU_DONE_H();

    // Find Framecounter value in received packet starting from 10th byte and update nonce
    // Copy in to nonce bytes (3-6) frame counter bytes
    // Incoming frame uses nonce Rx
    CC2520_MEMCP(HIGH_PRIORITY, 4, ADDR_RX+10, ADDR_NONCE_RX+3);
    WAIT_DPU_DONE_H();

    // Copy in short address to nonce bytes (7-8)
    CC2520_MEMCP(HIGH_PRIORITY, 2, ADDR_RX+7, ADDR_NONCE_RX+7);
    WAIT_DPU_DONE_H();

    CC2520_UCCM(HIGH_PRIORITY,ADDR_K/16, encrLength, ADDR_NONCE_RX/16, ADDR_RX, ADDR_RX+authLength, authLength, m);
    WAIT_DPU_DONE_H();

    // Check authentication status
    dpuStat = CC2520_REGRD8(CC2520_DPUSTAT);

    // Read from RX work buffer into data buffer
    CC2520_MEMRD(ADDR_RX, length, data);

    if( (dpuStat & AUTHSTAT_H_BM) != AUTHSTAT_H_BM ) {
        // Authentication failed
        return FAILED;
    }
    else return SUCCESS;
}


/***********************************************************************************
* @fn      halRfWriteTxBufCCM
*
* @brief   Encrypt and authenticate plaintext then fill TX buffer
*
* @param   uint8* data - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*          uint8 encrLength - number of bytes to decrypt
*          uint8 authLength - number of bytes to reverse authenticate
*          uuint8 m - length of integrity code (m=1,2,3 gives lenght of integrity
*                   field 4,8,16)
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfWriteTxBufCCM(uint8* data, uint8 length, uint8 encrLength, uint8 authLength, uint8 m)
{
    uint8 micLength;

    // Check range of m
    hal_assert(m<=4);

    if(m>0) {
        micLength = 0x2<<m;
    }
    else if(m==0) {
        micLength=0;
    }

    // Write packet to work buffer
    CC2520_MEMWR(ADDR_TX, length, data);

    // skip the length byte and start from the next byte in TXBUF
    // Outgoing frame uses nonce_tx
    CC2520_CCM(HIGH_PRIORITY,ADDR_K/16, encrLength, ADDR_NONCE_TX/16, ADDR_TX+1, 0, authLength, m);
    WAIT_DPU_DONE_H();

    // copy from work buffer to TX FIFO
    CC2520_TXBUFCP(HIGH_PRIORITY, ADDR_TX, length+micLength, NULL);
    WAIT_DPU_DONE_H();

    return halRfGetStatusByte();
}


/***********************************************************************************
* @fn      halRfIncNonceTx
*
* @brief   Increments frame counter field of stored nonce TX
*
* @param   none
*
* @return  none
*/
void halRfIncNonceTx(void)
{
    // Increment frame counter field of 16 byte nonce TX
    // Frame counter field is 4 bytes long

    // Increment framecounter bytes (3-6) of nonce TX
    CC2520_INC(HIGH_PRIORITY, 2, ADDR_NONCE_TX+3);
    WAIT_DPU_DONE_H();
}


/***********************************************************************************
* @fn      halRfReceiveOn
*
* @brief   Turn receiver on
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfReceiveOn(void)
{
    return halRfStrobe(CC2520_INS_SRXON);
}


/***********************************************************************************
* @fn      halRfReceiveOff
*
* @brief   Turn receiver off
*
* @param   none
*
* @return  HAL_RF_STATUS - chip status byte
*/
HAL_RF_STATUS halRfReceiveOff(void)
{
    return halRfStrobe(CC2520_INS_SRFOFF);
}

/**
 * _cc2520_fifop_handler
 * This function is the simple wrapper of the FIFOP interrupt handler. It will be 
 * called when the cc2520 raises an FIFOP interrupt request.
 */
void _cc2520_fifop_handler(void * object, TiEvent * e)
{
    TiCc2520Adapter * cc = (TiCc2520Adapter *)object;

    // todo  1ms is too long
	//hal_delayms(1);
    hal_enter_critical();
	cc->rxlen = _cc2520_read_rxbuf(cc, &cc->rxbuf[0], CC2520_RXBUF_SIZE);

    if (cc->listener != NULL)
    {
        cc->listener( cc->lisowner, NULL);
    }
    hal_leave_critical();
    // need clear the interrupt flag manually.
    EXTI_ClearITPendingBit(EXTI_Line0);    
}

