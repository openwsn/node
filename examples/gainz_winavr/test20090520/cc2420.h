//----------------------------------------------------//
//-------Institute Of  Computing Technology-----------//
//------------Chinese Academic  Science---------------//
//-----中国科学院计算技术研究所先进测试技术实验室-----//
//----------------------------------------------------//

/**
 * www.wsn.net.cn
 * @copyright:nbicc_lpp
 * @data:2005.11.22
 * @version:0.0.1
 * @updata:$
 *
 */
#ifndef rfdev_H
#define rfdev_H

#include "top.h"

bool cc2420_bramspi;
uint8_t *cc2420_rambuf;
uint8_t cc2420_ramlen;
uint16_t cc2420_ramaddr;

bool cc2420fifo_bspi;

uint8_t *cc2420fifo_txbuf;
uint8_t   cc2420fifo_txlength;

uint8_t *cc2420_rxbuf[2];
uint8_t   cc2420_rxlength;
uint8_t  *cc2420_rxbuf_now;
uint8_t    cc2420_rxlength_now;
uint8_t    cc2420_rxbufuse;
uint8_t   rfpackernum;
uint8_t    cc2420_rxflag;

uint16_t  cc2420_currentparameters[14];

demo_datapacket * demopacket;
uint8_t currentchannel;
//--------------------------------------------------------------------------------------
uint8_t cc2420_stateRadio;
volatile uint16_t cc2420_LocalAddr;
uint8_t ieee_address[8];
//----------------------------------------------------------------------------------------


inline  uint8_t cc2420_pin_init(void);
inline  uint8_t cc2420_reginit(void);
bool cc2420_setreg(void);
inline uint8_t cc2420_fifopintr(void);
inline  uint8_t cc2420_enablefifop(void);
uint8_t cc2420_sendcmd(uint8_t addr);
uint8_t cc2420_writereg(uint8_t addr, uint16_t data);
uint16_t cc2420_readreg(uint8_t addr);
uint8_t cc2420_readlargeram(uint16_t addr, uint8_t length, uint8_t *buffer);
uint8_t cc2420_writelargeram(uint16_t addr, uint8_t length, uint8_t *buffer);
uint8_t cc2420_writetxfifo(uint8_t len, uint8_t *msg);
void cc2420_readrxfifo(void);
inline  uint8_t cc2420_oscon(void);
inline uint8_t  cc2420_oscoff(void);
inline  uint8_t cc2420_VREFOn(void);
inline  uint8_t cc2420_VREFOff(void);
inline  uint8_t cc2420_enableAutoAck(void) ;
inline  uint8_t cc2420_disableAutoAck(void);
inline  uint8_t cc2420_enableAddrDecode(void);
inline  uint8_t cc2420_disableAddrDecode(void) ;
inline  uint8_t cc2420_setrxmode(void);
inline  uint8_t cc2420_settxmode(void);
inline  uint8_t cc2420_settxccamode(void);
inline  uint8_t cc2420_setrfoff(void);
inline  uint8_t cc2420_setchnl(uint8_t chnl);
inline  uint8_t cc2420_setshortaddress(uint16_t addr);
inline  uint8_t cc2420_getshortaddress(uint16_t * addr);
inline void      cc2420_init(void);
void demo_handlepacket(void);

/*********************added by zw*********************/
//uint8_t cc2420_readframe(char * buf, uint8_t size );
//uint8_t cc2420_writeframe( char * buf, uint8_t len );
//uint8_t cc2420_readframe(uint8_t * msg, uint8_t size );	//20090516
//uint8_t cc2420_writeframe( uint8_t * msg, uint8_t len );	//20090516
uint8_t cc2420_sendframe( uint8_t * msg, uint8_t len );	//20090520
uint8_t cc2420_receiveframe(uint8_t *msg);	//20090520
//uint8_t cc2420_status();	//20090520

/*
struct TiCc2420Frame
{
  uint8 len;
  uint16 control;
  uint panid;
  uint16 addrfrom;
  uint16 addtto;
  char data[100];
}


uint8_t cc2420_readstruct( TiCc2420Frame * frame );
uint8_t cc2420_writestruct( TiCc2420Frame * frame );
*/
void change(uint8_t x,uint8_t y);
/*****************************************************/
#endif
