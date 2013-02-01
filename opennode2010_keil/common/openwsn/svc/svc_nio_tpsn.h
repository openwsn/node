#ifndef _SVC_TPSN_H_2543_
#define _SVC_TPSN_H_2543_

/*******************************************************************************
 * @author Shi Zhirong on 2013.01.30
 * 	- revision today.
 ******************************************************************************/
 
/*******************************************************************************
 * TPSN Header := [TPSN Protocol Identifier 1B][FRAME TYPE 1B][Source Address 2B][Destination Address 2B]
 *               [T3 4B][T4 4B][T1 4B][T2 4B] 
 * TPSN_FRAME_REQUEST is [T1 4B][T2 4B][NULL 4B][NULL 4B]
 * when the second node receive the TPSN_FRAME_REQUEST,move the T1 and T2.
 * TPSN_FRAME_RESPONSE is [T3 4B][T4 4B][T1 4B][T2 4B] 
 ******************************************************************************/

 
#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_time.h"
#include "svc_nio_mac.h"
#include "svc_nodebase.h"
#include "svc_nio_acceptor.h"
#include "svc_nio_datatree.h"

#define TPSN_PROTOCOL_IDENTIFER TSYNC_PROTOCAL_ID
#define TPSN_MAX_FRAME_SIZE	MAX_IEEE802FRAME154_SIZE

#define TPSN_FRAME_REQUEST 0
#define TPSN_FRAME_RESPONSE 1

#define TPSN_MAKE_TIME(highest,high,lowe,lowest) (((uint32)highest<<24) | ((uint32)high<<16)|((uint16)lowe<<8)|((uint8)lowest))
#define TPSN_MAKEWORD(high,low) (((uint16)high<<8) | ((uint8)low))

#define TPSN_TYPE(pkt) ((pkt)[1])
#define TPSN_SHORTADDRFROM(pkt) TPSN_MAKEWORD((pkt)[3],(pkt)[2])
#define TPSN_SHORTADDRTO(pkt) TPSN_MAKEWORD((pkt)[5],(pkt)[4])

#define TPSN_SET_PROTOCAL_IDENTIFIER(pkt,value) (pkt)[0]=(value)
#define TPSN_SET_TYPE(pkt,value) (pkt)[1]=(value)
#define TPSN_SET_SHORTADDRFROM(pkt,addr) {(pkt)[2]=((uint8)(addr&0xFF)); (pkt)[3]=((uint8)(addr>>8));}
#define TPSN_SET_SHORTADDRTO(pkt,addr) {(pkt)[4]=((uint8)(addr&0xFF)); (pkt)[5]=((uint8)(addr>>8));}

#pragma pack(1)
typedef struct
{
    TiNioNetLayerDispatcher * dispatcher;
	TiRtcAdapter * 		rtc;
    TiTimeSyncAdapter * hsyn;
	uint8 id;
	uint8 state;
}TiNioTPSN;

TiNioTPSN *tpsn_construct( char * buf,uintx size);
void tpsn_destroy(TiNioTPSN * tpsn);
TiNioTPSN * tpsn_open( 	TiNioTPSN * tpsn,TiRtcAdapter * rtc,TiNioNetLayerDispatcher *dispatcher,TiTimeSyncAdapter *hsyn );
//be used in timesync_task 
TiNioTPSN * tpsn_send( TiNioTPSN * tpsn, uint16 addr, TiFrame *frame );
//dispatcher rxhandler
intx nio_tpsn_rxhandler( void * object, TiFrame * input, TiFrame * output, uint8 option );
void tpsn_evolve( void * object, TiEvent * e);


#endif /* _SVC_TPSN_H_2543_ */
