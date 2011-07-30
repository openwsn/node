#ifndef _HAL_CC2520_H_4842_
#define _HAL_CC2520_H_4842_

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cc2520base.h"
#include "../../rtl/rtl_iobuf.h"
#include "../../rtl/rtl_ieee802frame154.h"
#include "hal_frame_transceiver.h"


/*******************************************************************************
 * cc2520 adapter
 * Simple wraper of cc2520 low power wireless transceiver. it helps to implement 
 * the MAC network layer. 
 * 
 * @state
 *	developing
 * 
 * @history
 * @author zhangwei on 2010.04
 * @modified by zhangwei on 2011.06.04
 *	- Revision. Compiled successfully.
 *
 ******************************************************************************/



#ifdef __cplusplus
extern "C"{
#endif

#define CC2520_RXBUF_SIZE 0x7F

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
typedef struct{
    uint8 id;
	uint8 state;
	TiFunEventHandler listener;
	void * lisowner;
    uint8 option;
	volatile uint8 rxlen;
	volatile char rxbuf[CC2520_RXBUF_SIZE];
	//char ackbuf[CC2520_ACKBUFFER_SIZE];
	uint8 rssi;
	uint8 lqi;
	volatile uint8 spistatus;
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

uint8 cc2520_state( TiCc2520Adapter * cc );

void cc2520_restart( TiCc2520Adapter * cc );

#define cc2520_write(cc,buf,len,option) cc2520_send(cc,buf,len,option)
#define cc2520_read(cc,buf,len,option) cc2520_recv(cc,buf,len,option)

uint8 cc2520_send( TiCc2520Adapter * cc, char * buf, uint8 len, uint8 option );
uint8 cc2520_broadcast( TiCc2520Adapter * cc, char * buf, uint8 len, uint8 option );
uint8 cc2520_recv( TiCc2520Adapter * cc, char * buf, uint8 size, uint8 option );


void cc2520_enable_fifop( TiCc2520Adapter * cc );
void cc2520_disable_fifop( TiCc2520Adapter * cc );

void cc2520_enable_sfd( TiCc2520Adapter * cc );
void cc2520_disable_sfd( TiCc2520Adapter * cc );
//void cc2520_set_sfd_listener( TiCc2520Adapter * cc, TiFunEventHandler sfd_listener, void * owner );

void cc2520_default_listener( void * ccptr, TiEvent * e ); 



/***********************************************************************************
* INCLUDES
*/
#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cc2520.h"

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



