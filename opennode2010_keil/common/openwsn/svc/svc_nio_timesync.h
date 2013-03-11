#ifndef _SVC_TIMESYNC_H_8958_
#define _SVC_TIMESYNC_H_8958_

/*******************************************************************************
 * @author Shi Zhirong on 2012.08.02
 * 	- revision today.
 ******************************************************************************/

 
#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_time.h"
#include "svc_nio_mac.h"
#include "svc_nio_datatree.h"
#include "svc_nio_tpsn.h"

#define TSYNC_PROTOCOL_IDENTIFER TPSN_PROTOCOL_IDENTIFER
#define TSYNC_MAX_FRAME_SIZE MAX_IEEE802FRAME154_SIZE
#define	TSYNC_TASK_TIME 2		   

#pragma pack(1)
typedef struct{	
	TiDataTreeNetwork *	dtp;
	TiNioTPSN *			tpsn;
	TiFrame *  			txbuf;	
	TiNodeBase * 		nbase;
	char txbuf_mem[ FRAME_HOPESIZE(TSYNC_MAX_FRAME_SIZE) ];
	uint8 state;
}TiNioTimesync;

TiNioTimesync * timesync_construct( char * buf,uintx size);
void timesync_destroy(TiNioTimesync * tsync);
TiNioTimesync * timesync_open( 	TiNioTimesync * tsync, TiDataTreeNetwork * dtp, TiNioTPSN * tpsn,TiNodeBase * nbase);
void timesync_task(void * object, TiEvent * e );


#endif /* _SVC_TIMESYNC_H_8958_ */
