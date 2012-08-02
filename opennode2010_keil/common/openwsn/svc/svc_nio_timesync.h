/*******************************************************************************
 * @author Shi Zhirong on 2012.08.02
 * 	- revision today.
 ******************************************************************************/
 
#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_time.h"
#include "svc_nio_aloha.h"

#define CONFIG_NIOSYNC_IDENTIFIER 4

#define TSYNC_MAKE_TIMWE(highest,high,lowe,lowest) (((uint32)highest<<24) | ((uint32)high<<16)|((uint16)lowe<<8)|((uint8)lowest))

#define TSYNC_MASTER 1
#define TSYNC_SLAVE 2

#define TSYNC_STATE_IDLE 1
#define TSYNC_STATE_RECV 2
#define TSYNC_STATE_SEND 3

#define RECV_TIME  10000
#define TSYN_MAX_FRAME_SIZE	128

#define TSYNC_FRAME_1ST 0
#define TSYNC_FRAME_2ND 1

#pragma pack(1)
typedef struct
{
	TiAloha * mac;
    TiNioAcceptor *	nac;
	TiRtcAdapter * rtc;
    TiTimeSyncAdapter * hsyn;
	TiTimerAdapter * timer;
	TiFrame *  rxbuf;	
	char rxbuf_mem[ FRAME_HOPESIZE(TSYN_MAX_FRAME_SIZE) ];
	uint8 id;
	uint8 state;
}TiNioTimeSync;

TiNioTimeSync *timesync_construct( char * buf,uintx size);
void timesync_destroy(TiNioTimeSync * sync);
TiNioTimeSync * timesync_open( 	TiNioTimeSync * sync,TiAloha *mac,	TiRtcAdapter * rtc,TiTimeSyncAdapter *hsyn ,TiTimerAdapter *timer);
TiNioTimeSync * timesync_sendhandler( TiNioTimeSync * sync, TiFrame *frame );
TiNioTimeSync * timesync_recvhandler( TiNioTimeSync * sync, TiFrame *input );
TiNioTimeSync * timesync_evolve( TiNioTimeSync * sync );

//old version
// #include "svc_configall.h"
// #include "svc_foundation.h"
// #include "../rtl/rtl_time.h"
// #include "../osx/osx_timer.h"
// #include "svc_nio_aloha.h"
// #define CONFIG_NIOSYNC_IDENTIFIER 0x67
// #define NIOSYNC_CMD_REQUEST 1
// #define NIOSYNC_CMD_RESPONSE 0
// #define SYN_MAX_FRAME_SIZE 128

// /**
 // * This module implements the time synchronization service. This service usually 
 // * resident in the system and perform time synchronize operations periodically. 
 // * 
 // * In order to improve the timing precision, this service uses the high percision
 // * TIMER hardware to measure the duration. The hardware related source code is inside
 // * module "hal_timesync".
 // */
 
// /*
 // * period: 控制多长时间执行一次timesync操作
 // * systimer: 保存了当前的高精度时间值，这个值也是不同node间的同步目标
 // *
 // * scale_factor: a
 // * shift_factor: b
 // * 		y = x * a + b
 // */ 

// #pragma pack(1) 
// typedef struct{
  // // TiDTP * net;
  // TiOsxTimer * systimer;
  // TiAloha * mac;
  // uint32 period;
  // uint16 scale_factor;//这两个参数的值不知道该怎么定
  // uint16 shift_factor;//这两个参数的值不知道该怎么定
  // uint16 seqid;
  // TiFrame * txbuf;
  // char   txbuf_mem[ FRAME_HOPESIZE(SYN_MAX_FRAME_SIZE) ];

// }TiNioTimeSync;


// /** Construct the time-sync service component on specified memory block */
// //timesync_construct
// //timesync_destroy
// TiNioTimeSync *timesync_construct( char * buf,uintx size);//todo
// void timesync_destroy(TiNioTimeSync * sync);//todo
// /** Initialize the time-sync service component for further operation */
// //timesync_open( perod, protocol_id )
// //timesync_close
// TiNioTimeSync * timesync_open( TiNioTimeSync * sync, TiOsxTimer * timer,TiAloha * mac,uint16 period );//todo
// void  timesync_close( TiNioTimeSync * sync );
// /** Start the time sync service */
// //timesync_start
// //timesync_stop
// //timesync_initiate
// uintx timesync_start( TiNioTimeSync * sync);//todo
// void timesync_stop( TiNioTimeSync * sync);//todo
// void timesync_initiate(  TiNioTimeSync * sync,TiAloha *mac,TiFrame * rxbuf);//todo initialize the time
// //timesync_evolve
// void timesync_evolve( void * macptr, TiEvent * e );//todo
// TiSystemTime timesync_current( TiNioTimeSync * sync);//todo 
// void timesync_set( TiNioTimeSync * sync,TiSystemTime ms);
// TiSystemTime timesync_get( TiFrame * frame);
// void timesync_forward( TiNioTimeSync * sync,TiSystemTime step );// 向前拨快
// void timesync_backward( TiNioTimeSync * sync,TiSystemTime step );// 向后调慢

// //timesync_adjust = forward + backward
