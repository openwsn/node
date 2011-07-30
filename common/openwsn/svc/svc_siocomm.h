#ifndef _SVC_SIOCOMM_H_4890_
#define _SVC_SIOCOMM_H_4890_

#include "svc_configall.h"
#include "svc_foundation.h"
#include "../hal/hal_device.h"
// #include "../hal/hal_uart.h"
#include "../rtl/rtl_iobuf.h"
#include "../rtl/rtl_textspliter.h"
#include "../rtl/rtl_textcodec.h"


//#include "../hal/hal_timer.h"

/* CONFIG_DYNA_MEMORY
 * If this macro defines, then sio_create() and sio_free() are enabled
 *
 * CONFIG_SIOCOMM_PKTSIZE
 * The maximum possible length of the packet that can be processed by the siocomm object.
 */


#ifndef CONFIG_SIOCOMM_PKTSIZE
#define CONFIG_SIOCOMM_PKTSIZE 255
#endif

#define SIOCOMM_HOPESIZE(pktsize) (sizeof(TiSioComm)+IOBUF_HOPESIZE(CONFIG_SIOCOMM_PKTSIZE)*3)


 //Q ???: 0x7F or 0x80? is 7F big enough?
#ifndef CONFIG_UART_RXBUFFER_SIZE 
  #define CONFIG_UART_RXBUFFER_SIZE 0x7F
#endif

#ifndef SIO_RXBUFFER_CAPACITY  
  #define SIO_RXBUFFER_CAPACITY 0xFF
#endif


#ifndef SIO_TXBUFFER_CAPACITY  
  #define SIO_TXBUFFER_CAPACITY SIO_RXBUFFER_CAPACITY
#endif

/* @modified by zhangwei on 2010.05.03
 *  - add three new members into the TiSioComm object. 
 *    lowlayer_object, lowlayer_read, lowlayer_write
 *    by switching their values, the siocomm module can change low level communication 
 *    driver to TiUartAdapter, socket communication or other transportation mechanisms
 *    without modifying the source code.
 *	- todo
 *  replace uart_read/uart_write with lowlayer_read/lowlayer_write
 *
 *  @modified by yanshixing 20100511
 *  - comment some #ifndef before
 *  - modified the structure of TiSioComm
 *  -
 */

#define SIO_START 0x55
#define SIO_END   0x99

#ifdef __cplusplus
extern "C" {
#endif

/* TiSioComm
 * serial communication service based on TiUartAdapter, Spliter, Codec and Packet Queue
 *
 * member descriptions
 * 	device: an pointer to the block device interface. an interface is an structure of
 * 			function pointers.
 *  provider: which device driver provide the interface.
 */
typedef struct{
  uint8 state;
  //TiUartAdapter * uart;
  char splitermem[sizeof(TiTextSpliter)];
  TiTextSpliter * spliter;
  uint8 option;
  //TiSvcTimer * timer;
  //uint8 sof;

  char txmem[ IOBUF_HOPESIZE(0x7F) ];
  char rxmem[ IOBUF_HOPESIZE(0x7F) ];
  char quemem[ IOBUF_HOPESIZE(0x7F) ];
  char tmp_buf[ IOBUF_HOPESIZE(0x7F) ];

  TiIoBuf * txbuf;
  TiIoBuf * rxbuf;
  TiIoBuf * rxque;
  TiIoBuf * tmp_iobuf;

  TiBlockDeviceInterface device;

  // the TiSioComm object has three internal TiIoBuf members. They're padded to the
  // siocomm object variable. we provides three small utility functions _sio_rxbuf(),
  // _sio_txbuf() and _sio_rxque() to help you deal with these conditions in the
  // svc_siocomm.c file,
  //
  // TiIoBuf * rxbuf;
  // TiIoBuf * txbuf;
  // TiIoBuf * rxque;
}TiSioComm;

#define SIO_HOPESIZE(size) (sizeof(TiSioComm)+size)

TiSioComm * sio_construct( char * buf, uint16 size );
void sio_destroy( TiSioComm * sio );
TiSioComm * sio_create( uint16 size );
void sio_free( TiSioComm * sio );

//TiSioComm * sio_open( TiSioComm * sio, TiUartAdapter * uart, uint8 opt );
TiSioComm * sio_open( TiSioComm * sio, TiBlockDeviceInterface * device, uint8 opt );
void sio_close( TiSioComm * sio );


/* Configure an TiSioComm service */
// todo: in the future, sio will add timeout functionalities
//void sio_configure( TiSioComm * sio, TiTimerAdapter * timer, uint8 opt );
void sio_configure( TiSioComm * sio, uint8 opt );

/* Read a packet from TiSioComm service. The packet is in the internal queue of TiSioComm */
uintx sio_read( TiSioComm * sio, TiIoBuf * iobuf, uintx option );

/* Write a packet into TiSioComm. The packet will be sent out through UART interface */
uintx sio_write( TiSioComm * sio, TiIoBuf * iobuf, uintx option );

/* Evolve the TiSioComm object */
void sio_evolve( TiSioComm * sio, TiEvent * e );

#ifdef __cplusplus
}
#endif

#endif /* _SVC_SIOCOMM_H_4890_ */
