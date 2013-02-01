/*******************************************************************************
 * @author Shi Zhirong on 2012.08.02
 * 	- revision today.
 ******************************************************************************/

#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_time.h"
#include "../rtl/rtl_frame.h"
#include "svc_nio_tpsn.h"

TiNioTPSN * tpsn_construct( char * buf,uintx size)
{
	memset(buf,0x00,size);
	hal_assert( sizeof(TiNioTPSN)<=size);
	return ( TiNioTPSN *)buf;
}

void tpsn_destroy(TiNioTPSN * tpsn)
{
	return;
}

TiNioTPSN * tpsn_open( 	TiNioTPSN * tpsn,TiRtcAdapter * rtc,TiNioNetLayerDispatcher *dispatcher,TiTimeSyncAdapter *hsyn)
{
    tpsn->rtc = rtc;
	tpsn->dispatcher = dispatcher;
	tpsn->hsyn = hsyn;

	nac_set_timesync_adapter(dispatcher->mac->nac,hsyn);

	return tpsn;
}

TiNioTPSN * tpsn_send( TiNioTPSN * tpsn, uint16 addr, TiFrame *frame )
{
	char * pkt;
	char i;
	uint8 len,count;
	frame_reset(frame, 3, 25, 25);//20
	pkt = frame_startptr( frame );
	
	TPSN_SET_PROTOCAL_IDENTIFIER(pkt,TPSN_PROTOCOL_IDENTIFER);
	TPSN_SET_TYPE(pkt,TPSN_FRAME_REQUEST);
	TPSN_SET_SHORTADDRFROM(pkt,tpsn->dispatcher->mac->shortaddrfrom);
	TPSN_SET_SHORTADDRTO(pkt,addr);

	for (i=6; i<25; i++)//20
	{
		pkt[i] = 0;
	}
	frame_setlength( frame,25 );//20
	
	count = 0;
    while (count < 0x04)
    {   
		len = mac_send( tpsn->dispatcher->mac,addr, frame, 0x00);
		if (len > 0)
		{  
			//led_toggle(LED_RED);
			break;
		}
		count ++;
	}
}

intx nio_tpsn_rxhandler( void * object, TiFrame * input, TiFrame * output, uint8 option )
{
	uint32 t1,t2,t3,t4,tdelta;
	intx i;
	uint16 addr;
	TiNioTPSN * tpsn = (TiNioTPSN *)object;
    char * pkt;
	uint8 legth;
	pkt=frame_startptr(input);
	legth = frame_length( input );
	
	switch (TPSN_TYPE(pkt))
	{
	case TPSN_FRAME_RESPONSE:	
		t1=TPSN_MAKE_TIME( pkt[17], pkt[16],pkt[15],pkt[14]);
		t2=TPSN_MAKE_TIME( pkt[21], pkt[20],pkt[19],pkt[18]);
		t3=TPSN_MAKE_TIME( pkt[9], pkt[8],pkt[7],pkt[6]);
		t4=TPSN_MAKE_TIME( pkt[13], pkt[12],pkt[11],pkt[10]);

		tdelta=((t2-t1)-(t4-t3))/2;
		RTC_SetCounter(RTC_GetCounter()+tdelta);
		frame_totalclear(input);

		USART_Send(tdelta);

		break;
		
	case TPSN_FRAME_REQUEST:
		for(i=6;i<14;i++)
		{
			pkt[i+8]=pkt[i];
			pkt[i]=0;
		}
		TPSN_SET_TYPE(pkt,TPSN_FRAME_RESPONSE);
		addr=TPSN_SHORTADDRFROM(pkt);
		
		frame_totalcopyfrom(output,input);
		output->address=addr;
		output->option=0x00;
		frame_totalclear(input);
		break;
	}	
	return 1;
}


void tpsn_evolve( void * object, TiEvent * e)
{
	TiNioTPSN * tpsn = (TiNioTPSN *)object;
	return;
}
