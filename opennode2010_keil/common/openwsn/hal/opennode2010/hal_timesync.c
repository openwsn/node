#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_rtc.h"
#include "../../rtl/rtl_frame.h"
#include "../../rtl/rtl_time.h"
#include "hal_mcu.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "hal_timesync.h"

#define TSYNC_MAKE_DWORD(highest,high,lowe,lowest) (((uint32)highest<<24) | ((uint32)high<<16)|((uint16)lowe<<8)|((uint8)lowest))

TiTimeSyncAdapter * hal_tsync_open( TiTimeSyncAdapter * tsync, TiRtcAdapter *rtc)
{
    memset( (void *)tsync, 0x00, sizeof(TiTimeSyncAdapter) );
    tsync->rtc = rtc;
    return tsync;
}

intx hal_tsync_close(TiTimeSyncAdapter * tsync )
{
    tsync = tsync;
    return 0;
} 

intx hal_tsync_rxhandler(TiTimeSyncAdapter * tsync, TiFrame * input, TiFrame * output, uint8 option )
{
    char *ptr;
    //TiTime sendtime, recvtime;
    uint32 sendtime;
    uint32 recvtime;
    uint8 legth;
    
    ptr = frame_startptr( input );
    
    //前面12个字节是MAC头，不能用movehigher()使指针指向里层，也许是从2520直接收到的缘故。
    if (ptr[12] == TSYNC_PROTOCAL_ID)
    {
        // recommend rtc_curtime(tsync->rtc)
        recvtime = tsync->rtc->currenttime;//recvtime = RTC_GetCounter();
        sendtime = TSYNC_MAKE_DWORD( ptr[17], ptr[16],ptr[15],ptr[14]);
        tsync->rtc->currenttime = sendtime;//RTC_SetCounter( sendtime);
        
        // rtc_curtime(tsync->rtc, &sendtime);
        //time_read32( systimer, ptr+18 );
        
        ptr[18] = (uint8)(recvtime);
        ptr[19] = (uint8)(recvtime >> 8);
        ptr[20] = (uint8)(recvtime >> 16);
        ptr[21] = (uint8)(recvtime >> 24);
    }

    hal_assert( input == output );
    return frame_length(input);
}

intx hal_tsync_txhandler(TiTimeSyncAdapter * tsync, TiFrame * input, TiFrame * output, uint8 option )
{
    char *ptr;
    
    uint32 sendtime;

    ptr = frame_startptr( input);
    
    if (ptr[0] == TSYNC_PROTOCAL_ID)
    {
        // recommend rtc_curtime(tsync->rtc)
        sendtime = tsync->rtc->currenttime;//sendtime = RTC_GetCounter();
        ptr[2] = (uint8)(sendtime);
        ptr[3] = (uint8)(sendtime>>8);
        ptr[4] = (uint8)(sendtime>>16);
        ptr[5] = (uint8)(sendtime>>24);
    }
    
    hal_assert( input == output );
    return frame_length(input);
}
