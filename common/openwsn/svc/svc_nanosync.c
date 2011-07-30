
#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"
#include "../rtl/rtl_debugio.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_interrupt.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_rtc.h"
#include "../hal/hal_led.h"
#include "../hal/hal_frame_transceiver.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../svc/svc_nanosync.h"



TiNanoSync * nanosync_construct( char * buf,uintx size)
{
	memset(buf,0x00,size);
	hal_assert( sizeof(TiNanoSync)<=size);
	return ( TiNanoSync *)buf;
}

void nanosync_destroy( TiNanoSync *sync)
{
	return;
}

/**
 * open the TiNanoSync component for time sync operation
 *
 * @attention
 * - the rxtx component should be ready for sending and receving. the TiNanoSync 
 *  component assumes the rxtx has already opened and initialized successfully.
 */
TiNanoSync * nanosync_open( TiNanoSync * sync, uint8 mode, TiRtc * rtc, TiFrameTxRxInterface * rxtx, 
	uint8 chn, uint16 panid, uint16 address, char * buf,uint16 period, uint8 option )

{
	rtl_assert((rxtx != NULL) );
	sync->mode = mode;
	sync->rtc = rtc;
    sync->rxtx = rxtx;
	sync->panto = panid;
	sync->shortaddrto = FRAME154_BROADCAST_ADDRESS;
	sync->panfrom = panid;
	sync->shortaddrfrom = address;
	sync->seqid = 0;
	sync->option = option;

	ieee802frame154_open( &(sync->desc) );

	return sync;
}




uintx _nanosync_broadcast( TiNanoSync * sync, uint8 option )//这一句函数不知道能不能实现？
{    
	
	TiIEEE802Frame154Descriptor * desc;
	uintx ret;
    char * payload;
	uintx count=0;
	uint16 time;
	uint8 time_1,time_2;

	ieee802frame154_open( &(sync->desc) );
    
    

	desc = ieee802frame154_format( &(sync->desc), &(sync->txbuf[0]), sizeof(sync->txbuf), FRAME154_DEF_FRAMECONTROL_DATA );
	rtl_assert( desc != NULL );

	

	ieee802frame154_set_sequence( desc, sync->seqid );
	ieee802frame154_set_panfrom( desc, sync->panfrom );
	ieee802frame154_set_shortaddrfrom( desc, sync->shortaddrfrom );
	ieee802frame154_set_panto( desc, FRAME154_BROADCAST_PAN );
	ieee802frame154_set_shortaddrto( desc, FRAME154_BROADCAST_ADDRESS );

    payload = ieee802frame154_msdu( desc );
	
    payload[0] = CONFIG_NANOSYNC_IDENTIFIER;
    payload[1] = sync->seqid;
    payload[2] = NANOSYNC_CMD_RESPONSE;
    // @todo
	time = sync->rtc->curtime.year;
	time_2 = (uint8)(time);//low byte
	time_1 = (uint8)(time >> 8);//high byte
	payload[3] = time_1;
	payload[4] = time_2;
	payload[5] = sync->rtc->curtime.month;
	payload[6] = sync->rtc->curtime.day;
	payload[7] = sync->rtc->curtime.hour;
	payload[8] = sync->rtc->curtime.min;
	payload[9] = sync->rtc->curtime.sec;
	/*payload[5] = 0xd1;//测试用的
    payload[6] = 0xd2;//测试用的
	payload[7] = 0xd3;//测试用的
	payload[8] = 0xd4;//测试用的
	payload[9] = 0xd5;//测试用的*/
	time = sync->rtc->curtime.msec;
	time_2 = (uint8)(time);//low byte
	time_1 = (uint8)(time >> 8);//high byte
	payload[10] = time_1;
	payload[11] = time_2;
   /* payload[10] = 0xd6;//测试用的
	payload[11] = 0xd7;//测试用的*/
    

    // the last parameter 0x00 indicate this send doesn't require ACK. it's actually broadcast 
	count = sync->rxtx->send( sync->rxtx->provider, sync->txbuf, sizeof(sync->txbuf), 0x00 );
	if (count > 0)
	{
		sync->seqid ++;
	}

	

	return count;	
}

uintx       nanosync_recv( TiNanoSync * sync, char *buf,uint8 len,uint8 option )//uintx       nanosync_recv( TiNanoSync * sync, char *buf,uint8 option )
{
	uintx count = 0;
	
	count = sync->rxtx->recv( sync->rxtx->provider,buf,len ,option );//count = sync->rxtx->recv( sync->rxtx->provider,buf,sizeof(buf) ,option );
    return count;
}

uintx nanosync_master_evolve( TiNanoSync * sync, uint8 option )
{
   _nanosync_broadcast( sync, option);
}



uintx nanosync_slave_evolve( TiNanoSync * sync, char * rxbuf, uint8 len,uint8 option )
{   
   
    nanosync_update( sync, rxbuf, len );//有问题这一句
    
    _nanosync_broadcast( sync,option );//当执行update函数后，再执行到这一句的时候发生assert错误。
	
}


void  nanosync_update( TiNanoSync * sync, char * rxbuf, uint8 len )
{
    TiRtc * rtc;
	//@todo
	TiCalTime  time;
	
    hal_atomic_begin();
	
	time.year = FRAME154_MAKEWORD(rxbuf[15], rxbuf[16]);
	time.month = rxbuf[17];
	time.day = rxbuf[18];
	time.hour = rxbuf[19];
	time.min = rxbuf[20];
	time.sec = rxbuf[21];
	time.msec = FRAME154_MAKEWORD(rxbuf[22],rxbuf[23]);

	rtc = sync->rtc;
	rtc->curtime.year = ( rtc->curtime.year + time.year)/2;
	rtc->curtime.month = ( rtc->curtime.month + time.month)/2;
	rtc->curtime.day = ( rtc->curtime.day + time.day)/2;
	rtc->curtime.hour = ( rtc->curtime.hour + time.hour)/2;
	rtc->curtime.min = ( rtc->curtime.min + time.min)/2;
	rtc->curtime.sec = ( rtc->curtime.sec + time.sec)/2;
	rtc->curtime.msec = ( rtc->curtime.msec + time.msec)/2;


	hal_atomic_end();
	
}

