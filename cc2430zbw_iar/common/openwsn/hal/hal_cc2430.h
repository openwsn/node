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

#ifndef _HAL_CC2430_H_4842_
#define _HAL_CC2430_H_4842_

/*******************************************************************************
 * @file    hal_cc2430.h
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
#define CC2430_RXBUF_SIZE 128
#define CC2430_TXBUF_SIZE 128

/** 
 * The following macros are for testing only. You should change it to adapt to your 
 * own settings.
 */
#define CC2430_DEF_PANID                0x0001
#define CC2430_DEF_LOCAL_ADDRESS        0x0001   
#define CC2430_DEF_REMOTE_ADDRESS       0x0002
#define CC2430_DEF_CHANNEL              11

#ifdef __cplusplus
extern "C"{
#endif

/* TiCc2430Adapter
 * is a lightweight wrapper of TI/Chipcon's cc2420 2.4G transceiver. 
 * design principle: lightweight, fast, simple, easy for portable
 *
 *	id				         cc2420 identifier. should always be 0 currently
 *	state			         state of the current TiCc2430Adapter object
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
	volatile uint8 rxbuf[CC2430_RXBUF_SIZE];
	//char ackbuf[CC2430_ACKBUFFER_SIZE];
	// uint8 rssi;
	// uint8 lqi;
	// volatile uint8 spistatus;
	uint16 param[14];
}TiCc2430Adapter;


/*******************************************************************************
 * cc2430_construct
 * Construct the cc2420 object inside the specified memory block
 * 
 * @return 
 * 	The pointer to the TiCc2430Adapter object.
 ******************************************************************************/
TiCc2430Adapter * cc2430_construct( void * mem, uint16 size );

/*******************************************************************************
 * cc2430_destroy
 * Destroy the memory block allocated to cc2420 object
 ******************************************************************************/
void cc2430_destroy( TiCc2430Adapter * cc );

/*******************************************************************************
 * cc2430_open
 * Initialize cc2420 object for read/write. 
 * 
 * @return 
 * 	The pointer to the TiCc2430Adapter object if opened successfully. 
 ******************************************************************************/
TiCc2430Adapter * cc2430_open( TiCc2430Adapter * cc, uint8 id, TiFunEventHandler listener, void * lisowner, uint8 option );

/*******************************************************************************
 * cc2430_close
 * Close cc2420 object. Disable related interrupts, and release allocated resources
 * allocated in the cc2430_open() function.
 ******************************************************************************/
void cc2430_close( TiCc2430Adapter * cc );
void cc2430_sleep( TiCc2430Adapter * cc );

uint8 cc2430_state( TiCc2430Adapter * cc );

void cc2430_restart( TiCc2430Adapter * cc );
void cc2430_wakeup( TiCc2430Adapter * cc );

uint8 cc2430_vrefon( TiCc2430Adapter * cc );
uint8 cc2430_vrefoff( TiCc2430Adapter * cc );
uint8 cc2430_powerdown( TiCc2430Adapter * cc );
uint8 cc2430_powerup( TiCc2430Adapter * cc );

#define cc2430_write(cc,buf,len,option) cc2430_send(cc,buf,len,option)
#define cc2430_read(cc,buf,len,option) cc2430_recv(cc,buf,len,option)

intx cc2430_send( TiCc2430Adapter * cc, char * buf, uintx len, uint8 option );
intx cc2430_broadcast( TiCc2430Adapter * cc, char * buf, uintx len, uint8 option );
intx cc2430_recv( TiCc2430Adapter * cc, char * buf, uintx size, uint8 option );


void cc2430_enable_fifop( TiCc2430Adapter * cc );
void cc2430_disable_fifop( TiCc2430Adapter * cc );

void cc2430_enable_sfd( TiCc2430Adapter * cc );
void cc2430_disable_sfd( TiCc2430Adapter * cc );
//void cc2430_set_sfd_listener( TiCc2430Adapter * cc, TiFunEventHandler sfd_listener, void * owner );

void cc2430_default_listener( void * ccptr, TiEvent * e ); 

void cc2430_evolve( TiCc2430Adapter * cc );

uint8 cc2430_ischannelclear( TiCc2430Adapter * cc );
uint8 cc2430_snop( TiCc2430Adapter * cc );
uint8 cc2430_oscon( TiCc2430Adapter * cc );
uint8 cc2430_oscoff( TiCc2430Adapter * cc );
uint8 cc2430_calibrate( TiCc2430Adapter * cc );
uint8 cc2430_rxon( TiCc2430Adapter * cc );
uint8 cc2430_txon( TiCc2430Adapter * cc );
uint8 cc2430_txoncca( TiCc2430Adapter * cc );
uint8 cc2430_rfoff( TiCc2430Adapter * cc );   
void  cc2430_switchtomode( TiCc2430Adapter * cc, uint8 mode );

uint8 cc2430_flushrx( TiCc2430Adapter * cc );
uint8 cc2430_flushtx( TiCc2430Adapter * cc );

uint8 cc2430_writeregister( TiCc2430Adapter * cc, uint8 addr, uint8 data);
uint8 cc2430_readregister( TiCc2430Adapter * cc, uint8 addr);

uint8 cc2430_enable_autoack( TiCc2430Adapter * cc ) ;
uint8 cc2430_disable_autoack( TiCc2430Adapter * cc );

uint8 cc2430_enable_addrdecode( TiCc2430Adapter * cc );
uint8 cc2430_disable_addrdecode( TiCc2430Adapter * cc ) ;

uint8 cc2430_enable_filter( TiCc2430Adapter * cc );
uint8 cc2430_disable_filter( TiCc2430Adapter * cc ) ;

//以下5组函数只有在filter功能实现的时候才有意义
//define whether the reserved frames are accepted or rejected
uint8 cc2430_reserved_accept( TiCc2430Adapter * cc );
uint8 cc2430_reserved_reject( TiCc2430Adapter * cc ) ;

//define whether the CMD frames are accepted or rejected
uint8 cc2430_cmd_accept( TiCc2430Adapter * cc );
uint8 cc2430_cmd_reject( TiCc2430Adapter * cc ) ;

//define whether the CMD frames are accepted or rejected
uint8 cc2430_ack_accept( TiCc2430Adapter * cc );
uint8 cc2430_ack_reject( TiCc2430Adapter * cc ) ;

//define whether the data frames are accepted or rejected 
uint8 cc2430_data_accept( TiCc2430Adapter * cc );
uint8 cc2430_data_reject( TiCc2430Adapter * cc ) ;

//define whether the beacon frames are accepted or rejected 
uint8 cc2430_beacon_accept( TiCc2430Adapter * cc );
uint8 cc2430_beacon_reject( TiCc2430Adapter * cc ) ;

void  cc2430_setcoordinator( TiCc2430Adapter * cc, bool flag );

uint8 cc2430_setchannel( TiCc2430Adapter * cc, uint8 chn );
uint8 cc2430_setshortaddress( TiCc2430Adapter * cc, uint16 addr );
uint8 cc2430_getshortaddress( TiCc2430Adapter * cc, uint16 * addr );
uint8 cc2430_setpanid( TiCc2430Adapter * cc, uint16 id );
uint8 cc2430_getpanid( TiCc2430Adapter * cc, uint16 * id );

uint8 cc2430_settxpower( TiCc2430Adapter * cc, uint8 power );

uint8 cc2430_rssi( TiCc2430Adapter * cc );

void cc2430_setlistener(TiCc2430Adapter * cc, TiFunEventHandler listener, void * lisowner );



TiFrameTxRxInterface * cc2430_interface( TiCc2430Adapter * cc, TiFrameTxRxInterface * intf );


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

// formerly as halRfGetChipId
uint8 cc2430_chipid(void);
uint8 cc2430_chipver(void);
HAL_RF_STATUS halRfGetStatusByte(void);
uint8 halRfGetRandomByte(void);
uint8 halRfGetRssiOffset(void);

/*
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
*/

#ifdef __cplusplus
}
#endif

#endif /* _HAL_CC2430_H_4842_ */



