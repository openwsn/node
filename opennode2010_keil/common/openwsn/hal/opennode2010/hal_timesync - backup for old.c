#include "../hal_configall.h"
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_rtc.h"
#include "../../rtl/rtl_frame.h"
#include "../../rtl/rtl_time.h"
#include "../hal_mcu.h"
#include "../hal_uart.h"
#include "../hal_led.h"
#include "../hal_timesync.h"

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
    
    ptr = frame_startptr( input );

	// @attention
	// Since the frame is just received from the wireless transceiver adapter, there's
	// only one layer in the frame. So you cannot use frame_movehigher() to switch 
	// to the higher layer. The higher layer is actually not existed yet! So the only 
	// method is to parse the frame format and find the start of the frame payload
	// absolutely. That's why we add 12 here. 12 is just the header size. 
	//
	// 12 B = 1B Length + 2B Frame Control + 1B Sequence Number + 2B Destination PAN
	//		+ 2B Destination Address + 2B Source PAN + 2B Source Address
	//
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
