#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_rtc.h"
#include "../../rtl/rtl_frame.h"
#include "hal_mcu.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "hal_timesynchro.h"

TiTimsynchroAdapter m_syn;

TiTimsynchroAdapter * hal_syn_construct(char * buf, uint8 size )
{
    hal_assert( sizeof(TiTimsynchroAdapter) <= size );
    memset( buf, 0x00, size );
    return (TiTimsynchroAdapter*)buf;
} 

TiTimsynchroAdapter * hal_syn_open( TiTimsynchroAdapter * syn, TiRtcAdapter *rtc)
{
    syn->rtc = rtc;

    return syn;
}


void hal_syn_rxhandler( TiFrame * rxbuf)
{
    char *ptr;
    uint32 sendtime;
    uint32 recvtime;
    uint8 legth;
    ptr = frame_startptr( rxbuf);
    //前面12个字节是MAC头，不能用movehigher()使指针指向里层，也许是从2520直接收到的缘故。
    if ( ptr[12]==0x04)//0x04->time synchro protocal id.
    {
        recvtime = m_syn.rtc->currenttime;//recvtime = RTC_GetCounter();
        sendtime = HTSY_EMAKEDOUBLEWORD( ptr[17],ptr[16],ptr[15],ptr[14]);
        m_syn.rtc->currenttime = sendtime;//RTC_SetCounter( sendtime);
        ptr[18] = (uint8)(recvtime);
        ptr[19] = ( uint8)(recvtime>>8);
        ptr[20] = (uint8)(recvtime>>16);
        ptr[21] = (uint8)(recvtime>>24);
    }
}

void hal_syn_txhandler( TiFrame * txbuf)
{
    char *ptr;
    uint32 sendtime;
    uint32 recvtime;
    uint8 legth;
    legth = frame_length( txbuf);
    frame_movehigher(txbuf);
    ptr = frame_startptr( txbuf);
    if ( ptr[0]==0x04)//0x04->time synchro protocal id.
    {
        sendtime = m_syn.rtc->currenttime;//sendtime = RTC_GetCounter();
        ptr[2] = (uint8)(sendtime);
        ptr[3] = ( uint8)(sendtime>>8);
        ptr[4] = (uint8)(sendtime>>16);
        ptr[5] = (uint8)(sendtime>>24);
    }
    frame_movelower( txbuf);
    frame_setlength( txbuf,legth);
}