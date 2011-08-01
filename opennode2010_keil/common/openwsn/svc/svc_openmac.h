/**
 * This module defines the interface of an Wireless Medium Access (MAC) component. 
 * All the MAC component in the library must provide such interface in order to 
 * make the higher level coomponent such as NET or APP can change the low level 
 * MAC protocols.
 */
 
 typedef struct{
 }TiOpenMacInterface;
 
 svc_openmac
 



typedef struct{
}TiMacStatistics;


/**
 * TiOpenMacInterface
 * Regulate the medium access control layer's interface. all the MAC protocols, including
 * TiTinyMAC, TiSimpleAloha, TiAloha, TiCsma and TiMACA, should comply to this interface.
 */

typedef struct{
    void * provider;
}TiMacInterface;





#include "svc_configall.h"
#include "../rtl/rtl_frame.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_frame_transceiver.h"
#include "../hal/hal_timer.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "svc_foundation.h"

#define TINYMAC_OPTION_ACK 0x00
#define TINYMAC_DEF_OPTION 0x00

#ifdef __cplusplus
extern "C"{
#endif

TiTinyMAC *	tinymac_construct( char * buf, uintx size );
void tinymac_destroy( TiTinyMAC * mac );

/**
 * open the tiny mac service component for sending and recving. 
 *
 * attention: you should open the transceiver component successfully before calling
 * tinymac_open().
 */

TiTinyMAC *	tinymac_open( TiTinyMAC * mac, TiFrameTxRxInterface * rxtx, TiNioAcceptor * nac, uint8 chn, uint16 panid, 
			uint16 address, TiFunEventHandler listener, void * lisowner, uint8 option );
void        tinymac_close( TiTinyMAC * mac );

/** if bit 0 of option is 1, then this function will request ACK from the receiver.
 */
intx tinymac_send( TiTinyMAC * mac,TiFrame * frame, uint8 option);
intx tinymac_broadcast( TiTinyMAC * mac, TiFrame * frame, uint8 option );
intx tinymac_recv( TiTinyMAC * mac, TiFrame * frame, uint8 option );

void tinymac_evolve( void * macptr, TiEvent * e );

inline void tinymac_setlocaladdress( TiTinyMAC * mac, uint16 addr )
{
    mac->rxtx->setshortaddress( mac->rxtx->provider, addr );
}

inline void tinymac_setremoteaddress( TiTinyMAC * mac, uint16 addr )
{
	mac->shortaddrto = addr;
}

inline void tinymac_setpanid( TiTinyMAC * mac, uint16 pan )
{
    mac->rxtx->setpanid( mac->rxtx->provider, pan );
    mac->panto = pan;
	mac->panfrom = pan;
}

inline void tinymac_setchannel( TiTinyMAC * mac, uint8 chn )
{
    mac->rxtx->setchannel( mac->rxtx->provider, chn );
}

inline void tinymac_ischannelclear( TiTinyMAC * mac )
{
    mac->rxtx->ischnclear( mac->rxtx->provider );
}
