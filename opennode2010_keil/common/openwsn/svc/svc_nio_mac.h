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
	#define TiNioMac TiAloha
	#define mac_construct(buf,size) aloha_construct((buf),(size)) 
	#define mac_open(mac,rxtx,nac,chn,panid,address,timer,option) aloha_open((mac),(rxtx),(nac),(chn),(panid),(address),(timer),(option))
	#define mac_destroy(mac) aloha_destroy((mac))
	#define mac_close(mac) aloha_close((mac))
	#define mac_send(mac,addr,frame,option)  aloha_send((mac),(addr),(frame),(option))
	#define mac_broadcast(mac,frame,option)  aloha_broadcast((mac),(frame),(option))
	#define mac_recv(mac,frame,option)  aloha_recv((mac),(frame),(option))
	#define mac_evolve(mac,e)  aloha_evolve((mac),(e))
	#define mac_setlocaladdress(mac,addr)  aloha_setlocaladdress((mac),(addr))
	#define mac_setremoteaddress(mac,addr)  aloha_setremoteaddress((mac),(addr))
	#define mac_setpanid(mac,pan)  aloha_setpanid((mac),(pan))
	#define mac_setchannel(mac,chn)  aloha_setchannel((mac),(chn))
	#define mac_ischannelclear(mac)  aloha_ischannelclear(mac)
	#define mac_setprobability(mac,prob)  aloha_setsendprobability((mac),(prob))
	#define mac_statistics(mac,stat)  aloha_statistics((mac),(stat))
	#define MAC_IORET_ERROR_NOACK 	ALOHA_IORET_ERROR_NOACK
	//#define MAC_IORET_ERROR_ACCEPTED_AND_BUSY ALOHA_IORET_ERROR_ACCEPTED_AND_BUSY
	#define MAC_IORET_ERROR_ACCEPTED_AND_BUSY 0
	#define MAC_IORET_SUCCESS(retval)  ALOHA_IORET_SUCCESS((retval))
#endif

/* TiCsma
 * CSMA protocol. The default protocol recommended to use in real applications.
 */
#ifdef CONFIG_CSMA_ENABLED
	#define TiNioMac TiCsma
	#define mac_construct(buf,size) csma_construct((buf),(size)) 
	#define mac_open(mac,rxtx,nac,chn,panid,address,timer,option) csma_open((mac),(rxtx),(nac),(chn),(panid),(address),(timer),(option))
	#define mac_destroy(mac) csma_destroy((mac))
	#define mac_close(mac) csma_close((mac))
	#define mac_send(mac,addr,frame,option)  csma_send((mac),(addr),(frame),(option))
	#define mac_broadcast(mac,frame,option)  csma_broadcast((mac),(frame),(option))
	#define mac_recv(mac,frame,option)  csma_recv((mac),(frame),(option))
	#define mac_evolve(mac,e)  csma_evolve((mac),(e))
	#define mac_setlocaladdress(mac,addr)  csma_setlocaladdress((mac),(addr))
	#define mac_setremoteaddress(mac,addr)  csma_setremoteaddress((mac),(addr))
	#define mac_setpanid(mac,pan)  csma_setpanid((mac),(pan))
	#define mac_setchannel(mac,chn)  csma_setchannel((mac),(chn))
	#define mac_ischannelclear(mac)  csma_ischannelclear(mac)
	#define mac_setprobability(mac,prob)  csma_setsendprobability((mac),(prob))
	#define mac_statistics(mac,stat)  csma_statistics((mac),(stat))
	#define mac_setsendprobability(mac,prob) csma_setsendprobability((mac),(prob))
	#define MAC_IORET_ERROR_NOACK 	CSMA_IORET_ERROR_NOACK
	#define MAC_IORET_ERROR_ACCEPTED_AND_BUSY CSMA_IORET_ERROR_ACCEPTED_AND_BUSY
	#define MAC_IORET_SUCCESS(retval)  CSMA_IORET_SUCCESS((retval))
#endif /* CONFIG_CSMA_ENABLED */

#ifdef CONFIG_MACA_ENABLED
#endif


#endif
