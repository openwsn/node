#ifndef _SVC_NIO_MAC_H_5738_
#define _SVC_NIO_MAC_H_5738_

#define CONFIG_CSMA_ENABLED

/* TinyMAC
 * which is an simple encapsulation of the low level transceiver interface and to
 * provide MAC layer like interface. Mainly used for testing, demonstration and evaluation
 * purpose.
 */
#ifdef CONFIG_TINYMAC_ENABLED
#endif

/* TiAloha
 * Aloha protocol. Mainly used as an comparison standard. Can also be used in some
 * simple applications.
 */
#ifdef CONFIG_ALOHA_ENABLED
#endif

/* TiCsma
 * CSMA protocol. The default protocol recommended to use in real applications.
 */
#ifdef CONFIG_CSMA_ENABLED
#define TiNioMac TiNioCsma
#define mac_construct(buf,size) csma_construct((buf),(size)) 
#define mac_destroy(mac) csma_destroy((mac))
#define mac_open(mac,rxtx,nac,chn,panid,address,timer,option) csma_open((mac),(rxtx),(nac),(chn),(panid),(address),(timer),(option))
#define 
void csma_close( TiCsma * mac );


intx csma_send( TiCsma * mac, uint16 shortaddrto, TiFrame * frame, uint8 option );

/**
 * \brief This function broadcast a frame out.
 */
intx csma_broadcast( TiCsma * mac, TiFrame * frame, uint8 option );

/**
 * \brief This function try to receive a frame and place it into parameter "frame".
 * If there's no frame coming, then returns 0.
 */
intx csma_recv( TiCsma * mac, TiFrame * frame, uint8 option );


/**
 * \brief Evolve the state machine of csma protocol.
 */
void csma_evolve( void * macptr, TiEvent * e );

inline void csma_setlocaladdress( TiCsma * mac, uint16 addr )
{
    mac->rxtx->setshortaddress( mac->rxtx->provider, addr );
}

inline void csma_setremoteaddress( TiCsma * mac, uint16 addr )
{
	mac->shortaddrto = addr;
}

inline void csma_setpanid( TiCsma * mac, uint16 pan )
{
    mac->rxtx->setpanid( mac->rxtx->provider, pan );
    mac->panto = pan;
	mac->panfrom = pan;
}

inline void csma_setchannel( TiCsma * mac, uint8 chn )
{
    mac->rxtx->setchannel( mac->rxtx->provider, chn );
}

// todo
inline bool csma_ischannelclear( TiCsma * mac )
{
    // return (mac->rxtx->ischnclear == NULL) ? true : mac->rxtx->ischnclear( mac->rxtx->provider );
    return true;
}

void csma_statistics( TiCsma * mac, uint16 * sendcount, uint16 * sendfailed );


#endif /* CONFIG_CSMA_ENABLED */

#ifdef CONFIG_MACA_ENABLED
#endif


#endif
