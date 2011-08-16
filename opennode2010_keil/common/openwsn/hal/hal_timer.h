#ifndef _HAL_TIMERADAPTER_H_6828_
#define _HAL_TIMERADAPTER_H_6828_

//#include "cm3/core/core_cm3.h"
#include "hal_mcu.h"
#include "hal_configall.h"
#include "hal_foundation.h"
 
#define TIMER_STATE_ENABLED 	0x01
#define TIMER_STATE_PERIOD	 	0x02
#define TIMER_STATE_INTERRUPT 	0x04
#define TIMER_STATE_CAPTURE 	0x08

#pragma pack(1) 
typedef struct{
  uint8 id;
  uint8 subpriority;
  uint8 state;
  uint16 interval;
  uint16 count;
  uint16 prescale_factor;
  uint8 repeat;
  TiFunEventHandler listener;
  void * listenowner;//object 
  uint8 priority;
  uint8 option;//1->使能定时中断，0->不使能
}TiTimerAdapter;

#ifdef __cplusplus
extern "C" {   
#endif


TiTimerAdapter* timer_construct( char * buf, uint8 size );


void timer_destroy( TiTimerAdapter * timer );

//若以查询方式判断中断时间是否到，则option必须为0.尤其作为aloha等的时钟使用时更要注意！
TiTimerAdapter* timer_open( TiTimerAdapter * timer, uint8 id, TiFunEventHandler listener, void * object, uint8 option );


void timer_close( TiTimerAdapter * timer );


void timer_setinterval( TiTimerAdapter * timer, uint16 interval,uint16 prescaler );


void timer_start( TiTimerAdapter * timer );


void timer_restart( TiTimerAdapter * timer, uint16 interval, uint16 prescale);

void timer_stop( TiTimerAdapter * timer );

void timer_setlistener( TiTimerAdapter * timer, TiFunEventHandler listener, void * object );

void timer_setprior( TiTimerAdapter * timer, uint8 preprior,uint8 subprior );

uint8 timer_expired( TiTimerAdapter *timer);//1 time interrupt occur,0 not occur.

void timer_CLR_IT( TiTimerAdapter *timer);//clear the interrupt bit


#ifdef __cplusplus
}
#endif

#endif /* _HAL_TIMERADAPTER_H_6828_ */

