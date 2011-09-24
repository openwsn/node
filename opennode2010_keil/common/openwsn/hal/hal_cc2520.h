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

#ifndef _HAL_CC2520_H_4842_
#define _HAL_CC2520_H_4842_

/*******************************************************************************
 * @file    hal_cc2520.h
 * @author  Zhang Wei, Jiang Ridong
 * @version V 1.0.0
 * @date    11-March-2011
 * @brief   This file contains the adapter object of cc2520 transceiver. It's an 
 *          Simple wraper of cc2520 low power wireless transceiver. 
 *
 * @state released
 * 
 * @history
 * @author zhangwei on 2010.04
 * @modified by zhangwei on 2011.06.04
 *	- Revision. Compiled successfully.
 * @modified by Jiang Ridong  in 2011.07
 *  - Finished and tested Ok.
 * @modified by Zhang Wei on 2011.08.14
 *  - Revision
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_frame_transceiver.h"

/* The following constants represent the size of cc2520 RXBUF and TXBUF size. You 
 * should NOT change their value. 
 * 
 * @attention The first byte in the buffer is the length byte. The value of the length
 * byte doesn't include itself.
 */
#define CC2520_RXBUF_SIZE 128
#define CC2520_TXBUF_SIZE 128

/** 
 * The following macros are for testing only. You should change it to adapt to your 
 * own settings.
 */
#define CC2520_DEF_PANID                0x0001
#define CC2520_DEF_LOCAL_ADDRESS        0x0001   
#define CC2520_DEF_REMOTE_ADDRESS       0x0002
#define CC2520_DEF_CHANNEL              11

#ifdef __cplusplus
extern "C"{
#endif

/* TiCc2520Adapter
 * is a lightweight wrapper of TI/Chipcon's cc2420 2.4G transceiver. 
 * design principle: lightweight, fast, simple, easy for portable
 *
 *	id				         cc2420 identifier. should always be 0 currently
 *	state			         state of the current TiCc2520Adapter object
 *	rssi                     received signal strength indicator
 *  lqi                      link quality indicaator
 *	//spistatus              spistatus in the last operation. for debugging only.
 */

#pragma pack(1) 
typedef struct{
    uint8 id;
	uint8 state;
	TiFunEventHandler listener;
	void * lisowner;
    uint8 option;
	volatile uint8 rxlen;
	volatile uint8 rxbuf[CC2520_RXBUF_SIZE];
	//char ackbuf[CC2520_ACKBUFFER_SIZE];
	// uint8 rssi;
	// uint8 lqi;
	// volatile uint8 spistatus;
	uint16 param[14];
}TiCc2520Adapter;


/*******************************************************************************
 * cc2520_construct
 * Construct the cc2420 object inside the specified memory block
 * 
 * @return 
 * 	The pointer to the TiCc2520Adapter object.
 ******************************************************************************/
TiCc2520Adapter * cc2520_construct( void * mem, uint16 size );

/*******************************************************************************
 * cc2520_destroy
 * Destroy the memory block allocated to cc2420 object
 ******************************************************************************/
void cc2520_destroy( TiCc2520Adapter * cc );

/*******************************************************************************
 * cc2520_open
 * Initialize cc2420 object for read/write. 
 * 
 * @return 
 * 	The pointer to the TiCc2520Adapter object if opened successfully. 
 ******************************************************************************/
TiCc2520Adapter * cc2520_open( TiCc2520Adapter * cc, uint8 id, TiFunEventHandler listener, void * lisowner, uint8 option );

/*******************************************************************************
 * cc2520_close
 * Close cc2420 object. Disable related interrupts, and release allocated resources
 * allocated in the cc2520_open() function.
 ******************************************************************************/
void cc2520_close( TiCc2520Adapter * cc );
void cc2520_sleep( TiCc2520Adapter * cc );

uint8 cc2520_state( TiCc2520Adapter * cc );

void cc2520_restart( TiCc2520Adapter * cc );
void cc2520_wakeup( TiCc2520Adapter * cc );

uint8 cc2520_vrefon( TiCc2520Adapter * cc );
uint8 cc2520_vrefoff( TiCc2520Adapter * cc );
uint8 cc2520_powerdown( TiCc2520Adapter * cc );
uint8 cc2520_powerup( TiCc2520Adapter * cc );

#define cc2520_write(cc,buf,len,option) cc2520_send(cc,buf,len,option)
#define cc2520_read(cc,buf,len,option) cc2520_recv(cc,buf,len,option)

intx cc2520_send( TiCc2520Adapter * cc, char * buf, uintx len, uint8 option );
intx cc2520_broadcast( TiCc2520Adapter * cc, char * buf, uintx len, uint8 option );
intx cc2520_recv( TiCc2520Adapter * cc, char * buf, uintx size, uint8 option );


void cc2520_enable_fifop( TiCc2520Adapter * cc );
void cc2520_disable_fifop( TiCc2520Adapter * cc );

void cc2520_enable_sfd( TiCc2520Adapter * cc );
void cc2520_disable_sfd( TiCc2520Adapter * cc );
//void cc2520_set_sfd_listener( TiCc2520Adapter * cc, TiFunEventHandler sfd_listener, void * owner );

void cc2520_default_listener( void * ccptr, TiEvent * e ); 

void cc2520_evolve( TiCc2520Adapter * cc );

uint8 cc2520_ischannelclear( TiCc2520Adapter * cc );
uint8 cc2520_snop( TiCc2520Adapter * cc );
uint8 cc2520_oscon( TiCc2520Adapter * cc );
uint8 cc2520_oscoff( TiCc2520Adapter * cc );
uint8 cc2520_calibrate( TiCc2520Adapter * cc );
uint8 cc2520_rxon( TiCc2520Adapter * cc );
uint8 cc2520_txon( TiCc2520Adapter * cc );
uint8 cc2520_txoncca( TiCc2520Adapter * cc );
uint8 cc2520_rfoff( TiCc2520Adapter * cc );   
void  cc2520_switchtomode( TiCc2520Adapter * cc, uint8 mode );

uint8 cc2520_flushrx( TiCc2520Adapter * cc );
uint8 cc2520_flushtx( TiCc2520Adapter * cc );

uint8 cc2520_writeregister( TiCc2520Adapter * cc, uint8 addr, uint8 data);
uint8 cc2520_readregister( TiCc2520Adapter * cc, uint8 addr);

uint8 cc2520_enable_autoack( TiCc2520Adapter * cc ) ;
uint8 cc2520_disable_autoack( TiCc2520Adapter * cc );

uint8 cc2520_enable_addrdecode( TiCc2520Adapter * cc );
uint8 cc2520_disable_addrdecode( TiCc2520Adapter * cc ) ;

uint8 cc2520_enable_filter( TiCc2520Adapter * cc );
uint8 cc2520_disable_filter( TiCc2520Adapter * cc ) ;

//以下5组函数只有在filter功能实现的时候才有意义
//define whether the reserved frames are accepted or rejected
uint8 cc2520_reserved_accept( TiCc2520Adapter * cc );
uint8 cc2520_reserved_reject( TiCc2520Adapter * cc ) ;

//define whether the CMD frames are accepted or rejected
uint8 cc2520_cmd_accept( TiCc2520Adapter * cc );
uint8 cc2520_cmd_reject( TiCc2520Adapter * cc ) ;

//define whether the CMD frames are accepted or rejected
uint8 cc2520_ack_accept( TiCc2520Adapter * cc );
uint8 cc2520_ack_reject( TiCc2520Adapter * cc ) ;

//define whether the data frames are accepted or rejected 
uint8 cc2520_data_accept( TiCc2520Adapter * cc );
uint8 cc2520_data_reject( TiCc2520Adapter * cc ) ;

//define whether the beacon frames are accepted or rejected 
uint8 cc2520_beacon_accept( TiCc2520Adapter * cc );
uint8 cc2520_beacon_reject( TiCc2520Adapter * cc ) ;

void  cc2520_setcoordinator( TiCc2520Adapter * cc, bool flag );

uint8 cc2520_setchannel( TiCc2520Adapter * cc, uint8 chn );
uint8 cc2520_setshortaddress( TiCc2520Adapter * cc, uint16 addr );
uint8 cc2520_getshortaddress( TiCc2520Adapter * cc, uint16 * addr );
uint8 cc2520_setpanid( TiCc2520Adapter * cc, uint16 id );
uint8 cc2520_getpanid( TiCc2520Adapter * cc, uint16 * id );

uint8 cc2520_settxpower( TiCc2520Adapter * cc, uint8 power );

uint8 cc2520_rssi( TiCc2520Adapter * cc );

void cc2520_setlistener(TiCc2520Adapter * cc, TiFunEventHandler listener, void * lisowner );



TiFrameTxRxInterface * cc2520_interface( TiCc2520Adapter * cc, TiFrameTxRxInterface * intf );


/***********************************************************************************
* TYPEDEFS
*/
// The chip status byte, returned by chip for all SPI accesses
typedef uint8 HAL_RF_STATUS;

/***********************************************************************************
* CONSTANTS AND DEFINES
*/

#define KEY_LENGTH              16
#define NONCE_LENGTH            16

// TX power
#define TXPOWER_MIN_4_DBM       0
#define TXPOWER_0_DBM           1
#define TXPOWER_4_DBM           2

/***********************************************************************************
* GLOBAL FUNCTIONS
*/
void halRfPowerUp(void);
void halRfResetChip(void);
HAL_RF_STATUS halRfInit(void);
HAL_RF_STATUS halRfWaitXoscStable(void);

uint8 halRfGetChipId(void);
uint8 halRfGetChipVer(void);
HAL_RF_STATUS halRfGetStatusByte(void);
uint8 halRfGetRandomByte(void);
uint8 halRfGetRssiOffset(void);

void halRfSetChannel(uint8 channel);
HAL_RF_STATUS halRfSetShortAddr(uint16 shortAddr);
HAL_RF_STATUS halRfSetPanId(uint16 PanId);
uint8 halRfSetPower(uint8 power);

HAL_RF_STATUS halRfRecvFrame(uint8* data, uint8 length);
HAL_RF_STATUS halRfWriteTxBuf(uint8* data, uint8 length);
HAL_RF_STATUS halRfReadRxBuf(uint8* data, uint8 length);
uint8 halRfTransmitCCA(void);

void halRfSecurityInit(uint8* key, uint8* nonceRx, uint8* nonceTx);
uint8 halRfRecvFrameUCCM(uint8* data, uint8 length, uint8 encrLength, uint8 authLength, uint8 m);
HAL_RF_STATUS halRfWriteTxBufCCM(uint8* data, uint8 length, uint8 encrLength, uint8 authLength, uint8 m);
void halRfIncNonceTx(void);

HAL_RF_STATUS halRfReceiveOn(void);
HAL_RF_STATUS halRfReceiveOff(void);


#ifdef __cplusplus
}
#endif

#endif /* _HAL_CC2520_H_4842_ */



