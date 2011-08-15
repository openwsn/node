#ifndef _HAL_TIMESYNC_H_5678_
#define _HAL_TIMESYNC_H_5678_

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_rtc.h"
#include "../rtl/rtl_frame.h"

#define TSYNC_PROTOCAL_ID 0x04 

typedef struct{
    TiRtcAdapter *rtc;              
}TiTimeSyncAdapter;

TiTimeSyncAdapter * hal_tsync_open( TiTimeSyncAdapter * tsync, TiRtcAdapter *rtc);
intx hal_tsync_close(TiTimeSyncAdapter * tsync );
intx hal_tsync_rxhandler(TiTimeSyncAdapter * tsync, TiFrame * input, TiFrame * output, uint8 option );
intx hal_tsync_txhandler(TiTimeSyncAdapter * tsync, TiFrame * input, TiFrame * output, uint8 option );

#endif
