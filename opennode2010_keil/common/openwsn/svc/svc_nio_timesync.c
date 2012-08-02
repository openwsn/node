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

TiNioTimeSync *timesync_construct( char * buf,uintx size)//todo
{
	memset(buf,0x00,size);
	hal_assert( sizeof(TiNioTimeSync)<=size);
	return ( TiNioTimeSync *)buf;
}

void timesync_destroy(TiNioTimeSync * sync)//todo
{
	return;
}

TiNioTimeSync * timesync_open( 	TiNioTimeSync * sync,TiAloha *mac,	TiRtcAdapter * rtc,TiTimeSyncAdapter *hsyn ,TiTimerAdapter *timer)//todo
{
	sync->mac = mac;
    sync->rtc = rtc;
	sync->hsyn = hsyn;
	sync->timer = timer;
	nac_set_timesync_adapter(mac->nac,hsyn);
	sync->rxbuf = frame_open( (char *)( &sync->rxbuf_mem), FRAME_HOPESIZE(SYN_MAX_FRAME_SIZE), 3, 20,25);
	sync->state=TSYNC_STATE_IDLE;

	return sync;
}

TiNioTimeSync * timesync_sendhandler( TiNioTimeSync * sync, TiFrame *frame )
{
	char *pc;
	char i;
	sync->state=TSYNC_STATE_IDLE;
	frame_reset(frame, 3, 20, 25);
	pc = frame_startptr( frame );
	pc[0] = CONFIG_NIOSYNC_IDENTIFIER;
	pc[1] = TSYNC_FRAME_1ST;
	pc[2] = sync->mac->shortaddrfrom;//CONFIG_ALOHA_LOCAL_ADDRESS;
	for (i=3; i<20; i++)
	{
		pc[i] = 0;
	}
	frame_setlength( frame,20 );
	if (aloha_send( sync->mac,0xffff, frame, 0x00) > 0)
	{
		led_toggle(LED_RED);
		sync->state=TSYNC_STATE_RECV;		
	
		timer_setinterval( sync->timer, RECV_TIME, 0 );
        timer_start( sync->timer );
	}
}

TiNioTimeSync * timesync_recvhandler( TiNioTimeSync * sync, TiFrame *input )
{
 	char * pc;
	char i;
	char count;
	uint16 addr;
	uint32 t1,t2,t3,t4,tdelta;
	pc = frame_startptr( input);
	if(pc[0]==CONFIG_NIOSYNC_IDENTIFIER)
	{
		switch(sync->state)
		{
			case TSYNC_STATE_RECV:
				if(pc[1]==TSYNC_FRAME_2ND)
				{	
					sync->state=TSYNC_STATE_IDLE;
					timer_stop(sync->timer);

					t1=TSYNC_MAKE_TIMWE( pc[15], pc[14],pc[13],pc[12]);
					t2=TSYNC_MAKE_TIMWE( pc[19], pc[18],pc[17],pc[16]);
					t3=TSYNC_MAKE_TIMWE( pc[7], pc[6],pc[5],pc[4]);
					t4=TSYNC_MAKE_TIMWE( pc[11], pc[10],pc[9],pc[8]);

					tdelta=((t2-t1)-(t4-t3))/2;
					RTC_SetCounter(RTC_GetCounter()+tdelta);

				}
				break;
			case TSYNC_STATE_IDLE:
				if(pc[1]==TSYNC_FRAME_1ST)
				{
					pc=frame_startptr(input);
					for(i=4;i<12;i++)
					{
						pc[i+8]=pc[i];
						pc[i]=0;
					}
					pc[1] = TSYNC_FRAME_2ND;
					addr=(uint16)pc[2];
					pc[3]=sync->mac->shortaddrfrom;//CONFIG_ALOHA_LOCAL_ADDRESS;
					if (aloha_send(sync->mac , addr, input, 0x00) > 0)	  //0x01
					{
						sync->state=TSYNC_STATE_IDLE;
					}
					else
					{
						count = frame_totalcopyfrom( sync->rxbuf, input ); 
						nac_evolve( sync->mac->nac, NULL);
						sync->state=TSYNC_STATE_SEND;
					}
				}
				break;
			default:
				break;
		}
	}
}
TiNioTimeSync * timesync_evolve( TiNioTimeSync * sync )
{
	char *pc;
	uint16 addr;
	switch(sync->state)
	{
		case TSYNC_STATE_IDLE:
			break;
		case TSYNC_STATE_RECV:
			if(timer_expired(sync->timer))
			{
				sync->state=TSYNC_STATE_IDLE;
			}
			break;
		case TSYNC_STATE_SEND:
			if (aloha_send(sync->mac , addr, sync->rxbuf, 0x00) > 0)//0x01
			{
				pc=frame_startptr(sync->rxbuf);
				addr=(uint16)pc[2];
				sync->state=TSYNC_STATE_IDLE;
				frame_totalclear( sync->rxbuf );

			}
			else
			{
				nac_evolve( sync->mac->nac, NULL);
			}
	}
}

//old version
// #include "svc_configall.h"
// #include "svc_foundation.h"
// #include "../rtl/rtl_time.h"
// #include "../rtl/rtl_frame.h"
// #include "../osx/osx_timer.h"
// #include "svc_nio_aloha.h"
// #include "svc_nio_timesync.h"

// TiNioTimeSync *timesync_construct( char * buf,uintx size)//todo
// {
	// memset(buf,0x00,size);
	// hal_assert( sizeof(TiNioTimeSync)<=size);
	// return ( TiNioTimeSync *)buf;
// }

// void timesync_destroy(TiNioTimeSync * sync)//todo
// {
	// return;
// }



// TiNioTimeSync * timesync_open( TiNioTimeSync * sync, TiOsxTimer * timer,TiAloha * mac,uint16 period)//todo
// {
	// sync->systimer = timer;
	// sync->period = period;
    // sync->seqid =0;
	// //不知道以下两个变量应该赋什么值？
	// //下载只是简单地把其他节点广播过来的时间赋给本地时钟
	// /*
	// sync->scale_factor = 
	// sync->shift_factor = 
	// */
	// sync->mac = mac;
	// sync->txbuf = frame_open( (char *)( &sync->txbuf_mem), FRAME_HOPESIZE(SYN_MAX_FRAME_SIZE), 3, 20, 0);

	// return sync;
// }

// //发起时间同步，对外广播时间同步帧
// uintx timesync_start( TiNioTimeSync * sync)//todo
// {
	// TiSystemTime time;
	// TiSystemTime temp;
	// char * ptr;
	// int i;
	// dbc_putchar( 0xfe);//todo for testing
	// if ( frame_empty( sync->txbuf))
	// {
		// dbc_putchar( 0xfd);//todo for testing
		// frame_reset( sync->txbuf,3,20,0);
		// time = timesync_current(  sync);
		// ptr = frame_startptr( sync->txbuf);
		// ptr[0] = CONFIG_NIOSYNC_IDENTIFIER;
		// ptr[1] = NIOSYNC_CMD_REQUEST;
		// ptr[2] = sync->seqid;
		// for ( i=0;i<8;i++)
		// {
			// temp = time;
			// ptr[3+i] = ( uint8)( temp>> (8*i));
		// }
		// frame_setlength( sync->txbuf,13);
	// }
	 // timesync_evolve( sync, NULL );
	 // return frame_length( sync->txbuf);

// }
// //???????????????下面两个函数有用吗？
// void  timesync_close( TiNioTimeSync * sync )
// {
	
// }


// void timesync_stop( TiNioTimeSync * sync)//todo
// {
	
// }

// //返回本地时间
// TiSystemTime timesync_current( TiNioTimeSync * sync)//todo 
// {
	// return sync->systimer->time;
// }

// void timesync_set( TiNioTimeSync * sync,TiSystemTime ms)
// {
	// sync->systimer->time = ms;
// }
// //从时间同步帧中获得同步时间
// //ptr[0] = identifier;
// //ptr[1] = cmd_type;
// //ptr[2] = seqid;

// //ptr[3] ~ptr[10] = time;低位在前高位在后
// TiSystemTime timesync_get( TiFrame * frame)
// {
	// TiSystemTime time;
	// char * ptr;
	// ptr = frame_startptr( frame);
	// time = (uint64)( ptr[3])|((uint64)( ptr[4])<< 8)|((uint64)( ptr[5])<< 16)|((uint64)( ptr[6])<< 24)|((uint64)( ptr[7])<< 32)|
		// ((uint64)( ptr[8])<< 40)|((uint64)( ptr[9])<< 48)|((uint64)( ptr[10])<< 56);
	// return time;
// }
// void timesync_forward( TiNioTimeSync * sync,TiSystemTime step )// 向前拨快
// {
	// clock_forward(sync->systimer->time,step);
// }

// void timesync_backward( TiNioTimeSync * sync,TiSystemTime step )// 向后调慢
// {
	// clock_backward( sync->systimer->time,step);
// }


// void timesync_initiate(  TiNioTimeSync * sync,TiAloha *mac,TiFrame * rxbuf)//todo initialize the time
// {
	// TiSystemTime time;
	// uint16 i;
	// uint8 count = 0;
	// char * ptr;
	// i = 0;
	// //接受外来的时间同步请求帧，改变本地时间
	// while ( i<0x1ff)
	// {
		// frame_reset( rxbuf,3,20,0);
		// count = aloha_recv( mac,rxbuf,0x00);
		// i++;
        // if ( count)
        // {
			// ptr = frame_startptr( rxbuf);
			// //现在只是简单的时间同步，把收到的时间直接赋给本地时钟。
			// if ( ptr[0]==CONFIG_NIOSYNC_IDENTIFIER&&ptr[1]==NIOSYNC_CMD_REQUEST&&ptr[2]> sync->seqid)
			// {
				// time = timesync_get( rxbuf);
                // timesync_set( sync,time);
				// sync->seqid = ptr[2];
				// break;
			// } 
			// else
			// {
				// count = 0;
			// }
        // }
	// }
// //继续对外广播时间同步请求帧，连续广播三次
	// if ( count)
	// {
		// for ( i=0;i<3;i++)
		// {
			// timesync_start( sync);
		// }
	// }
// }


// void timesync_evolve( void * macptr, TiEvent * e )//todo
// {
	// TiNioTimeSync * sync = (TiNioTimeSync *)macptr;
	// uintx count = 0;
	// dbc_putchar( 0xfc);//todo for testing
	// if ( !frame_empty( sync->txbuf))
	// {
		// dbc_putchar( 0xfb);//todo for testing
        // count = aloha_broadcast( sync->mac,sync->txbuf,0x00);
		// if ( count)
		// {
			// dbc_putchar( 0xfa);//todo for testing
			// frame_totalclear( sync->txbuf);
		// }
	// }
// }


