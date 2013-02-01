/*******************************************************************************
 * @author Shi Zhirong on 2012.08.02
 * 	- revision today.
 ******************************************************************************/

#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_time.h"
#include "../rtl/rtl_frame.h"
#include "svc_nio_aloha.h"
#include "svc_nio_timesync.h"
#include "svc_nio_tpsn.h"

TiNioTimesync * timesync_construct( char * buf,uintx size)
{
	memset(buf,0x00,size);
	hal_assert( sizeof(TiNioTimesync)<=size);
	return ( TiNioTimesync *)buf;
}

void timesync_destroy(TiNioTimesync * tsync)
{
	return;
}

TiNioTimesync * timesync_open( 	TiNioTimesync * tsync, TiDataTreeNetwork * dtp, TiNioTPSN * tpsn,TiNodeBase * nbase)
{
	tsync->dtp=dtp;
	tsync->tpsn=tpsn;
	tsync->nbase=nbase;
	tsync->txbuf = frame_open( (char *)( &tsync->txbuf_mem), FRAME_HOPESIZE(TSYNC_MAX_FRAME_SIZE), 3, 25,25);//20
	return tsync;
}

void timesync_task(void * object, TiEvent * e )//DTP sink node doesn't need it
{
	TiNioTimesync * tsync=(TiNioTimesync *)object;
	tpsn_send( tsync->tpsn, tsync->dtp->parent,tsync->txbuf );
	osx_taskspawn( timesync_task, tsync, TSYNC_TASK_TIME, 0, 0);
}


