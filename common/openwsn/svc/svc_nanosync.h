
/**
 * CONFIG_NANOSYNC_IDENTIFIER 
 * This value indicate the frame is a nanosync protocol frame. If the value is conflicated
 * with other protocols, you can change this value. Attention all the nodes in the network
 * should agree one unique protocol identifier.
 */
#define CONFIG_NANOSYNC_IDENTIFIER 0x67

#include "svc_configall.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_frame_transceiver.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_rtc.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "svc_foundation.h"

#define NANOSYNC_MASTER 1
#define NANOSYNC_SLAVE 0

#define NANOSYNC_CMD_REQUEST 1
#define NANOSYNC_CMD_RESPONSE 0

#ifdef _cplusplus
extern "c"{
#endif

typedef struct
{
	uint8 mode;
	//TiCalTime remotetime;
	TiFrameTxRxInterface * rxtx;
    TiRtc * rtc;
	uint16 panto;
	uint16 shortaddrto;
	uint16 panfrom;
	uint16 shortaddrfrom;
	uint8 seqid;
	uint8 cmd;
    char txbuf[80];//之前是127
	TiIEEE802Frame154Descriptor desc;
	TiFunEventHandler handler;
	uint8 option;
}TiNanoSync;

TiNanoSync * nanosync_construct( char * buf,uintx size);
void nanosync_destroy(TiNanoSync * sync);

/**
 * open an TiNanoSync component. after open, you can call its evolve function to
 * perform time sync operations
 */
TiNanoSync * nanosync_open( TiNanoSync * sync, uint8 mode, TiRtc * rtc, TiFrameTxRxInterface * rxtx, 
    uint8 chn, uint16 panid, uint16 address, char * buf, uint16 period, uint8 option );

void        nanosync_close( TiNanoSync * sync );

uintx       nanosync_send( TiNanoSync * sync, char * txbuf, uint8 option );

/**
 * 广播给相邻节点
 * @param sync TiNanoSync object
 * @param frame pointer to the frame buffer allocated by the master program. nanosync doesn't manage the buffer itself.
 * @return >=0 when succeed
 */
uintx       _nanosync_broadcast( TiNanoSync * sync,  uint8 option );

uintx       nanosync_recv( TiNanoSync * sync, char * buf,uint8 len, uint8 option );//uintx       nanosync_recv( TiNanoSync * sync, char * buf, uint8 option );

/**
 * execute/start time sync operation in the whole network
 */
uintx nanosync_master_evolve( TiNanoSync * sync,  uint8 option );

/**
 * execute/start time sync operation in the whole network
 */
uintx nanosync_slave_evolve( TiNanoSync * sync, char * buf, uint8 len,uint8 option);

/**
 * return the TiRtc component related to the current TiNanoSync component.
 */
TiRtc * nanosync_realtime_clock( TiNanoSync * sync );

/**
 * return the current timestamp kept by the TiNanoSync component. 
 */
//void nanosync_localtime( TiNanoSync * sync, TiCalTime * localtime );
//void nanosync_set_localtime( TiNanoSync * sync, TiCalTime * localtime );

void nanosync_update( TiNanoSync * sync, char * rxbuf, uint8 len );

#ifdef _cplusplus
}
#endif

