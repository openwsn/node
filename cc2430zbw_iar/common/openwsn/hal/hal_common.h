#ifndef _HAL_COMMON_H_6578_
#define _HAL_COMMON_H_6578_

#include "hal_configall.h"
#include "hal_foundation.h"
#include "../rtl/rtl_time.h"

//typedef (TiTimerAdapter *)(* TiFunTimerOpen)(void * timer, uint8 id, TiFunEventHandler listener, void * object, uint8 option );
//typedef void(* TiFunTimerClose(void * timer);
typedef void(* TiFunTimerSetListener)( void * timer, TiFunEventHandler listener, void * object );
typedef void(* TiFunTimerSetScale)( void * timer, uint16 scale );
typedef void(* TiFunTimerSetInterval)( void * timer, uint16 interval, uint8 repeat );
typedef void(* TiFunTimerStart)( void * timer );
typedef void(* TiFunTimerReStart)( void * timer );
typedef void(* TiFunTimerStop)( void * timer );
// typedef void(* TiFunTimerEnable)( void * timer );
// typedef void(* TiFunTimerDisable)( void * timer );
typedef void(* TiFunTimerSetValue)( void * timer, TiTime value);
typedef TiTime(* TiFunTimerGetValue)( void * timer );
typedef TiTime(* TiFunTimerElapsed)( void * timer );
typedef void(* TiFunTimerForward)(void * timer, uint16 steps);
typedef void(* TiFunTimerBackward)(void * timer, uint16 steps);
typedef bool(* TiTimerExpired)( void * timer );
typedef TiTime(* TiFunClockPerMilliSecond)( void * timer ); //这个是干什么用的？  

typedef struct{
    void * provider;
    TiFunTimerSetListener setlistener;
    TiFunTimerSetScale setscale;
    TiFunTimerSetInterval setinterval;
    TiFunTimerStart start;
    TiFunTimerStop stop;
    TiTimerExpired expired;
    TiFunClockPerMilliSecond clockpermsec;
}TiBasicTimerInterface;

typedef struct{
    void * provider;
    TiFunTimerSetListener setlistener;
    TiFunTimerSetScale setscale;
    TiFunTimerSetInterval setinterval;
    TiFunTimerStart start;
    TiFunTimerReStart restart;
    TiFunTimerStop stop;
    TiFunTimerSetValue setvalue;
    TiFunTimerGetValue getvalue;
    TiFunTimerForward forward;
    TiFunTimerBackward backward;
    TiFunTimerElapsed elapsed;
    TiTimerExpired expired;
    TiFunClockPerMilliSecond clockpermsec;
}TiLightTimerInterface;

typedef struct{
    // snapshot
    // capture
    // channel
    uint8 timer;//todo for testing
}TiPowerTimerInterface;

#endif
