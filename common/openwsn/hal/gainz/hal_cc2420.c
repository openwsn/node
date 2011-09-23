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
 * hal_cc2420.c
 * This module implements a cc2420 adapter service. It's an simple wrapper of the
 * cc2420 low power wireless transceiver from Chipcon/TI. It helps to implement 
 * the MAC network layer. 
 *
 * You can regard the adapter as the PHY layer in layered communication architecture.
 * However. cc2420 provides more functions than a simple PHY.
 * 
 * Reference
 * - cc2420's startup and initialization, http://blog.sina.com.cn/s/blog_5e194f5e0100heex.html;
 * 
 * @state
 *	compile passed. tested. released.
 * 
 * @history
 * @author zhangwei on 200609
 *
 * @modified by zhangwei on 20090718
 *	- revision. 
 * @modified by yan-shixing(TongJi University) in 200907
 *	- tested
 * @modified by zhangwei on 20090808
 *	- bugfix: cc2420_setrxmode(cc) in cc2420_open()
 *  - add automatic mode transition in cc2420_recv()/cc2420_write()
 *  - add cc2420_sfd_handler(). not tested.
 * @modified by zhangwei(TongJi University) on 20090919
 *	- add cc2420_rssi() and cc2420_lqi(). not tested.
 * @modified by zhangwei, yanshixing 
 *  - improved cc2420_recv add crc校验
 * @modified by Xu-Fuzhen in TongJi University(xufz0726@126.com) in 2010.10
 *  - improved interface and cc2420_ischannelclear()
 *
 * @modified by openwsn on 20110405
 *  - add cc2420_broadcast()
 * 	- upgrade cc2420_send()
 * 	- upgrade cc2420_ischannelclear()
 *
 * @modified by openwsn on 20110410
 *  - Upgrade cc2420_send and revised the ACK frame processing. Now the cc2420
 *    module doesn't care for the ACK processing yet. 
 *  - Change clear channel assessment (CCA) request option from 0x20 to 0x02 in
 *    the send() function.
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
#include "../hal_cc2420const.h"
#include "../hal_cc2420.h"
#include "../hal_debugio.h"
#include "../hal_cc2420inc.h"

/* @attention
 * If you want to disable all the assertions in this macro, you should undef CONFIG_DEBUG.
 * You should do this in release version */

#undef  CONFIG_DEBUG
#define CONFIG_DEBUG


/* In "hal_cc2420inc.h", we implement the most fundamental cc2420 operating functions.
 * If you want to port hal_cc2420 to other platforms, you can simply revise the 
 * hal_cc2420inc.h. The other part inside hal_cc2420.h can keep unchanged.
 */
#include "../hal_cc2420inc.h"


// Current Parameter Arrray Positions
enum{
 CP_MAIN = 0,
 CP_MDMCTRL0,
 CP_MDMCTRL1,
 CP_RSSI,
 CP_SYNCWORD,
 CP_TXCTRL,
 CP_RXCTRL0,
 CP_RXCTRL1,
 CP_FSCTRL,
 CP_SECCTRL0,
 CP_SECCTRL1,
 CP_BATTMON,
 CP_IOCFG0,
 CP_IOCFG1
} ;

static inline int8 _cc2420_pin_init( TiCc2420Adapter * cc );
static inline int8 _cc2420_reg_init( TiCc2420Adapter * cc );
static inline bool _cc2420_setreg( TiCc2420Adapter * cc );
void _cc2420_waitfor_crystal_oscillator( TiCc2420Adapter * cc );
static uint8       _cc2420_writetxfifo( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 option );
static uint8       _cc2420_readrxfifo( TiCc2420Adapter * cc, char * buf, uint8 size, uint8 option );
static void        _cc2420_fifop_handler( void * ccptr, TiEvent * e );

#ifdef CONFIG_CC2420_SFD
static void        _cc2420_sfd_handler( void * ccptr, TiEvent * e );
#endif

/******************************************************************************
 * cc2420 service management 
 * construct(), destroy(), open() and close()
 *****************************************************************************/

TiCc2420Adapter * cc2420_construct( void * mem, uint16 size )
{
	hal_assert( sizeof(TiCc2420Adapter) <= size );
	memset( mem, 0x00, size );
	return (TiCc2420Adapter *)mem;
}

void cc2420_destroy( TiCc2420Adapter * cc )
{
	cc2420_close( cc );
}

TiCc2420Adapter * cc2420_open( TiCc2420Adapter * cc, uint8 id, TiFunEventHandler listener, 
	void * lisowner, uint8 option )
{
    cc->id = 0;
    cc->state = CC2420_STATE_RECVING;
    cc->listener = listener;
    cc->lisowner = lisowner;
    cc->option = option;
	cc->rssi = 0;
	cc->lqi = 0;
	cc->spistatus = 0;
	cc->rxlen = 0;

	_cc2420_pin_init( cc );
	_cc2420_spi_open();

	// enable the cc2420's internal voltage regulator through the VREG_EN pin of cc2420.
	// assert( the voltage regulation work correctly ). 
	cc2420_vrefon( cc );	

	// reset the chip by giving a pulse and level signal.
	HAL_CLR_CC_RSTN_PIN();
	// the old delay value is 10, seems also ok
	hal_delayus(100);  

	HAL_SET_CC_RSTN_PIN();
	// old value is 10, seems also ok
	hal_delayus(100);
	
	// start the oscillator of cc2420. 
	// assert( the oscillator must be started successfully );
	#ifdef CONFIG_DEBUG
	if (cc2420_oscon(cc))
	{
		led_on(LED_GREEN); hal_delay(100);
		led_off(LED_GREEN); hal_delay(1000);
	}
	else{

		led_on(LED_YELLOW); hal_delay(100);
		led_off(LED_YELLOW); hal_delay(1000);
	}
	#endif

	#ifndef CONFIG_DEBUG
	cc2420_oscon( cc );
	#endif

	// default settings attention:
	// - MDMCTRL0.AUTOCRC is always enabled for both the sender and receiver. 
	// - MDMCTRL0.AUTOACK is by default disabled. But you can call cc2420_enable_autoack()
	//	 to enable it.
	// - security is by default disabled.
	//
	_cc2420_reg_init( cc );
	
	// the default setting is to disable address recognition and set threhold of FIFOP
	#ifdef CONFIG_DEBUG
	if (_cc2420_setreg( cc ))
	{
		led_on(LED_GREEN); hal_delay(100);
		led_off(LED_GREEN); hal_delay(100);
	}
	else
	{
		led_on(LED_YELLOW); hal_delay(1000);
		led_off(LED_YELLOW); hal_delay(1000);
	} 
	#else
	_cc2420_setreg( cc );
	#endif

	cc2420_setchannel( cc, CC2420_DEF_CHANNEL );            // default communication channel
	cc2420_enable_addrdecode( cc );                         // enable hardware address recognition
	cc2420_setpanid( cc, CC2420_DEF_PANID );                // network id
	cc2420_setshortaddress( cc, CC2420_DEF_LOCAL_ADDRESS ); // node id inside the network
	cc2420_disable_autoack( cc );                           // auto ack is disabled by default
	
	cc2420_setrxmode( cc );  
	                               
	// @todo by zhangwei on 2011.04.05
	// whether we should waitfor oscillator start before the above register settings
	// or after them? please read the data sheet to verify the  current settings.
	_cc2420_waitfor_crystal_oscillator( cc );

	/* Here only enable the FIFOP interrupt request, but you must enable the global
	 * interrupt flag somewhere or else the cc2420 adapter still cannot response 
	 * to the FIFOP interrupt.
	 */
    hal_attachhandler( INTNUM_CC2420_FIFOP, _cc2420_fifop_handler, cc );
	
	/* @attention
	 * - Though you attach the SFD interrupt handler here, the interrupt itself 
	 *   hasn't been enabled yet! You should also enable the global interrupt flag 
	 *   in order to make the system really response to the SFD interrupt request.
	 * - SFD interrupt is controlled by the svc_timesync module.
	 * - The default settings for the SFD interrupt is disabled currently.
	 */
	#ifdef CONFIG_CC2420_SFD
    hal_attachhandler( INTNUM_CC2420_SFD, _cc2420_sfd_handler, cc );
	#endif

	/* Enable the FIFOP interrupt so that the microcontroller can know when a frame
	 * is received by the transceiver successfully. */
    cc2420_enable_fifop( cc );
	
	/* Enable the SFD interrupt so that the microcontroller can know when a frame
	 * arrived. This interrupt is often used to implement time synchronization 
	 * protocol */
	#ifdef CONFIG_CC2420_SFD
	//cc2420_enable_sfd(cc);
	#endif
    return cc;
} 

void cc2420_close( TiCc2420Adapter * cc )
{
	cc2420_disable_fifop( cc );
    hal_detachhandler( INTNUM_CC2420_FIFOP );
}

uint8 cc2420_state( TiCc2420Adapter * cc )
{
	return cc->state;
}


/* _cc2420_pin_init()
 * This function is used in this file only. It initialize MCU's PIN functions 
 * and directions. If you want to port this adapter to other MCU, you should modify 
 * this function.
 * 
 * Actually, the following pin initialization has nothing to do with cc2420 operations.
 */
inline int8 _cc2420_pin_init( TiCc2420Adapter * cc )
{
	HAL_MAKE_MISO_INPUT();
	HAL_MAKE_MOSI_OUTPUT();
	HAL_MAKE_SPI_SCK_OUTPUT();
	HAL_MAKE_CC_RSTN_OUTPUT();
	HAL_MAKE_CC_VREN_OUTPUT();
	HAL_MAKE_CC_CS_OUTPUT();
	HAL_MAKE_CC_FIFOP_INPUT();
	HAL_MAKE_CC_CCA_INPUT();
	HAL_MAKE_CC_SFD_INPUT();
	HAL_MAKE_CC_FIFO_INPUT();

	// @attention: suggest giving initial values to these pins and wait for a little time
	// to guarantee the configuration is correct. 
  
    return 0;
}


inline static int8 _cc2420_reg_init( TiCc2420Adapter * cc )
{
	cc->param[CP_MAIN] = 0xf800;

	// disable auto adr_decode, cca mode is 3, auto crc is set
	cc->param[CP_MDMCTRL0] = ((0 << CC2420_MDMCTRL0_PANCRD) | (0 << CC2420_MDMCTRL0_ADRDECODE) | 
		(2 << CC2420_MDMCTRL0_CCAHIST) | (3 << CC2420_MDMCTRL0_CCAMODE)  | 
		(1 << CC2420_MDMCTRL0_AUTOCRC) | (2 << CC2420_MDMCTRL0_PREAMBL)); 

	cc->param[CP_MDMCTRL1] = 20 << CC2420_MDMCTRL1_CORRTHRESH; // default
	cc->param[CP_RSSI] = 0xE080; //default 
	cc->param[CP_SYNCWORD] = 0xA70F;

	// CC2420_TXCTRL_BUFCUR:  default is 2-1.16ma,now is 980ua
	// default, turnaround time is 12 symbols
	cc->param[CP_TXCTRL] = ((1 << CC2420_TXCTRL_BUFCUR) | (1 << CC2420_TXCTRL_TURNARND) | 
		(3 << CC2420_TXCTRL_PACUR) | (1 << CC2420_TXCTRL_PADIFF) | (0x1f << CC2420_TXCTRL_PAPWR)); 

	// default is 980ua
	cc->param[CP_RXCTRL0] = ((1 << CC2420_RXCTRL0_BUFCUR) | (2 << CC2420_RXCTRL0_MLNAG) | 
		(3 << CC2420_RXCTRL0_LOLNAG) | (2 << CC2420_RXCTRL0_HICUR) | (1 << CC2420_RXCTRL0_MCUR) | 
		(1 << CC2420_RXCTRL0_LOCUR));

	cc->param[CP_RXCTRL1]  = ((1 << CC2420_RXCTRL1_LOLOGAIN) | (1 << CC2420_RXCTRL1_HIHGM) |
		(1 << CC2420_RXCTRL1_LNACAP) | (1 << CC2420_RXCTRL1_RMIXT) |
		(1 << CC2420_RXCTRL1_RMIXV) | (2 << CC2420_RXCTRL1_RMIXCUR));
							    
	// default to 2405 MHz
	cc->param[CP_FSCTRL] = ((1 << CC2420_FSCTRL_LOCK) | ((357+5*(CC2420_DEF_CHANNEL-11)) << CC2420_FSCTRL_FREQ));

	// disable the security
	cc->param[CP_SECCTRL0] = ((0 << CC2420_SECCTRL0_PROTECT) | (1 << CC2420_SECCTRL0_CBCHEAD) |
		(1 << CC2420_SECCTRL0_SAKEYSEL)  | (1 << CC2420_SECCTRL0_TXKEYSEL) | (1 << CC2420_SECCTRL0_SECM ));

	cc->param[CP_SECCTRL1] = 0;
	cc->param[CP_BATTMON]  = 0;		//battery monitor is disable 

	//cc->param[CP_IOCFG0]   = (((TOSH_DATA_LENGTH + 2) << cc2420_IOCFG0_FIFOTHR) | (1 <<cc2420_IOCFG0_FIFOPPOL)) ;
	//set fifop threshold to greater than size of frame, fifop goes active at end of frame
 
	// todo: modified 20090723
	// set high rise interrupt
	// FIFOP threshold is 127

	// 为什么要用127移位？ 不要用这种写法（颜：其实127就是设置的值，因为CC2420_IOCFG0_FIFOTHR为0，它大概是为了统一格式，所以这么写）
	//cc->param[CP_IOCFG0]   = (((127) << CC2420_IOCFG0_FIFOTHR) | (1 <<CC2420_IOCFG0_FIFOPPOL)) ; //polarity is inverted as compareed to the specification
	cc->param[CP_IOCFG0]   = (((127) << CC2420_IOCFG0_FIFOTHR) | (0 <<CC2420_IOCFG0_FIFOPPOL)) ; //polarity is inverted as compareed to the specification
	cc->param[CP_IOCFG1]   =  0;

	return 0;
}

/* cc2420 set register vlaues. used to set the register values of cc2420 */
inline bool _cc2420_setreg( TiCc2420Adapter * cc )
{
	uint16 data;
  
	/*cc2420_writeregister( cc, CC2420_MAIN, cc->param[CP_MAIN] );
	cc2420_writeregister( cc, CC2420_MDMCTRL0, cc->param[CP_MDMCTRL0] );
	data = cc2420_readregister( cc, CC2420_MDMCTRL0 );

	if (data != cc->param[CP_MDMCTRL0]) 
	{
		return FALSE;
    }
	
	cc2420_writeregister( cc, CC2420_MDMCTRL1, cc->param[CP_MDMCTRL1] );
	cc2420_writeregister( cc, CC2420_RSSI, cc->param[CP_RSSI] );
	cc2420_writeregister( cc, CC2420_SYNCWORD, cc->param[CP_SYNCWORD] );
	cc2420_writeregister( cc, CC2420_TXCTRL, cc->param[CP_TXCTRL] );
	cc2420_writeregister( cc, CC2420_RXCTRL0, cc->param[CP_RXCTRL0] );
	cc2420_writeregister( cc, CC2420_RXCTRL1, cc->param[CP_RXCTRL1] );
	cc2420_writeregister( cc, CC2420_FSCTRL, cc->param[CP_FSCTRL] );

	cc2420_writeregister( cc, CC2420_SECCTRL0, cc->param[CP_SECCTRL0] );
	cc2420_writeregister( cc, CC2420_SECCTRL1, cc->param[CP_SECCTRL1] );
	cc2420_writeregister( cc, CC2420_IOCFG0, cc->param[CP_IOCFG0] );
	cc2420_writeregister( cc, CC2420_IOCFG1, cc->param[CP_IOCFG1] );

	cc2420_sendcmd( cc, CC2420_SFLUSHTX );
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );
	*/
	// todo:
	// 上面的代码是老版本，下面的代码是新版本，请将新老版本merge到一起去
	// new version started from here
	// 20090724

	cc2420_writeregister( cc, CC2420_MAIN, cc->param[CP_MAIN] );
	data = cc2420_readregister( cc, CC2420_MAIN);
	if (data != cc->param[CP_MAIN]) 
	{
		return false;
    }
	
	// register CC2420_MDMCTRL0 controls address recognition, CCA polority, checksum(CRC)
	// and more. Refer to cc2420 datasheet for more detail. 

	cc2420_writeregister( cc, CC2420_MDMCTRL0, cc->param[CP_MDMCTRL0]);
	data = cc2420_readregister( cc, CC2420_MDMCTRL0);
	if (data != cc->param[CP_MDMCTRL0]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_MDMCTRL1, cc->param[CP_MDMCTRL1]);
	data = cc2420_readregister( cc, CC2420_MDMCTRL1);
	if (data != cc->param[CP_MDMCTRL1]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_RSSI, cc->param[CP_RSSI] );
	data = cc2420_readregister( cc, CC2420_RSSI);
	if (data != cc->param[CP_RSSI]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_SYNCWORD, cc->param[CP_SYNCWORD] );
	data = cc2420_readregister( cc, CC2420_SYNCWORD);
	if (data != cc->param[CP_SYNCWORD]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_TXCTRL, cc->param[CP_TXCTRL] );
	data = cc2420_readregister( cc, CC2420_TXCTRL);
	if (data != cc->param[CP_TXCTRL]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_RXCTRL0, cc->param[CP_RXCTRL0] );
	data = cc2420_readregister( cc, CC2420_RXCTRL0);
	if (data != cc->param[CP_RXCTRL0]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_RXCTRL1, cc->param[CP_RXCTRL1] );
	data = cc2420_readregister( cc, CC2420_RXCTRL1);
	if (data != cc->param[CP_RXCTRL1]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_FSCTRL, cc->param[CP_FSCTRL] );
	data = cc2420_readregister( cc, CC2420_FSCTRL);
	if (data != cc->param[CP_FSCTRL]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_SECCTRL0, cc->param[CP_SECCTRL0] );
	data = cc2420_readregister( cc, CC2420_SECCTRL0);
	if (data != cc->param[CP_SECCTRL0]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_SECCTRL1, cc->param[CP_SECCTRL1] );
	data = cc2420_readregister( cc, CC2420_SECCTRL1);
	if (data != cc->param[CP_SECCTRL1]) 
	{
		return false;
    }

	// Register IOCFG0 controlls the FIFOP threshold, CCA pin polority, etc. 
	// Refer to cc2420 datasheet for more detail.
	
	cc2420_writeregister( cc, CC2420_IOCFG0, cc->param[CP_IOCFG0]);
	data = cc2420_readregister( cc, CC2420_IOCFG0);
	if (data != cc->param[CP_IOCFG0]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_IOCFG1, cc->param[CP_IOCFG1]);
	data = cc2420_readregister( cc, CC2420_IOCFG1);
	if (data != cc->param[CP_IOCFG1]) 
	{
		return false;
    }
	
	cc2420_sendcmd( cc, CC2420_SFLUSHTX );
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );

	return true;
}

/*void cc2420_restart( TiCc2420Adapter * cc )
{
	uint8 status;

	status = cc2420_sendcmd( cc, CC2420_SXOSCON );
	while (!(status & 0x40))
	{
		//CC2420_StrobCmd(CC2420_SXOSCOFF);
		status = CC2420_StrobCmd( CC2420_SXOSCON );
	}

	status = cc2420_sendcmd( cc, CC2420_SRXON );
	CC_CLR_CS_PIN();
	_cc2420_spi_put( CC2420_SFLUSHRX );
	_cc2420_spi_put( CC2420_SFLUSHRX );
	CC_SET_CS_PIN();
}*/


/******************************************************************************
 * cc2420 service input/output 
 * _cc2420_writetxfifo(), _cc2420_readrxfifo(), cc2420_recv(), cc2420_send()
 *****************************************************************************/

/* Write a frame into cc2420's TXFIFO buffer for sending. Attention the microcontroller
 * still need to send an CC2420_STXON command to the cc2420 transceiver to start 
 * the real transmitting. 
 * 
 * @param
 *	cc				          TiCc2420Adapter service
 *  buf                       the frame buffer inside the memory
 *  len                       length of the frame data inside "buf". The first byte is the mac 
 *                            frame length. it will be overwritten by the value "len-1".
 *  option			          if option & 0x01 == 1, then this frame request ACK.
 *                            if option & 0x02 == 1, then the CCA before sending is enabled.
 * 
 * @return
 *  >0                        byte count actually written to TXFIFO.
 *  0                         when failed
 * 
 * attention: 
 *  - TXFIFO underflow is issued if too few bytes are written to the TXFIFO. transmission
 * is then automatically stopped. the underflow is indicated by the TX_UNDERFLOW status
 * bit. (bit 5)  
 * 
 *	- bit5(TX_UNDERFLOW) indicates whether an FIFO underflow has occured during 
 * transmission. attention this must be cleared manually with a SFLUSHTX command strobe.
 * 
 * @todo QQQ: the above comments seems wrong! 
 */ 
uint8 _cc2420_writetxfifo( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 option )
{
	uint8 i=0;

	// Invalid frames will be ignored directly by the cc2420 driver. These frames
	// includes those which are too long or too short.
	//
	// @attention
	// The minimal frame is the acknowledge frame which has only 5 byte in MAC layer. 
	// Namely, 6 bytes in the PHY layer. The format is as the following: 
	//
	// 	[Length 1B] [Frame Control 2B] [Sequence 1B] [Checksum 2B]
	//
	// We don't support sending frames longer than 128 bytes(including 128 bytes) or 
	// less than 6 bytes. 
	
	// hal_assert( len == buf[0] );
	if ((len < CC2420_MIN_FRAME_LENGTH) || (len >= CC2420_TXFIFO_SIZE))
		return 0;

	// Cc2420's SFD pin goes high when the SFD field has been completely transmitted. 
	// It goes low again when the complete MPDU (as defined  by  the length field) 
	// has been transmitted or if an underflow is detected.
	// 
	// Refer to "Transmit mode" in cc2420 datasheet (page 33) for more.
	//
	// So we can judge whether the sending is active by reading the SFD pin. High 
	// SFD indicates the last frame is still in sending. It's safe to wait for the 
	// SFD change to low. Attention the plority of SFD pin can be configured in IOCFG1 
	// register. 
	// 
	// Another method to judge whether the cc2420 is in sending state is to use 
	// the status byte returned from SPI. An SNOP command can be issued to cc2420
	// to retrieve the status byte back.
	//
	// Refer to "Status Byte" in cc2420 datasheet for more detail.
	
	// @todo: Jiang Ridong, you can uncomment the following now. But the following 
	// deadloop as the following isn't a better idea. 
	// while (HAL_READ_CC_SFD_PIN() == 1) {};
	
	// for (i=0; i<0xFE; i++)
	//	for (j=0; j<0xFE; j++)
	//		if (HAL_READ_CC_SFD_PIN() == 0)
	//			break;
	//wait for the former frame to be sent completely. 
	while (HAL_READ_CC_SFD_PIN() == 1) {};

	cpu_atomic_t cpu_status = _cpu_atomic_begin();
	{
        cc2420_sendcmd( cc, CC2420_SFLUSHTX );
		HAL_CLR_CC_CS_PIN();
	    
		// attention
		// - the crystal oscillator must be running for writing the TXFIFO(0x3E) 
		// - reading TXFIFO is only possible using RAM read (according to cc2420 datasheet)

		cc->spistatus = _cc2420_spi_put( CC2420_TXFIFO );

		_cc2420_spi_put( len-1 );

		for (i=1; i<len; i++) 
		{
			_cc2420_spi_put( buf[i] );
		}
		
		HAL_SET_CC_CS_PIN();
	}
	_cpu_atomic_end(cpu_status);
	return len;
}

/**
 * Reads the first frame inside cc2420 transceiver's RXFIFO and place the frame data 
 * into "buf".
 * 
 * This function can be called safely in both interrupt mode or non-interrupt mode. 
 *
 * @param
 *	cc			TiCc2420Adapter service
 *  buf         the memory buffer to hold the frame
 *  size        size of the buffer. for cc2420, it should always larger than the 
 *              maximum frame length + 1. Currently, the default settins is 128
 *              (defined by macro CONFIG_CC2420_RXBUF_SIZE)
 *  option      No use now. Default to 0x00.
 *
 * @return
 *	0			no frame received inside the buffer
 *	>0          frame received. the return value indicates how many bytes in the buffer. 
 *              the first byte in the buf is the MAC frame length read out from 
 *              cc2420 transceiver. so the returned value is always equal to buf[0]+1.
 */
uint8 _cc2420_readrxfifo( TiCc2420Adapter * cc, char * buf, uint8 size, uint8 option )
{
	//cpu_atomic_t cpu_status;
	uint8 i, len, count=0;
	uint8 status;

	// attention: the crystal oscillator must be running for access the RXFIFO(0x3F) 
	// Wait for the oscillator stable. If it's not stable, then you may had to restart
	// the transceiver chip. (not implemented now)
	
	_cc2420_waitfor_crystal_oscillator( cc );

	do{	 
		status = cc2420_sendcmd( cc, CC2420_SNOP );
	}while (!(status & (1 << CC2420_XOSC16M_STABLE)));

	/* @todo since the readrxfifo is called in both non-interrupt mode and interrupt
	 * node, I suggest to develope two version of readrxfifo to satisfy different
	 * contexts.
	 *
	 * modified by zhangwei on 200912
	 *	- Bug fix. Since the execution of this function maybe intervened by the interrupts, 
	 * we should place _cpu_atomic_begin() at the very begging of the implementation.
	 */
	 
	//cpu_status = _cpu_atomic_begin();
	{
		HAL_CLR_CC_CS_PIN();
		
		// SPDR bit7 (SPIF): SPI interrupt flag. this bit will be set when a serial 
		// transfer complete. it's automatically clearly by ISR or by first reading 
		// the SPI status register with SPIF set, then accessing the SPI data register.
		// reference: SPSR register in atmega128L datasheet. 

		// Read RXFIFO register	

	    cc->spistatus = _cc2420_spi_put( CC2420_RXFIFO | 0x40 );	

		/* Read the length byte from cc2420 transceiver. Attention the value of the 
		 * length byte doesn't include itself. It assumes the first byte in the 
		 * RXFIFO is the frame length. */
		len = _cc2420_spi_get();						
	                             
		/* modified by zhangwei on 200909
		 *	- Bug fix. You cannot enable the following assertion in real applications. 
		 * If two frames encounter collision during their transmission, the len byte 
		 * read out may longer than size. This is the feature of wireless communication
		 * and not our own design fault. 
		 * 
		 * hal_assert( len < size );
		 */
		
		/* @attention
		 * Though cc2420 datasheet tells us this transceiver supports frames longer
		 * than 128 bytes(including the length byte itself), the TiCc2420Adapter doesn't
		 * plan to support this feature. 
		 * 
		 * The frames which are very short and very long(larger than 128) are all
		 * discarded by TiCc2420Adapter. 
		 */
		if (len+1 < CC2420_MIN_FRAME_LENGTH)
		{
			
			cc2420_sendcmd( cc, CC2420_SFLUSHRX );
			// hal_delayus(5);
			cc2420_sendcmd( cc, CC2420_SFLUSHRX );
			// hal_delayus(5);
			while (HAL_READ_CC_SFD_PIN()) {};
			count=0;
		}
		else if (len >= CC2420_MAX_FRAME_LENGTH)
		{
			// if the incomming frame length is too long (RXFIFO buffer overflow occured), 
			// then this function will simply drop it.
			//
			// @attention
			// Actually, the cc2420 transceiver support frames longer than 127. 
			// It's not an standard 802.15.4 feature. You can deal with this case 
			// if you need long frame support. 
			//

			
			cc2420_sendcmd( cc, CC2420_SFLUSHRX );
			cc2420_sendcmd( cc, CC2420_SFLUSHRX );
			while (HAL_READ_CC_SFD_PIN()) {};
			count=0;

		}
		else{
			// @attention
			// If the frame length exceed the available size in the RXFIFO, then 
			// the cc2420 will be transfered to RX_OVERFLOW state. You must send 
			// CC2420_SFLUSHRX command(flush RXFIFO) to cc2420 manually to transfer 
			// the cc2420 state to RX_SFD_SEARCH again, or else the cc2420 will 
			// stop here.
			//
			// @modified by zhangwei in 2009
			// In the past, we modify the len directly if it exceed the maximum 
			// length of an valid 802.15.4 frame. But this isn't a good solution
			// because the wrong length field in the frame inside cc2420 chip is
			// still unchanged. So we give up this solution.
			// 
			// len &= 0x7F;
			//
			// @modified by zhangwei in 2011
			// The following assertioin may report failure some time. In normal 
			// cases, it should be passed. But if the frame is desctroyed during 
			// its transmission, we cannot guanrante the value of the length byte. 
			// That's why the following assertion may failed. So we decided to 
			// comment the assert. 
			//
			// hal_assert( len+1 <= size );

			if (len < size)
			{
				buf[0] = len;
				for (i=1; i<=len; i++)
				{
					buf[i] = _cc2420_spi_get();
				}
				count = len+1;

				// @todo You should first verify whether the RSSI is enabled
				// Set the RSSI and LQI indicator of the current frame
				cc->rssi = cc->rxbuf[len-1];
				cc->lqi = cc->rxbuf[len];

				// If the crc checksum failed, then simply drop this frame.
				if(((cc->lqi >> 7) == 0x00))
				{
					count = 0;
				}
                

			}
			else{

				/*
				cc2420_sendcmd( cc, CC2420_SFLUSHRX );
				cc2420_sendcmd( cc, CC2420_SFLUSHRX );
				while (HAL_READ_CC_SFD_PIN()) 
				{
				}*/
				count = 0;
			}

			/* According to cc2420's datasheet, the last two bytes in the frame are 
			 * RSSI(Received Signal Strength Indicator) and LQI(link Quality Indicator).
			 * The value of "len" already includes them. You needn't do anything special 
			 * to read them out. they've already been read out and put into the "buf".
			 */
		}

		//Maybe we should clear the frame in the rxfifo after reading,but i don't know if we may loss some important frames such as ack.
		/*
		cc2420_sendcmd( cc, CC2420_SFLUSHRX );
		cc2420_sendcmd( cc, CC2420_SFLUSHRX );
		while (HAL_READ_CC_SFD_PIN()) {};*/
		HAL_SET_CC_CS_PIN();
    }
    //_cpu_atomic_end(cpu_status); 
	
	return count;
}

/* cc2420_send()
 * send a frame at once through cc2420 transceiver.
 *
 * @param
 *	len                       is the length of the data in the "buf". it's equal to 
 *                            "frame length + 1".
 *  option                    if bit 0 of option is 1, then this function will request 
 *                            ACK from the receiver.
 *							  if bit 1 of option is 1, then this function will not
 *                            do CCA before sending. 0 is the default setting.
 * 
 *  option			          if option & 0x01 == 1, then this frame request ACK.
 *                            if option & 0x02 == 1, then the CCA before sending is enabled.
 *
 * @return
 *	0			              failed. you should retry again.
 *  >0                        byte count successfully send. It should equal to parameter 
 *                            "len" when success.
 */
uint8 cc2420_send( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 option )
{
	uint16 count, rxlen, loopcount;
	uint8 status;

	hal_assert( len > 0 );

	// @attention
	// The hardware transceiver will transfer to RX mode automatically after sending
	// the whole frame. However, there maybe a little delay for the transceiver to switch 
	// from TX to RX mode. You should guarantee the oscillator is stable now.
	
	cc->state = CC2420_STATE_SENDING;

	// If the "option" parameter indicates the frame needs an ACK response, then we 
	// should modify the frame control field of this frame and set the ACK REQUEST bit
	// in the frame control. 
	//
	// If b5 in the frame control byte is set to 1, then it indicates ACK request
	// according to cc2420 and 802.15.4 specification
	
	if (option & 0x01)
	{
		buf[1] |= (0x01 << 5);
	}
	else{
		buf[1] &= (~(0x01 << 5));
	}

	// @attention
	// We should check whether the last sending is really finished or not, in particular 
	// when the MCU runs very fast. However, I didn't find an satisfied method to 
	// check this for cc2420 transceiver. You can read the TX bit in the status byte
	// or judge the SFD pin. However, both of these two method may lead to wrong 
	// results.
	//
	// The following checking only correct if we insert 12 symbol time waiting 
	// after issuing the STXON command strobe to cc2420. Or else may lead to wrong 
	// judgement.
	//
	// If the RF Transmission is active, we can further judge SFD pin. Though this
	// may still miss some cases.
	status = cc2420_sendcmd( cc,CC2420_SNOP );
	if ((status & 0x04) == 1)
	{
		while (HAL_READ_CC_SFD_PIN() == 1) {};
	}

	// Assert the oscillator is running now. If it failed to run, you should restart
	// the transceiver chip.
	hal_assert( status & (1 << CC2420_XOSC16M_STABLE) );
	
	count = _cc2420_writetxfifo( cc, (char*)&(buf[0]), len, option );
	
	// Perform carrier channel assessment (CCA)
	if (option & 0x02)
	{
		// Before sending the STXON command to start the wireless transmitting, it is 
		// necessary to check the CCA pin to see whether the channel is clear. If it 
		// is clear, the CCA pin might be 1 (whether its 0 or 1 depends on cc2420's 
		// configuration.  
		// Referring to the graph in the checkpoint3.pdf, page 7 and the cc2420 datasheet, 
		// page 63, the CCA_MODE.
	
		while (1)
		{
			if (cc2420_ischannelclear(cc))
				break;
		}
	}
	
	// The preamble sequence is started 12 symbol periods after the command strobe. 
	// After the programmable start of frame delimiter has been transmitted, data 
	// is fetched from the TXFIFO. 
	//
	// A TXFIFO underflow is issued if too few bytes are written to  the  TXFIFO. 
	// Transmission is then  automatically stopped. The underflow is indicated in 
	// the TX_UNDERFLOW status bit, which is returned during each address byte and 
	// each byte written to the TXFIFO. The underflow bit is only cleared  by  
	// issuing a SFLUSHTX command strobe. 
	//
	// The TXFIFO can only contain one data frame at a given time. 
	//
	// After complete transmission of a data frame, the TXFIFO is automatically 
	// refilled with the last transmitted frame. Issuing a new  STXON or STXONCCA 
	// command strobe will then cause CC2420 to retransmit the last frame. 
	//
	// Writing to the TXFIFO after a frame has been transmitted will cause the TXFIFO 
	// to be automatically flushed before the new byte is written. The only exception  
	// is if a TXFIFO  underflow  has occurred, when a SFLUSHTX command strobe is required. 
	//
	//the following four sentences are to send the packet.
    //cc2420_sendcmd( cc,CC2420_SRFOFF);
	// Send the STXON command to cc2420 to start the transmitting now
	cc2420_sendcmd( cc, CC2420_STXON );
	
	// The SFD pin goes high when the SFD field has been completely transmitted. 
	// It  goes low again when the complete MPDU  (as defined  by  the length field) 
	// has been transmitted or if an underflow is detected. 
	
	// Wait for sending complete. The sending complete source code should 
	// be better moved to the front before the sending. Placing them here will
	// degrade the performance. 
	//
	// The cc2420 datasheet indicates there's at least 12 symbol time after issuing
	// the STXON command strobe. During this period, the cc2420 transceiver sends
	// the preamble and SFD character.
	
	while (!(HAL_READ_CC_SFD_PIN() == 1)) {};
	while (HAL_READ_CC_SFD_PIN() == 1) {};



	// hal_delay( at least 12 symbole time );

	// Check whether the TX process encounters underflow error
	status = cc2420_sendcmd(cc, CC2420_SNOP );
	if (status & 0x20) 
	{
		cc2420_sendcmd( cc, CC2420_SFLUSHTX );
        
		//it seems to need only one SFLUSHTX command.
		//cc2420_sendcmd( cc, CC2420_SFLUSHTX );
	}
	
	
	// @attention
	// If you change cc2420 to receiving mode immediately, you may encounter frame 
	// loss because the last sending may not complete in time.
	// Suggest to wait some time in the transition between sending and receiving. 
	
	//cc->state = CC2420_STATE_RECVING;
	
	// modified by zhangwei on 2011.04.10
	// The cc2420 adapter is an light weight encapsulation of the cc2420 hardware.
	// It should be as simple as possible. So I decided the ACK processing should 
	// moved out from this module into the MAC layer components, though it seems
	// pretty simpler and direct to implement ACK frame checking here. 
	

	// If the frame requires ACK response, then we should wait for the ACK frame. 
	// According to 802.15.4 specification and cc2420 design, the ACK frame will
	// arrive after 12 symbol time. 
	//
	// Theoretically speaking, the frame received next to the last sending frame 
	// may be not the ACK, but this should rarely occurs. We can assume the next
	// frame should be the ACK to the last frame to simplify the softeare. 

	//if ((count > 0) && (buf[1] & 0x20))
	if (0)
	{
		// @attention
		//	- After calling _cc2420_writetxfifo() and start transmistting, the cc2420
		// transceiver will sending the frame data now. But the micro controller
		// is running separately. So the software may runs faster than the cc2420
		// transceiver. You should coordinate these to avoid potential mistaks.
		//	- The cc2420 can automatically switch into RX mode after sending the 
		// frame. But you may need to wait for the RX state stable. 

		// -- Wait for sending complete --
		// while (HAL_READ_CC_SFD_PIN() == 1) {};
		
		// for (i=0; i<0xFE; i++)
		//	for (j=0; j<0xFE; j++)
		//		if (HAL_READ_CC_SFD_PIN() == 0)
		//			break;

		// -- Wwitch to RX mode and wait for RX mode stable --
		// seems not necessary for cc2420 transceiver. It seems can handle it well.
		//
		// cc2420_setrxmode( cc );
		// cc->state = CC2420_STATE_RECVING;
		// todo: _cc2420_waitfor_crystal_oscillator( cc );
		
		// if the oscillator isn't stable, then restart the transceiver chip
		/*
		status = cc2420_sendcmd( CC2420_SNOP );
		if (status < 0x40)
		{
			cc2420_restart( cc );
			return 0;
		}
		*/
		
		// Skip a short period to wait for the ACK frame. Then start checking the
		// ACK frame. It should arrive normally.
		//
		// @attention
		// According to cc2420 and 802.15.4 datasheet, the receiver should start to 
		// send ACK frame after Tack and 12 symbol time. The Sender should check ACK
		// frame after this time plus additional ACK frame receiving time. 
		//
		// @attention: There're two important parameters you can tunning. The first
		// is CC2420_MIN_ACK_DELAY. You can change it to control how long the receiver
		// start checking ACK after the last sending. The second is the maximum 
		// time for a ACK frame arrived and fully accepted by the transceiver. That's
		// the maximum time we should wait. Currently, there's no timer controlls 
		// the waiting time in cc2420 transceiver, so we try to receive for only 
		// some times. If we cannot got the ACK frame successfully, then we will
		// assume no ACK received. It's controlled by the loopcount variable.
		//

		/* todo for testing
		hal_delayus( CC2420_MIN_ACK_DELAY );
		
		rxlen = 0;
		for (loopcount = 0; loopcount < 5; loopcount++)
		{
			rxlen = _cc2420_readrxfifo( cc, &(cc->ackbuf[0]), CC2420_ACKBUFFER_SIZE, 0x00 );
			if (rxlen > 0)
			{
				// todo for Jiang Ridong
				// todo: You should judge the frame by the control bit in frame control 
				// field instead of length. This's only a lazy method. Should upgrade
				// 
				// furthermore, we should also judge whether the sequence in ACK 
				// is identical with the frame sent.
				//
				if (cc->ackbuf[0] == 5)
				{
					break;
				}
				rxlen = 0;
			}
			
			// insert a little delay between two adjacent RXFIFO read callings.
			hal_delayus( 2 );
		}
		
		// If rxlen > 0 now, then it means a correct ACK is successfully received. 
		// This sending is fully succeed. Or else set count to 0 to indicate failure.
		count = (rxlen > 0) ? count : 0;     
		*/

		// @todo 2011.05
		// the following warning is obsolete. should be deleted later.
		//
		// @warning
		// assert( global interrupts is enabled here )
		// You should guarantee the global interrupt is enabled at this point, or else
		// you're imporssible to got the ACK frame returned. In the last main version 
		// two years ago, hal_enable_interrupts() is called here. But in this version,
		// we use hal_atomic_begin() and hal_atomic_end(). And hal_enable_interrupts()
		// hal_disable_interrupts() are suggested not to be used anymore.

		// todo suggest cc2420_readrxfifo here

		// @todo
		// QQQ
		// ???
		// why cc->rxlen = 0 here? 
		// rxlen = 0 seems ok
		// this will cause unnecessary frame loss during transmission because the rxbuf may 
		// has some frame

		// If this frame needs ACK response, then we can check for ACK here.
		// If there's no ACK frame received, then we can assume the frame just sent
		// was lost. This transmission is failed.

		// to be deleted
		cc->rxlen = 0;

		rxlen = 0; 
		loopcount = 0;
		// ???: I'm not sure whether it's enough to loop 0xFF times to call cc2420_recv?
		// shall we try it more times?
		// todo: you should use timer instead of loopcount to control the checking period
		while ((rxlen == 0) && (loopcount < (0x3FF-1)))
		{	
			hal_delayus( 10 );
			rxlen = cc2420_recv( cc, (char*)&(cc->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0 );
			loopcount ++;
		}
		

		// Based on the above analysis and cc2420 transceiver's help, we can assume the 
		// frame just received is ACK frame. This can simplify the software. 
		//

		// to be deleted 
		// todo: 
		// ???
		// 如果收到ACK,还必须比较ACK帧中的sequence number和txbuf中的sequence number
		// 两个只有一致才能说明ack确认的是txbuf中的frame

		// if hasn't received the ACK frame, we had to return 0 to indicate the sending
		// is failed.
		if (rxlen == 0)
		{
			count=0;
		}
		else 
		{
			// if (buf[seqid_idx] != cc->rxbuf[]) 
			//	count = 0;
		}
		
	}
	return count;
}

/**
 * Broadcast a frame to all of the neighbor nodes. 
 * 
 * The difference between cc2420_send() and cc2420_broadcast() is that the cc2420_broadcast() 
 * doesn't support ACK frame by default, while the cc2420_send() can control whether 
 * requires ACK or not by parameter "option".
 *
 * @param
 *	len                       is the length of the data in the "buf". it's equal to 
 *                            "frame length + 1".
 *  option                    always 0x00. (always ignored by the broadcast function)
 *
 * @return
 *	0			              failed. you should retry again.
 *  >0                        byte count successfully send. It should equal to parameter 
 *                            "len" when success.
 */
uint8 cc2420_broadcast( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 option )
{
	/* clear the ACK REQUEST bit in the option parameter */
	option &= (~(0x01));
	return cc2420_send( cc, buf, len, option );
}

/* cc2420_recv()
 * Read a frame from cc2420 adapter's internal buffer or from cc2420 transceiver 
 * directly. Attention this function should only called outside of ISR in non-Interrupt
 * mode.
 *
 * This function will also parse the frame to get the RSSI and LQI value, and save 
 * them temporarily into cc->rssi and cc->lqi. Then you can call cc2420_rssi(...)
 * and cc2420_lqi(...) to retrieve them out. 
 * 
 * @return
 *	0			no frame received in the buffer
 *	>0          frame received in the buffer. The returned value is the data length. 
 *              The first byte in the "buf" is the frame length. The returned length
 *              should euqla to "frame length + 1".
 *
 * @todo
 * If the cc2420 is working in "manual ack" mode, then the cc2420_recv() must send a ACK
 * when the ACK REQUEST bit in the frame control field is set. However, currently, 
 * the cc2420 transceiver is set to AUTO ACK mode, so the software part is simplified.
 */
uint8 cc2420_recv( TiCc2420Adapter * cc, char * buf, uint8 size, uint8 option )
{
	cpu_atomic_t cpu_status;
	uint8 ret = 0;

	if (cc->state == CC2420_STATE_SENDING)
	{
		/* @todo
		 * You shouldn't check whether the last sending is complete or not. If the 
		 * program runs much faster than the cc2420 transmission speed, then it may lead 
		 * to unecessary frame loss at the sender. 
		 * In order to avoid such unecessary frame loss, you should read FIFO pin to 
		 * check the transceiver's state. 
		 * 
		 * Another idea is simply delay here. Though this will decreasing the performance.
		 * hal_delayus( 950 );
		 */

		//cc2420_setrxmode( cc );
		//_cc2420_waitfor_crystal_oscillator( cc );
		cc->state = CC2420_STATE_RECVING;
	}
	else if (cc->state != CC2420_STATE_RECVING)
	{
		return 0;
	}

	// @assert: the cc2420 adapter is in RECVING state now.

	// @attention
	// @warning
	// You should set the cc2420's FIFOP threshold to the largest possible size of 
	// the frame. Because this function will call _cc2420_readrxfifo() when both 
	// FIFO and FIFOP are high! This assume the two pin indicate the entire frame 
	// has already been received by cc2420
	cpu_status = _cpu_atomic_begin();
	{
		// If there's already a frame pending inside the rxbuf, then move this frame into
		// parameter buf. and try to pull the new incoming frames from the transceiver's 
		// hardware. 
		//
		if (cc->rxlen > 0)
		{	
			// The input buffer should be large enough to hold the frame or else 
			// the following assert will be failed. This shouldn't occure. But if
			// This really occurs, you should return 0 and clear the rxbuf at the 
			// same time to enable the program can continue running.
			//
			// @modified by zhangwei on 2011.04.11
			// @attention: Jiang Ridong and Zhang Wei eliminate the following assertion
			// because it may cause failure some time. We think this pheonomenon 
			// occurs when the ACK frame encounters confliction and the length byte
			// in the ACK frame changes to a larger value.
			//
			// hal_assert( cc->rxlen <= size );
			//
			// So we modified this assertion as an if clause.
			
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

			
			// we should move the frame inside cc2420 transceiver to MCU's RAM as fast as 
			// possible. they will reduce the possibility for frame dropping.
			//
			// if there's still some data pending inside the cc2420's RXFIFO buffer, then 
			// read them from cc2420 to mcu's ram. this often occurs when the sending 
			// rate is too high and exceeds the receiver's processing rate. 
			//
			if (CC_READ_FIFOP_PIN() && HAL_READ_CC_FIFO_PIN()) 
			{
				cc->rxlen = _cc2420_readrxfifo( cc, (char*)(&cc->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00 );

				/* @attention
				 * If the frame has an ACK request, then the transceiver cc2420's hardware will
				 * send ACK back automatically. However, you must enable the AUTOACK feature 
				 * of cc2420 during its initialization. 
				 * 
				 * Since cc2420 can send ACK automatically, we needn't to send ACK manually
				 * in the source code. 
				 * 
				 * If you want to send ACK manually, you can generate your own ACK frame similar
				 * to a normal data frame, or else send command SACK or SACKPEND to cc2420. 
				 * cc2420 can recognize the two commands and send ACK. 
				 *

				 * attention here cc->rxlen should be large enough or else this frame must 
				 * be an invalid frame, and it should be discarded. 
				 * attention cc->rxlen includes the first length byte.
				 */
				hal_assert( (cc->rxlen == 0) || (cc->rxlen >= CC2420_MIN_FRAME_LENGTH) );
				if (cc->rxlen >= CC2420_MIN_FRAME_LENGTH)
				{
					cc->rssi = cc->rxbuf[cc->rxlen-2];
					cc->lqi = cc->rxbuf[cc->rxlen-1];

					// If the crc checksum failed, then simply drop this frame.
					if(((cc->lqi >> 7) == 0x00))
					{
						cc->rxlen = 0;
					}
				}

				if ( CC_READ_FIFOP_PIN())
				{
					if ( !HAL_READ_CC_FIFO_PIN())
					{
						cc2420_sendcmd( cc, CC2420_SFLUSHRX );
						cc2420_sendcmd( cc, CC2420_SFLUSHRX );
						while (HAL_READ_CC_SFD_PIN()) {};
					}
				}
				/*
				if (HAL_READ_CC_FIFO_PIN()) 
				{
					cc2420_sendcmd( cc, CC2420_SFLUSHRX );
					cc2420_sendcmd( cc, CC2420_SFLUSHRX );
				}*/
			}
			else{ 
				// @attention: Needn't clear the cc2420 RXFIFO every time. This will
				// degrade the performance.
				//
			
				 cc2420_sendcmd( cc, CC2420_SFLUSHRX );
				 cc2420_sendcmd( cc, CC2420_SFLUSHRX );
				 
			}
			
		}

		/* If there's no frame pending inside the rxbuf, then try to pull a frame from 
		 * the transceiver into parameter buf directly. We must state that the whole
		 * cc2420 adapter should run corrected even without the following clause
		 * because the interrupt handler is able to place frame into cc->rxbuf. 
		 * 
		 * @attention If the interrupt is disabled, then the following clause will 
		 * still run and the cc2420 adapter can still work, which is running in query
		 * based mode.
		 */
		else{
			ret = 0;
			if (!HAL_READ_CC_FIFO_PIN())
			{
				hal_delayus( 2 );
				if (CC_READ_FIFOP_PIN())
				{
					ret = _cc2420_readrxfifo( cc, buf, size, 0x00 );
					
					// Send SFLUSHRX twice to clear cc2420's RXFIFO and wait SFD pin and 
					// FIFO pin go back to low. 
                  
					cc2420_sendcmd( cc, CC2420_SFLUSHRX );
					cc2420_sendcmd( cc, CC2420_SFLUSHRX );
					while (HAL_READ_CC_SFD_PIN()) {};
					
				}
			}
			
			
		
			/*
			if (CC_READ_FIFOP_PIN() && HAL_READ_CC_FIFO_PIN()) 
			{
				ret = _cc2420_readrxfifo( cc, buf, size, 0x00 );

				* @attention
				 * If the frame has an ACK request, then the transceiver cc2420's hardware will
				 * send ACK back automatically. However, you must enable the AUTOACK feature 
				 * of cc2420 during its initialization. 
				 * 
				 * Since cc2420 can send ACK automatically, we needn't to send ACK manually
				 * in the source code. 
				 * 
				 * If you want to send ACK manually, you can generate your own ACK frame similar
				 * to a normal data frame, or else send command SACK or SACKPEND to cc2420. 
				 * cc2420 can recognize the two commands and send ACK. 
				 *

				* attention here ret value should be large enough or else this frame must 
				 * be an invalid frame, and it should be discarded. 
				 *
				hal_assert( (ret == 0) || (ret >= CC2420_MIN_FRAME_LENGTH) );
				
				if (ret >= CC2420_MIN_FRAME_LENGTH)
				{
					cc->rssi = buf[ret-2];
					cc->lqi = buf[ret-1];

					// If the crc checksum failed, then simply drop this frame.
					if(((cc->lqi >> 7) == 0x00))
					{
						ret = 0;
					}
				}
				else{
					cc->rssi = 0;
					cc->lqi = 0;
					ret = 0;
				}
			}
			else{
				// @attention: Needn't clear the cc2420 RXFIFO every time. This will
				// degrade the performance.
				//
				// cc2420_sendcmd( cc, CC2420_SFLUSHRX );
				// cc2420_sendcmd( cc, CC2420_SFLUSHRX );
			}
			*/
		}
	}
    _cpu_atomic_end(cpu_status); 
  
	return ret;
}
/*
uint8 cc2420_iobsend( TiCc2420Adapter * cc, TiIoBuf * iobuf, uint8 option )
{
    return cc2420_send( cc, iobuf_ptr(iobuf), iobuf_length(iobuf), option );
}

uint8 cc2420_iobrecv( TiCc2420Adapter * cc, TiIoBuf * iobuf, uint8 option )
{
    uint8 count = cc2420_recv( cc, iobuf_ptr(iobuf), iobuf_size(iobuf), option );
    iobuf_setlength( iobuf, count );
    return count;
}
*/
void cc2420_evolve( TiCc2420Adapter * cc )
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
	{
		// @todo
		//cc2420_readrxfifo( cc );
		//m_fifop_request --;
		//m_fifop_request = 0;
	}
}


/* set the cc2420 transceiver into receiving mode
 */
inline uint8 cc2420_setrxmode( TiCc2420Adapter * cc )
{
	cc2420_sendcmd( cc, CC2420_SRXON );
	return 0;
}

/* set the cc2420 transceiver into sending mode
 */
inline uint8 cc2420_settxmode( TiCc2420Adapter * cc )
{
	cc2420_sendcmd( cc, CC2420_STXON );
	return 0;
}

/* cc2420 transceiver's mode in its datasheet
 *  - voltage regulation disabled (1uA)
 *  - power down (PD, voltage regulator enabled)  20uA
 *  - idle (XOSC running. Voltage regulator must be enabled in this case) 426uA
 *    attention some times the idle mode equals to power down mode in the datasheet.
 *  - receive mode 19.7 mA
 *  - transmit mode 8.5-17.4 mA
 * 
 * The "Voltage Regulator" can be enabled through VREG_EN pin.
 * The oscillator can be start/stop by issuing a command to cc2420 throught SPI interface.
 * 
 * sleep() and wakeup() are two standard interface functions which should be supported
 * by all hardware adapters. They're used to help implementing low power programming.
 */

inline uint8 cc2420_sleep( TiCc2420Adapter * cc )
{
	/* Assert the voltage regulator is on now. Actually, the voltage regulator is on
	 * since the device is powered on and never be turned off in this version. */
	return cc2420_oscoff( cc );
}

inline uint8 cc2420_wakeup( TiCc2420Adapter * cc )
{
	/* Assert the voltage regulator is already on now */
	return cc2420_oscon( cc );
}

/*******************************************************************************
 * cc2420 PIN based operations
 * These functions can be called in any case.
 ******************************************************************************/

/* Estimate whether the current channel is clear not, namely, the clear channel 
 * assesement (CCA) in CSMA protocol. 
 * 
 * @attention
 * - According to the description of cc2420 datasheet, the CCA pin is only
 * valid when the receiver has been enabled for at least 8 symbol periods. each symbol
 * periods is about 125us. So when you switch the transceiver mode from TX to RX, 
 * you should wait for at lease 125 us to wait for the CCA pin output correct value.
 *   It's the callers responsibility. So we don't implement the delay inside this
 * function. You can implement the delay by calling:
 *   hal_delayus( 125 )
 * Suggest use hal_delayus(150) instead of hal_delayus(125) because the delay is
 * not accurate.
 * 
 * - Before you use this function, you should enable the transceiver's CCA functionality
 * correctly. You should operate the following registers of cc2420:
 * 
 *   IOCFG0 register (address 0x1C): Controls the polority of CCA pin. (IOCFG0.CCA_POLARITY)
 * 
 *   RSSI.CCA_THR: The threshold value. Can be programmed in steps of 1 dB. The 
 * 		typical value is -77 for cc2420. 
 * 
 * 		ps: Jiang Ridong: suggest to set the value to -50. Attention the settins
 *		shouldn't exceed the transmitting power.
 * 
 *   MDMCTRL0.CCA_HYST: A CCA hysteresis can also be programmed in the MDMCTRL0.CCA_HYST control bits. 
 * 
 *   MDMCTRL0.CCA_MODE: select the CCA mode. Since the cc2420 is an 802.15.4 compatible 
 *      chip, we use choice 2 (clear channel when not receiving valid IEEE 802.15.4 data).
 *      A more strongly choice is 1(clear channel when received energy is below threshold)
 * 
 *   The above initialization process is usually done when the chip is firstly started
 * (usually in function cc2420_open(). In the current implementation, it should be 
 * placed into _cc2420_setreg() ). 
 * 
 * @reference
 * - Clear Channel Assessment, cc2420 datasheet, page 49, http://www.ti.com
 * - MDMCTRL0 register, cc2420 datasheet, page 63, http://www.ti.com
 */
uint8 cc2420_ischannelclear( TiCc2420Adapter * cc )
{
	/* For cc2420 transceiver, the clear channel assessment  signal  is based on
	 * based on the measured RSSI value and a programmable threshold. The  clear 
	 * channel assessment function is used  to implement  the CSMA-CA functionality. 
	 * CCA is valid  when  the receiver  has  been  enabled for at least 8 symbol periods. 
	 * 
	 * Carrier sense threshold  level  is programmed by  RSSI.CCA_THR. The threshold 
	 * value can be programmed in steps of 1 dB. A CCA hysteresis can also be 
	 * programmed in the MDMCTRL0.CCA_HYST control bits. All  3  CCA modes specified are 
	 * implemented in  CC2420. They are  set  in MDMCTRL0.CCA_MODE, as can be seen in 
	 * the  register description. The different modes are: 
	 * 	- 0  Reserved 
	 *  - 1  Clear channel when received energy is below threshold.  
	 *  - 2  Clear channel when not receiving valid IEEE 802.15.4 data. 
	 * 	- 3  Clear channel when energy is below threshold and not receiving valid IEEE 802.15.4 data 
	 * 
	 * Clear channel assessment is available on the CCA output pin. CCA is active high, 
	 * but the polarity may be changed by setting the IOCFG0.CCA_POLARITY control bit. 
	 * Implementing CSMA-CA may  easiest  be done by using  the  STXONCCA command 
	 * strobe,  as  described in the Radio control state  machine  section on page 42. 
	 * Transmission will then only  start  if  the channel  is clear. The  TX_ACTIVE status 
	 * bit  (see  Table 5) may be used to detect the result of the CCA. 
	 */

	// @todo
    // return HAL_READ_CC_CCA_PIN();
	return true;
}

/* turn on the cc2420 VREF
 * attention you should wait long enough to guarantee the voltage is stable and ok 
 */
inline uint8 cc2420_vrefon( TiCc2420Adapter * cc )
{
    HAL_SET_CC_VREN_PIN();                    //turn-on  
    hal_delayus( 1800 );    
	return 0;
}

/* turn off the cc2420 VREF */
inline uint8 cc2420_vrefoff( TiCc2420Adapter * cc )
{
	HAL_CLR_CC_VREN_PIN();                    //turn-off  
    hal_delayus( 1800 );  
	return 0;
}

inline uint8 cc2420_powerdown( TiCc2420Adapter * cc )
{
	return cc2420_vrefoff( cc );
}

/* Reference
 *	- cc2420 datasheet: state machine of cc2420.
 */
inline uint8 cc2420_powerup( TiCc2420Adapter * cc )
{
	return cc2420_vrefon( cc );
}

/*******************************************************************************
 * cc2420 command operations
 ******************************************************************************/

/* send a command to cc2420. This is function is very important because it's the 
 * foundation of all the other command functions.
 *
 * @return 
 *	command execution status
 */
uint8 cc2420_sendcmd( TiCc2420Adapter * cc, uint8 addr )
{
	uint8 status;

	cpu_atomic_t cpu_status = _cpu_atomic_begin();
	HAL_CLR_CC_CS_PIN(); 
	status = _cc2420_spi_put( addr );
    HAL_SET_CC_CS_PIN();
    _cpu_atomic_end(cpu_status); 
	
	return status;
}

uint8 cc2420_getcmdstatus( TiCc2420Adapter * cc) 
{
    return cc2420_sendcmd(cc,CC2420_SNOP);
}

/* No operation. Has no other effects than reading out cc2420's status-bit */
inline uint8 cc2420_snop( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_STXON );
}

/* start the 16MHz crystal oscllator required by cc2420 
 * @return 
 *	0		success
 *
 * @modified by zhangwei on 20090720
 *	- revision
 */
inline uint8 cc2420_oscon( TiCc2420Adapter * cc )
{
	uint8 i;
	uint8 status;
	bool osx_stable = FALSE;
	
	i = 0;
	cc2420_sendcmd( cc, CC2420_SXOSCON );
	while ((i < 200) && (osx_stable == FALSE))
	{
		hal_delayus(100);
		status = cc2420_sendcmd( cc, CC2420_SNOP );
		
		// bit6 of status: XOSC16M_STABLE bit. 1 means the 16MHz crystal oscllator is 
		// running successfully

		if (status & (1 << 6))
		{
	        osx_stable = TRUE;
			break;
        }

		i++;
    }
  
	if (!osx_stable) 
	{
		// attention: when there's problems, you can add the following assert() in
		// the source code and check whether the oscillator is really started or not.
		//hal_assert( false );

		return false;
    }
  
	return true;
}

/* turn-off cc2420's 16MHz crystal oscillator */
inline uint8 cc2420_oscoff( TiCc2420Adapter * cc ) 
{
	cc2420_sendcmd( cc, CC2420_SXOSCOFF );   
    return 0;
}

/* Waits for the crystal oscillator to become stable by polling the SPI status byte. 
 * You must wait until it is stable before doing further sending and receiving. 
 * 
 * @attention
 * Note that this function will lock up if the SXOSCON command strobe has not been 
 * given before the function call.
 */
void _cc2420_waitfor_crystal_oscillator( TiCc2420Adapter * cc ) 
{
	uint8 status;

	do{	   
		status = cc2420_sendcmd( cc, CC2420_SNOP );
	}while (!(status & (1 << CC2420_XOSC16M_STABLE)));
}

inline uint8 cc2420_calibrate( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_STXCAL );
}

inline uint8 cc2420_rxon( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_SRXON );
}

inline uint8 cc2420_txon( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_STXON );
}

/* similar to STXON. send a frame out. nothing happens if the channel is busy.
 * refer to: cc2420 datasheet on STXONCCA, radio control status machine, CCA
 *
 * - transmission is enabled by issuing a STXON or STXONCCA command strobe. see the 
 * radio control status machine section in cc2420 datasheet on Page42 for an illusion
 * of how the transmit command strobe affect the state of cc2420. 
 * - the STXONCCA strobe is ignored if the channel is busy. see the clear channel 
 * assessment section in cc2420 datasheet 49 for detail.
 */
inline uint8 cc2420_txoncca( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_STXONCCA );
}

inline uint8 cc2420_rfoff( TiCc2420Adapter * cc )
{
	cc2420_sendcmd( cc, CC2420_SRFOFF );
	return 0;
}

void cc2420_switchtomode( TiCc2420Adapter * cc, uint8 mode )
{
    // todo
}

inline uint8 cc2420_flushrx( TiCc2420Adapter * cc )
{
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );
	return 0;
}

inline uint8 cc2420_flushtx( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_SFLUSHTX );
}

inline uint8 cc2420_ack( TiCc2420Adapter * cc )              
{
	return cc2420_sendcmd( cc, CC2420_SACK );
}

inline uint8 cc2420_rxdec( TiCc2420Adapter * cc )               
{
	return cc2420_sendcmd( cc, CC2420_SRXDEC );
}

inline uint8 cc2420_txenc( TiCc2420Adapter * cc )              
{
	return cc2420_sendcmd( cc, CC2420_STXENC );
}


/*******************************************************************************
 * cc2420 register/memory access functions and configuration functions.
 ******************************************************************************/

/* write specific data on the specified address of cc2420 */
uint8 cc2420_writeregister( TiCc2420Adapter * cc, uint8 addr, uint16 data)
{
	uint8 status;
	
	cpu_atomic_t cpu_status = _cpu_atomic_begin();

	HAL_CLR_CC_CS_PIN();
	status = _cc2420_spi_put( addr );
	if (addr > 0x0E) 
	{
		_cc2420_spi_put( data >> 8 );
		_cc2420_spi_put((uint8)(data & 0xff));
	}
	_cpu_atomic_end(cpu_status); 
	HAL_SET_CC_CS_PIN();

	return status;
}

/* read data from specified address of cc2420 */
uint16 cc2420_readregister( TiCc2420Adapter * cc, uint8 addr )
{
	uint16 data = 0;
	uint8 status;

	cpu_atomic_t cpu_status = _cpu_atomic_begin();

	HAL_CLR_CC_CS_PIN();
	status = _cc2420_spi_put( addr | 0x40 );
	data = _cc2420_spi_put( 0 );

	data = (data << 8) | (_cc2420_spi_put( 0 ));
	HAL_SET_CC_CS_PIN();

	_cpu_atomic_end(cpu_status); 
	return data;
}

uint8 cc2420_readlargeram( TiCc2420Adapter * cc, uint16 addr, uint8 length, uint8 *buffer )
{
	uint8 i = 0;
	uint8 status;

	cpu_atomic_t cpu_status = _cpu_atomic_begin();

	HAL_CLR_CC_CS_PIN();
		  
	status =  _cc2420_spi_put((addr & 0x7F) | 0x80);
       
    status = _cc2420_spi_put((addr >> 1) & 0xe0);
    
	for (i = 0; i < length; i++) 
	{
		buffer[i]=_cc2420_spi_get();
    }
	
	HAL_SET_CC_CS_PIN();
    _cpu_atomic_end(cpu_status); 

	return 0;
}

//---------------------------------------------------------连续写cc2420ram
uint8 cc2420_writelargeram( TiCc2420Adapter * cc, uint16 addr, uint8 length, uint8 *buffer)
{
	uint8 i = 0;
	uint8 status;

	cpu_atomic_t cpu_status = _cpu_atomic_begin();

	HAL_CLR_CC_CS_PIN();
	
	status = _cc2420_spi_put((addr & 0x7F) | 0x80);
	
	status = _cc2420_spi_put((addr >> 1) & 0xC0)	;

	for (i = 0; i < length; i++) 
	{
		_cc2420_spi_put(buffer[i]);
	}

	HAL_SET_CC_CS_PIN();
    _cpu_atomic_end(cpu_status); 

	return 0;
}



//------------------------------------------------------------------
inline uint8 cc2420_enable_autoack( TiCc2420Adapter * cc )
{
	cc->param[CP_MDMCTRL0] |= (1 << CC2420_MDMCTRL0_AUTOACK);
    return cc2420_writeregister( cc, CC2420_MDMCTRL0,cc->param[CP_MDMCTRL0]);
}

inline uint8 cc2420_disable_autoack( TiCc2420Adapter * cc )
{
	cc->param[CP_MDMCTRL0] &= ~(1 << CC2420_MDMCTRL0_AUTOACK);
    return cc2420_writeregister( cc, CC2420_MDMCTRL0,cc->param[CP_MDMCTRL0]);
}

inline uint8 cc2420_enable_addrdecode( TiCc2420Adapter * cc )
{
    cc->param[CP_MDMCTRL0] |= (1 << CC2420_MDMCTRL0_ADRDECODE);
    return cc2420_writeregister( cc, CC2420_MDMCTRL0,cc->param[CP_MDMCTRL0]);
}

inline uint8 cc2420_disable_addrdecode( TiCc2420Adapter * cc )
{
    cc->param[CP_MDMCTRL0] &= ~(1 << CC2420_MDMCTRL0_ADRDECODE);
    return cc2420_writeregister( cc, CC2420_MDMCTRL0,cc->param[CP_MDMCTRL0]);
}


/*
void CC2420_SetCCAMode(USIGN8 mode)
{
USIGN16 temp;
temp = CC2420_ReadRegister(CC2420_MDMCTRL0);
CC2420_WriteRegister(CC2420_MDMCTRL0, ((temp & 0xff3f) | (mode<<6)));
}
USIGN8 CC2420_GetCCAMode()
{
return (CC2420_ReadRegister(CC2420_MDMCTRL0) & 0x00c0)>>6;
}
*/
void cc2420_setcoordinator( TiCc2420Adapter * cc, bool flag )
{
	uint16 temp;
	temp = cc2420_readregister( cc, CC2420_MDMCTRL0 );
	if (flag)//Coordinator and Auto Ack
		cc2420_writeregister( cc, CC2420_MDMCTRL0, (temp | 0x1010) );
	else    //other device type and Auto Ack
		cc2420_writeregister( cc, CC2420_MDMCTRL0, (temp | 0x0010) );
}

/* select the communication channel of cc2420 */
inline uint8 cc2420_setchannel( TiCc2420Adapter * cc, uint8 chn )
{
	int ctrl;

	hal_assert( (chn >= 11) && (chn <=26));

	ctrl = (uint16) (chn - 11);	             // subtract the base channel 
	ctrl = ctrl + (ctrl << 2);                   // multiply with 5, which is the channel spacing
	ctrl = ctrl + 357 + 0x4000;                  // 357 is 2405-2048, 0x4000 is LOCK_THR = 1

	cc->param[CP_FSCTRL] = ctrl;
	cc2420_writeregister( cc, CC2420_FSCTRL, ctrl );
	return 0;
}

/*
void CC2420_SetIEEEAddress(USIGN8 *ieeeaddr)
{
CC2420_WriteRam(CC2420_RAM_IEEEADR, 8, ieeeaddr);
}
void CC2420_GetIEEEAddress(USIGN8 *ieeeaddr)
{
CC2420_ReadRam(CC2420_RAM_PANID, 8, ieeeaddr);
}
*/

/* set the cc2420 short address
 * assume cc2420 is using short address with pan id currently 
 */
inline uint8 cc2420_setshortaddress( TiCc2420Adapter * cc, uint16 addr)
{
	addr = toLSB16(addr);
	cc2420_writelargeram( cc, CC2420_RAM_SHORTADDR, 2, (uint8 *)&addr );
	return 0;
}

inline uint8 cc2420_getshortaddress( TiCc2420Adapter * cc, uint16 * addr )
{
	cc2420_readlargeram( cc, CC2420_RAM_SHORTADDR, 2, (uint8 *)addr );
	return 0;
}

/* set cc2420 pan id 
 * assume the frame control bytes in the frame have correct settings on pan and 
 * address. 
 */
inline uint8 cc2420_setpanid( TiCc2420Adapter * cc, uint16 id )
{
	id = toLSB16(id);
    cc2420_writelargeram( cc, CC2420_RAM_PANID, 2, (uint8 *)&id );
    return 0;
}

inline uint8 cc2420_getpanid( TiCc2420Adapter * cc, uint16 * id )
{
	cc2420_readlargeram( cc, CC2420_RAM_PANID, 2, (uint8 *)&id );
	return 0;
}


inline uint8 cc2420_settxpower( TiCc2420Adapter * cc, uint8 power )
{
    uint16 ctrl;
    
    switch (power)
    {
	case CC2420_POWER_1:  ctrl = 0xa0e3; break;
    case CC2420_POWER_2:  ctrl = 0xa0e7; break;
    case CC2420_POWER_3:  ctrl = 0xa0eb; break;
    case CC2420_POWER_4:  ctrl = 0xa0ef; break;
    case CC2420_POWER_5:  ctrl = 0xa0f3; break;
    case CC2420_POWER_6:  ctrl = 0xa0f7; break;
    case CC2420_POWER_7:  ctrl = 0xa0fb; break;
    case CC2420_POWER_8:  ctrl = 0xa0ff; break;
    default :             ctrl = 0xa0ef; break; 
    }
    
    return cc2420_writeregister(cc, CC2420_TXCTRL, ctrl);     
}




/*******************************************************************************
 * cc2420 adapter enhanced functions
 ******************************************************************************/

/* return the rssi(received signal strength indicator) of the current frame just
 * read out by cc2420_recv().
 * 
 * cc2420_readrxfifo() will read the whole frame into cc->rxbuf. it will also read
 * the extra two bytes and place them into rxbuf. these two bytes are "rssi" and "lqi"
 * according to cc2420 datasheet. 
 * 
 * when the user call cc2420_recv() to retrieve the frame out from cc->rxbuf, then 
 * the cc2420_recv() function will return the data only and assign the last two bytes
 * to cc->rssi and cc->lqi here.  
 * 
 * Q: How cc2420 transceiver deals with RSSI, LQI and CRC result?
 * R: When MODEMCTRL0.AUTOCRC is set the two FCS bytes are replaced by the RSSI 
 * value, average correlation  value (used  for LQI) and CRC OK/not OK. 
 *    The  first FCS byte is replaced by the 8-bit RSSI value. The 7 least significant 
 * bits in the last FCS byte are replaced by the average correlation value. This 
 * correlation value may be used as a basis for calculating the LQI. The most 
 * significant bit in the last byte of each frame is set high if the CRC of the 
 * received frame is correct and low otherwise. 
 */ 
uint8 cc2420_rssi( TiCc2420Adapter * cc )
{
	return cc->rssi;
}

uint8 cc2420_lqi( TiCc2420Adapter * cc )
{
	return (cc->lqi & 0x7F);
}

bool cc2420_crctest( TiCc2420Adapter * cc )
{
	return ((cc->lqi >> 7) == 1);
}

TiFrameTxRxInterface * cc2420_interface( TiCc2420Adapter * cc, TiFrameTxRxInterface * intf )
{
    memset( intf, 0x00, sizeof(TiFrameTxRxInterface) );
    intf->provider = cc;
    intf->send = (TiFunFtrxSend)cc2420_send;
    intf->recv = (TiFunFtrxRecv)cc2420_recv;
    intf->evolve = (TiFunFtrxEvolve)cc2420_evolve;
    intf->switchtomode = (TiFunFtrxSwitchToMode)cc2420_switchtomode;
    intf->ischnclear = (TiFunFtrxIsChannelClear)cc2420_ischannelclear;
    intf->enable_autoack = (TiFunFtrxEnableAutoAck)cc2420_enable_autoack;
    intf->disable_autoack = (TiFunFtrxDisableAutoAck)cc2420_disable_autoack;
    intf->enable_addrdecode = (TiFunFtrxEnableAddrDecode)cc2420_enable_addrdecode;
    intf->disable_addrdecode = (TiFunFtrxDisableAddrDecode)cc2420_disable_addrdecode;
    intf->setchannel = (TiFunFtrxSetChannel)cc2420_setchannel;
    intf->setpanid = (TiFunFtrxSetPanId)cc2420_setpanid;
    intf->getpanid = (TiFunFtrxGetPanId)cc2420_getpanid;
    intf->setshortaddress = (TiFunFtrxSetShortAddress)cc2420_setshortaddress;
    intf->getshortaddress = (TiFunFtrxGetShortAddress)cc2420_getshortaddress;
    intf->settxpower = (TiFunFtrxSetTxPower)cc2420_settxpower;
    intf->getrssi = (TiFunFtrxGetRssi)cc2420_rssi;
    return intf;
}

/******************************************************************************
 * cc2420_dump
 * Dump cc2420's internal state to the debug output. This function is for debugging
 * use only.
 *****************************************************************************/

void cc2420_dump( TiCc2420Adapter * cc )
{
	#ifdef GDEBUG
    /*{
    FAST2420_GETREG(cc->spi,CC2420_MDMCTRL0, &rereg);   
    uart_putchar(g_uart,(char)rereg);
    uart_putchar(g_uart,(char)(rereg>>8));
    
    FAST2420_GETREG(cc->spi,CC2420_MDMCTRL1, &rereg);    
    uart_putchar(g_uart,(char)rereg);
    uart_putchar(g_uart,(char)(rereg>>8));
    
    FAST2420_GETREG(cc->spi,CC2420_IOCFG0, &rereg);    
    uart_putchar(g_uart,(char)rereg);
    uart_putchar(g_uart,(char)(rereg>>8));
    //FAST2420_GETREG(cc,CC2420_SECCTRL0, &rereg); 
    } */ 
    #endif
}

/*******************************************************************************
 * cc2420 interrupt related functions
 *
 * cc2420_enable_fifop
 * Enable the cc2420's FIFOP interrupt
 * 
 * cc2420_disable_fifop
 * Disable the cc2420's FIFOP interrupt
 *
 * cc2420_enable_sfd
 * Enable the cc2420's SFD interrupt
 * 
 * cc2420_disable_sfd
 * Disable the cc2420's SFD interrupt
 *
 *****************************************************************************/

/* Enable cc2420 FIFOP interrupt. This interrupt request is activated when a frame
 * is received by the transceiver successfully.
 * 
 * - according to the FIFOP description in cc2420's datasheet, the FIFOP goes to high
 * when the data length in RXFIFO exceed the threshold IOCFG0.FIFOP_THR or the 
 * frame is completely received in RXFIFO. 
 * 
 * - FIFOP goes to low once one bytes has been read out from RXFIFO.
 */
inline void cc2420_enable_fifop( TiCc2420Adapter * cc )
{
	switch (cc->id)
	{
	case 0:
		//EICRB &= ~(1 << 4);	// up edge of FIFOP to trigger the interrupt
								// register EICRB control the features of INT4,5,6,7
		EICRB |= (1 << 4);
		EICRB |= (1 << 5);
		EIMSK |= (1 << 6);		// = sbi(EIMSK, INT6)
		break;
	case 1:
		break;
	}
	return;
}

/* Disable cc2420 FIFOP interrupt. This interrupt request is activated when a frame
 * is received by the transceiver successfully.
 */ 
inline void cc2420_disable_fifop( TiCc2420Adapter * cc )
{
	switch (cc->id)
	{
	case 0:
	    EIMSK &= (1 << INT6);		// cbi( EIMSK, INT6 );
		break;
	case 1:
		break;
	}
}

/* Enable cc2420 SFD interrupt. This interrupt request is activated when a frame
 * is firstly arrived the transceiver and after the transceiver received the SFD
 * character. 
 */ 
inline void cc2420_enable_sfd( TiCc2420Adapter * cc )
{
	// @todo
}

/* Disable cc2420 SFD interrupt. This interrupt request is activated when a frame
 * is firstly arrived the transceiver and after the transceiver received the SFD
 * character. 
 */ 
inline void cc2420_disable_sfd( TiCc2420Adapter * cc )
{
	// @todo
}

/******************************************************************************
 * cc2420_default_listener
 * A callback listerner function can be used when call cc2420_open(). If the master
 * module doesn't provide a propriate listener function, then you can use the 
 * this default implementation and let the osx kernel help you do the left. 
 * The default listener will generate an event and put this event into a system
 * event queue.
 *****************************************************************************/

void cc2420_default_listener( void * ccptr, TiEvent * e )
{
	TiCc2420Adapter * cc = (TiCc2420Adapter *)ccptr;
	hal_notify_ex( EVENT_DATA_ARRIVAL, ccptr, cc->lisowner );
}


/******************************************************************************
 * cc2420 interrupt handler
 * it's called by the cc2420 FIFOP interrupt service handler (ISR). it should 
 * be called when a new frame received. you must call hal_attachhandler(), 
 * cc2420_enable_fifop() and hal_enable_interrupts() to fully enable this handler.
 *****************************************************************************/

/* @attention 
 * refer to cc2420 datasheet page 31
 *
 * - FIFO pin indicates whether there's data in the RXFIFO. and FIFO goes to low
 * when RXFIFO is empty. so you can also judge whether there's data pending in the 
 * RXFIFO by FIFO pin value.  
 *
 * @attention: the FIFOP interrupt is triggered by up edge at cc2420's FIFOP pin
 *	(connect to atmega128's INT6 PIN in GAINZ). you should configure the PIN 
 *  direction and interrupt features during initialization (down edge currently).
 *
 * - The FIFOP pin is high when the number of unread bytes in the RXFIFO exceeds 
 *   the threshold programmed into IOCFG0.FIFOP_THR
 *
 * - when address recognition is enabled the FIFOP will not go high until the 
 *   incoming frame passes address recognition. even if the number of bytes inRXFIFO 
 *   exceeds the programmed threshold
 *
 *	 => how should the interrupt handler do in this case? 
 *      it seems the cc2420 will clear the data in the RXFIFO if address recognition
 *      enabled and recognition failed. if then the interrupt handler needn't do 
 *      any thing. but how about the address recognition disabled?
 *
 * - FIFOP pin will also go high when the last byte of a new packet is received 
 *   even if the threshold is not exceeded. If so  the FIFOP  pin  will go back 
 *   to low once one byte has been read out of the RXFIFO. 
 * 
 *    => summary: FIFOP goes to high when the received bytes in RXFIFO exceed the 
 *       threshold or the whole frame is complete, no matter the address recognition
 *       is enabled or not.
 *    => FIFOP goes to low once one byte has been read out of the RXFIFO.
 *
 * - When address recognition is enabled, data should not be read out of the RXFIFO 
 *   before the address is completely received, since  the frame may be automatically 
 *   flushed by CC2420 if it fails  address recognition. This may be handled by 
 *   using the FIFOP pin, since this pin does not go high until the frame  passes  
 *   address recognition. 
 *
 * Q: what will be if a frame pending inisde cc2420? (???)
 * R: when the first frame arrives, both the FIFOP and FIFO go to high and the 
 *	FIFOP interrupt notify the MCU to process it. 
 *	  assume the MCU hasn't process the FIFOP interrupt in time, the frame will 
 *  be kept inside cc2420's RXFIFO and the second frame arrives now. then overflow 
 *  occurs and the FIFO pin goes to low. at this time, the FIFOP is still high
 *  so the interrupt still has chance to be executes. 
 * 
 *    attention the FIFOP interrupt will not raised to MCU because it's already 
 *  high in the overflow case. there's no chance to change from low to high (we
 *  use the up edge interrupt now).
 * 
 *    suggested processing: if overflow occurs, then you can still read the first 
 *  frame out. then the ISR should be:
 *    1) read first frame out. 
 *    2) check if overflow occurs. if true, then clear the RXFIFO
 *
 * @modified by zhangwei on 20090809
 *	- attention: Remove the debugging source code in this handler. Attention that 
 *  the hal_delay() shouldn't appeared inside this handler because it runs in interrupt 
 *  mode, except you know clearly what the future behavior will be. 
 */
void _cc2420_fifop_handler( void * ccptr, TiEvent * e )
{
    TiCc2420Adapter * cc = (TiCc2420Adapter *)ccptr;

	// HAL_READ_CC_FIFO_PIN() returns 0 means RXFIFO overflow. this should seldom 
	// occur, but the program should be able to recover from this case. 
	// 
	// the FIFO pin is set to high when the length byte of the frame is put into 
	// the RXFIFO and remains high if there's data in the RXFIFO.
	// 
	// RXFIFO may contains serveral serveral frames unless the total length is less
	// than 128. if overflow occurs, then cc2420 will signal to MCU by setting FIFO
	// to low while FIFOP is still high now. data already in the RXFIFO will not be 
	// affected by the overflow, i.e. frame already received maybe read out.
	//
	
	// The RXFIFO can only contain a maximum of 128 bytes at a given time. This 
	// may be divided between multiple frames, as long as the total number of bytes 
	// is 128 or less. If an overflow occurs in  the RXFIFO,  this is  signalled 
	// to the microcontroller by setting the FIFO pin low while the FIFOP pin is 
	// high. Data already in the  RXFIFO will  not  be  affected by the overflow, i.e. 
	// frames already received may be read out. 
	// 
	// A SFLUSHRX command strobe is required after a RXFIFO overflow to  enable 
	// reception  of new data. Note that the SFLUSHRX command strobe should be 
	// issued twice to ensure  that  the  SFD pin goes back to its idle state. 
	if (!HAL_READ_CC_FIFO_PIN())
	{
		// (HAL_READ_CC_FIFO_PIN() == 0) and (CC_READ_FIFOP_PIN() == 1) indicates
		// transceiver RXFIFO overflow. 
		//
		// @attention We should delay a little while to read the FIFOP pin. If we 
		// read FIFOP too fast, the cc2420 may still keeping FIFOP high(which is 
		// the interrupt request) and haven't enough time to make it back to low 
		// when encounter overflow. 
		// 
		// That's why the following source code already returns true:
		// 		if (CC_READ_FIFOP_PIN() && (!HAL_READ_CC_FIFO_PIN())) 
		// This one is better, however, it cannot guarantee the program work correctly
		// on all microcontrollers. So I finally decided to insert hal_delayus() 
		// here.
		
		if (CC_READ_FIFOP_PIN())
		{
			
			cc2420_sendcmd( cc, CC2420_SFLUSHRX );
			
			cc2420_sendcmd( cc, CC2420_SFLUSHRX );
			

			while (HAL_READ_CC_SFD_PIN()) {};
			return;
		}
    }
    
	// indicate the master program there's new frame coming. you can use either the 
	// share global variable, event queue or semaphore. we use event queue in openwsn
	// implemention. 
	//
	// @attention 
	// - the listener or the master program must read the frame out from RXFIFO as 
	// soon as possible or else the next frame will be padded to the first one. the 
	// current program doesn't take this case into account. 
	// - if there's overflow in the RXFIFO, then the first correct frame will also be 
	// cleared. 

	if (cc->listener != NULL)
	{
		// @attention
		// @warning
		// if you use listener, the master program must repeated call cc2420_recv()
		// to deal with the possible overflow cases. if you call cc2420_recv() only 
		// one time in the listener, then the listener will be never called for the 
		// second time if RXFIFO is overflow. => this may lead the whole application
		// looks deadlock and receive any frames therefore.
		//
		cc->rxlen = _cc2420_readrxfifo( cc, (char*)(&cc->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00 );

		cc->listener( cc->lisowner, NULL );
	}
	else if (cc->rxlen == 0)
	//if (cc->rxlen == 0)
	{
		
		// @attention
		// @warning
		//	If the master program cannot serve the incoming frames fast enough, then the
		// newly incoming frames will overwrite the last one. This will lead to 
		// frame lossing.
		cc->rxlen = _cc2420_readrxfifo( cc, (char*)(&cc->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00 );
	}
	else{

		// @modified by zhangwei on 20090919
		// The above analysis is correct. In order to simplify the whole process,
		// I add the following line of source code here. The call of cc2420_readrxfifo() 
		// will overwrite last frame inside the rxbuf. 
		// I assume the new incoming frame always has the high priority than the elder one.
		//
		cc->rxlen = _cc2420_readrxfifo( cc, (char*)(&cc->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00 );
	}

	// attention: there's a little chance that 2 or more frames pending inside the TXFIFO
	// if so, you need to try to read them out.
	
	// @todo
	// suggest continue reading until reading all the data from cc2420 FIFO and place
	// them into MCU's buffer. 
	// suggest read out the above data and discard them
	// if FIFO
	//   read all left data out and discard 
	// endif

    return;
}

/* For high precision time synchronization
 * This handler will be called by hardware timer's capture interrupt. This interrupt
 * will be fired when the SFD pin changes. 
 * 
 * Q: Where can I found the interrupt configuration source code?
 * R: hal_interrupt.c
 */
#ifdef CONFIG_CC2420_SFD
void _cc2420_sfd_handler( void * ccptr, TiEvent * e )
{
	TiCc2420Adapter * cc = (TiCc2420Adapter *)ccptr;
	// todo stop the timer capture here
	// xiejing: probably, you should read the timer value out here
	led_toggle( LED_RED );
}
#endif
