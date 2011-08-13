#ifndef _HAL_TIMESYNC_H_5678_
#define _HAL_TIMESYNC_H_5678_

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_rtc.h"
#include "../../rtl/rtl_frame.h"


#define HTSY_EMAKEDOUBLEWORD(highest,high,lowe,lowest) (((uint32)highest<<24) | ((uint32)high<<16)|((uint16)lowe<<8)|((uint8)lowest))

typedef struct{
    TiRtcAdapter *rtc;              
}TiTimsynchroAdapter;

extern TiTimsynchroAdapter m_syn;

TiTimsynchroAdapter * hal_syn_construct(char * buf, uint8 size );

TiTimsynchroAdapter * hal_syn_open( TiTimsynchroAdapter * syn, TiRtcAdapter *rtc);

void hal_syn_rxhandler( TiFrame * rxbuf);

void hal_syn_txhandler( TiFrame * txbuf);

#endif
