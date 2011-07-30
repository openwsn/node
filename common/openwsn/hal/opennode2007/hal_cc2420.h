/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 *
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 *
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 *
 *****************************************************************************/

#ifndef _HAL_CC2420_H_1278_
#define _HAL_CC2420_H_1278_

/******************************************************************************
 * @author zhangwei on 2006-07-20
 * TiCc2420Adapter
 * This is an software abstraction of the cc2420 transceiver hardware. you can
 * fully manipulate the cc2420 hardware through this object.
 *
 * Though cc2420 is a IEEE 802.15.4 compatible chip, the TiCc2420Adapter is designed to
 * be as simple as possible. So it's not an standard PHY or MAC interface.
 *
 * @attention
 * 	- TiCc2420Adapter is different to TiSpiAdapter. TiSpiAdapter is only
 * the encapsulation of SPI communication. It shouldn't has relationships with
 * cc2420.
 *  - this module is a software abstraction and encapsulation of cc2420 hardware.
 * it should be as simple as possible.
 * 	- some one may regards this module as the PHY layer of the whole system.
 * however, it may contain some MAC functions. This is due to the reason that
 * the chip "cc2420" is 802.15.4 MAC supported.
 *
 * @modified by huanghuan on 2006-08-10
 * - modified the interface function.
 * - changes a lot data type from uint16 to uint8, because cc2420 support maximum
 * 	frame length of 256.
 * - add interface function: cc2420_configure, cc2420_sleep, cc2420_wakeup
 *
 * @modified by huanghuan on 2006-08-01
 * porting the old source code successfully
 * testing passed.
 *
 * @modified by zhangwei on 2006-09-11
 * add support to sleep in the state machine
 * revised the state machine and other source code.
 *
 * @modified by zhangwei on 2006-11-19
 * formulate the I/O interface of cc2420 drvier
 *
 * uint8 cc2420_rawread( TiCc2420Adapter * cc, char * buf, uint8 size, uint8 opt );
 * 从2420 driver内部buffer中读走一个frame, size表示参数buf所指缓冲区的大小
 * return the byte count moved to parameter "frame" buffer.
 * 返回实际move的字节数
 * 如果size偏小放不下，那么只最多size个字符。我们认为主程序有责任开辟足够大的buffer来调用rawread
 * 参数buf所指的buffer中将包含一个完整的PHY frame，或者内容无变动
 * 如果没有数据包可读，则返回0
 * opt参数备用
 *
 * uint8 cc2420_rawwrite( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 opt );
 * 将char * buf中的frame发送出去
 * 这里参数buf所指的内存中已经包含了一个完整的cc2420 phy frame，所有数据都已经在内存中排好了
 * 返回实际发送成功的字节数，一般情况下应该等于len，表示全部发送成功
 * 我们以一个完整的frame作为发送和接收单位，不支持发送或者接收半个frame
 *
 * int8 cc2420_write( TiCc2420Adapter * cc, TiCc2420AdapterFrame * frame, uint8 opt );
 * 类似于cc2420_rawwrite 但是frame信息是通过一个结构体TiCc2420AdapterFrame传递进来的，不是在内存中排好再传进来的
 *
 * int8 cc2420_read( TiCc2420Adapter * cc, TiCc2420AdapterFrame * frame, uint8 opt );
 * 类似于cc2420_rawread, 接收一个frame，但是接收到的数据不是排列在内存中，而是放在一个结构体中
 * opt中的某一位指明是否是broadcast还是普通的发送，是否需要ACK
 * opt中的设置将覆盖参数frame中的设置
 *
 * #define cc2420_rawbroadcast(cc,frame,len,opt) cc2420_rawwrite(cc,frame,len,opt|0x01)
 * #define cc2420_broadcast(cc,frame,opt) cc2420_sendframe(cc,frame,opt|0x01)
 * 广播一个frame出去，其实就是调用rawwrite或者sendframe
 * broadcast frame是不需要ACK的
 *
 * @modified by zhangwei on 20070510
 * add function cc2420_open() and cc2420_close().
 *
 * cc2420_init() is deprecated. you needn't call it in your source code. it will
 * called automatically in cc2420_configure().
 *
 * @modified by makun on 20070511
 * add support to disable address recognition for sniffer applications.
 *
 * @modified by zhangwei on 20070601
 * revise the old logic on read/write. and eliminate some unecessary variables.
 *
 *****************************************************************************/

#include "hal_foundation.h"
#include "hal_configall.h"
#include "hal_spi.h"
#include "hal_cc2420def.h"
#include "hal_openframe.h"

/******************************************************************************
 * IEEE 802.15.4 PPDU format
 * [4B Preamble][1B SFD][7b Framelength, 1b Reserved][nB PSDU/Payload]
 *
 * IEEE 802.15.4 MAC DATA format (the payload of PHY frame)
 * Beacon Frame
 * [2B Frame Control] [1B Sequence Number][4 or 10 Address][2 Superframe Specification]
 * 		[k GTS fields][m Padding address fields] [n Beacon payload][2 FCS]
 *
 * Data Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][n Data Payload][2 FCS]
 *
 * ACK Frame
 * [2B Frame Control] [1B Sequence Number][2 FCS]
 *
 * MAC Control Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 ADdress][1 Command Type][n Command Payload][2 FCS]
 *
 * IEEE 802.15.4 PHY Frame Header
 * [4B Preamble] [1B SFD] [7b FrameLength] [1b Reserved]
 *
 * SFD is 11100101b = 0xE5
 * Length is the total number of octets contained in PSDU(i.e. PHY payload)
 * it's a value between 8 and 127
 * the frame with length = 5 is often the acknowledgement
 *
 *****************************************************************************/

#define TiCc2420AdapterDriver TiCc2420Adapter

/* the default settings when the cc2420 transceiver first started
 * you should change it using cc2420_configure() to adapt to your own settings.
 *
 * @attention
 * the valid channel value is 11-26. the default channel is the at the center.
 */
#define CC2420_DEFAULT_PANID    0x2420
#define CC2420_DEFAULT_ADDRESS  0x1234
#define CC2420_DEFAULT_CHANNEL  19

/* cc2420 mode constants */
enum { CC_MODE_GENERAL=0x00, CC_MODE_SNIFFER=0x01 };

/* cc2420 state constants */
enum { CC_STATE_IDLE=0x00, CC_STATE_RECVING, CC_STATE_SENDING, CC_STATE_SLEEP,
	CC_STATE_POWERDOWN };

/* config control code */
#define CC2420_CONFIG_PANID 			0x01
#define CC2420_CONFIG_LOCALADDRESS		0x02
#define CC2420_CONFIG_TUNNING_POWER 	0x03
#define CC2420_CONFIG_CHANNEL			0x04
#define CC2420_BASIC_INIT               0x05
#define CC2420_XTAL_SWITCH              0x06
#define CC2420_CONFIG_APPLY				0x07
#define CC2420_SET_ACKREQUEST           0x08
#define CC2420_CONFIG_SNIFFER_MODE      0x09

/* power level
 * POWER_1 is the highest, while POWER_8 is the lowest
 * the default settings is @TODO
 */
#define CC2420_POWER_MIN CC2420_POWER_8
#define CC2420_POWER_MAX CC2420_POWER_1
#define CC2420_POWER_DEFAULT CC2420_POWER_MIN
#define CC2420_POWER_1  0x01       //  0dBm   17.4mA
#define CC2420_POWER_2  0x02       // -1dBm   16.5mA
#define CC2420_POWER_3  0x03       // -3dBm   15.2mA
#define CC2420_POWER_4  0x04       // -5dBm   13.9mA
#define CC2420_POWER_5  0x05       // -7dBm   12.5mA
#define CC2420_POWER_6  0x06       //-10dBm   11.2mA
#define CC2420_POWER_7  0x07       //-15dBm    8.9mA
#define CC2420_POWER_8  0x08       //-25dBm    8.5mA

#define cc2420_broadcast(cc,frame,len,opt) cc_2420_rawwrite(cc,frame,len,opt|0x01)

/* TiCc2420AdapterFrame/TOpenFrame is defined is hal_openframe.h.
 * it is an 802.15.4 frame mapping in the memory except the first "length" and
 * last "footer" member.
 *
 * [1B length] [2B control] [1B seqid] [1B panid] [2B nodeto] [2B nodefrom]
 * [OPF_PAYLOAD_SIZE payload] [2B footer]
 *
 * the first byte "length" is the 802.15.4 frame length. not the length of structure.
 * the length the structure occupied = 1B for "length" + length B for 802.15.4 frame
 *   + 2B for footer.
 */
#define TiCc2420AdapterFrame TiOpenFrame

/* TiCc2420Adapter object
 * this structure represent a cc2420 object in the system. you can interact with
 * the transceiver cc2420(by TI.com/chipcon.com) with this object. this object
 * is designed to hide all the hardware details and provide an abstract interface
 * for upper layer application.
 *
 * attention that cc2420 is a 802.15.4 MAC compatible transceiver. however, the
 * TiCc2420Adapter is a MAC object. i think the TiCc2420Adapter is below the MAC layer and higher
 * than the PHY layer.
 *
 * 	mode			general mode and sniffer mode
 * 	spi				used for the core MCU to communicate with the transceiver
 * 	channel			communication channel setting. represents the frequency.
 * 	                the valid channel value varies from 11 to 26 .
 * 	                while, the frequvency f = 2405 + 5*(channel - 11) MHz
 * 	rxbuf			used to hold the frame just received
 * 	rxlen			the data length in the rxbuf. if "rxlen == 0", then means
 * 					their's no frame received.
 * 	txbuf			used to hold the frame to be sent
 * 	txlen			the data length of the frame to be sent. if "txlen == 0",
 * 	                then means the TiCc2420Adapter is avaiable for sending new frames
 *  seqid           the next sequence id to used in the sending frame.
 *                  it will increase automatically after sending.
 *  ack_request		option. to indicate whether the sender required ACK frame
 * 	                or not.
 *  ack_response    TRUE means ACK frame received. FALSE means NAK.
 *                  it can be used to indicate whether the last sending received
 *                  ACK or not. generally you need not use it.
 * receiveOn		is an status flag. used to eliminate un-necessary ON/OFF
 *                  operations. because they are time/energy consuming.
 */
typedef struct{
  uint8 mode;
  uint8 state;
  volatile uint8 nextstate;
  TiSpiAdapter * spi;
  uint16 panid;
  uint16 address;
  uint8 channel;
  volatile uint8 txlen;
  volatile uint8 rxlen;
  TiCc2420AdapterFrame txbuf;
  volatile TiCc2420AdapterFrame rxbuf;
  uint8  sleeprequest;
  uint8  power;
  uint8  ackrequest;
  volatile bool ack_response;
  volatile uint8 seqid;
  volatile uint8 rssi;          //最近一次接收到的信息的信号强度
  BOOL receiveOn;				// @TODO: @modified zhangwei on 20070601. i do think this variable is no use.
}TiCc2420Adapter;

/* @attention: The following variable is declared usually in "global.c". However,
 * it is used in the interrupt service routine of this module. Be sure the variable
 * name cannot be modified! it must be "g_cc2420"!
 */
extern TiCc2420Adapter * g_cc2420;

/******************************************************************************
 * construct(): initialize memory
 * destroy(): destroy memory used by the object. may do further resource releasing
 *  	process.
 *****************************************************************************/
TiCc2420Adapter * cc2420_construct( char * buf, uint16 size, TiSpiAdapter * spi );
void cc2420_destroy( TiCc2420Adapter * cc );

/******************************************************************************
 * configure the cc2420 driver object.
 * you must do configure() before you can do read()/write().
 *
 * you may add more parameters in the functions.
 * different to the parameters in the construct function, you can use this function
 * to modify parameters without rebooting the system to make them take effect.
 *
 * @param
 * idx
 * 	1	设置地址
 * 	2	调整发射功率
 * 	3	调整发射频率
 * pvalue	the buffer pointers to the configure data. it is also the buffer
 * 			to receive the results.
 * size		buffer size.
 *****************************************************************************/

 /* @modified by zhangwei on 20061108
  * 我还是觉得不要用uint16, 而用void * buf或者 void * pvalue比较好，它和后面的
  * size合在一起表示配置数据。用uint16看似代码方便，但是使用者会没有观念，不清楚
  * 配置数据要多大比较合适,而且代码也要做强制类型转换。pvalue方式强制用户要设置size.
  * 写代码的原则是接口原型应尽量做到满足未来需求，而不是眼前需要。函数的实现可以
  * 改，但函数原型一旦确定，就不宜轻易变动，否则日后维护容易出错
  */
void cc2420_configure( TiCc2420Adapter * cc, uint8 ctrlcode, uint16 value, uint8 size );

/******************************************************************************
 * open(): prepare the hardware for sending/receiving data. different to construct(),
 *		open() contains the processing to interact with the hardware.
 * close(): disable the hardware and release necessary resources.
 *****************************************************************************/
void cc2420_open( TiCc2420Adapter * cc );
void cc2420_close( TiCc2420Adapter * cc );

/******************************************************************************
 * read data from cc2420 driver
 * read data from the driver's internal buffer. the interrupt service routine
 * is responsible to place the received data into the internal buffer.
 * you can always call this function to check whether there has data in the internal
 * buffer no matter what state.
 *
 * @attention
 * you may lost the input data when you cannot call this function fast enough.
 *
 *****************************************************************************/
int8 cc2420_read( TiCc2420Adapter * cc,TiCc2420AdapterFrame * frame, uint8 opt);

/******************************************************************************
 * return the received frame entirely to the frame buffer.
 * the only different between read() and rawread() is that rawread() will place
 * the received frame directly into a buffer other than a structure.
 *
 * @attention
 * 	- similar to cc_read(), you'll lost the received frame if you cannot call this
 * function quickly enough.
 * 	- when this function is effective, you must guarantee the internal buffer is
 * large enough to hold a frame.
 *  - if the input buffer "capacity" is not large enough, the additional data
 * will be truncated and lost.
 *  - you can freely mix using read() and rawread() in your program.
 *
 * @param
 * 	buf			the memory to receive the data
 * 	size		buffer size
 * 	opt			option settings.
 * @return
 * 	the character count copied successfully to the buffer
 *
 *****************************************************************************/
int8 cc2420_rawread( TiCc2420Adapter * cc, char * buf, uint8 capacity, uint8 opt );

/******************************************************************************
 * write(): send a frame out.
 * 		attention, you must set some members in the "frame" structure before you
 * 		call write() to send them out. they are the following:
 * 			frame->length. panid, nodeto, payload
 *
 * rawwrite(): quite similar to write() except that it uses a memory buffer other
 * 		than a structure to pass the frame data.
 *****************************************************************************/
int8 cc2420_write( TiCc2420Adapter * cc, TiCc2420AdapterFrame * frame, uint8 opt);
int8 cc2420_rawwrite( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 opt );

/******************************************************************************
 * this function is used mainly by the driver itself.
 * it will check whether there are some data to be sent. if there's data in the
 * txbuf, then it starts the sending process.
 *
 * this function doesn't check whether there are some data received like some other
 * drivers, because the data receiving process is interrupt driven.
 *
 * generally, you needn't to call this function in your source code.
 *****************************************************************************/
int8 cc2420_evolve( TiCc2420Adapter * cc );

/******************************************************************************
 * return the cc2420 driver state.
 * attention that state is used for other modules. it is not the same as "mode".
 *****************************************************************************/
uint8 cc2420_state( TiCc2420Adapter * cc );
uint8 cc2420_ioresult( TiCc2420Adapter * cc );

/******************************************************************************
 * set the TX sending power of the transceiver chip. this will affect the communication
 * distance. there're some constants already defined for you:
 *
 * #define CC2420_POWER_MIN CC2420_POWER_8
 * #define CC2420_POWER_MAX CC2420_POWER_1
 * #define CC2420_POWER_1  0x01       //  0dBm   17.4mA
 * #define CC2420_POWER_2  0x02       // -1dBm   16.5mA
 * #define CC2420_POWER_3  0x03       // -3dBm   15.2mA
 * #define CC2420_POWER_4  0x04       // -5dBm   13.9mA
 * #define CC2420_POWER_5  0x05       // -7dBm   12.5mA
 * #define CC2420_POWER_6  0x06       //-10dBm   11.2mA
 * #define CC2420_POWER_7  0x07       //-15dBm    8.9mA
 * #define CC2420_POWER_8  0x08       //-25dBm    8.5mA
 *
 *****************************************************************************/
void cc2420_set_power(TiCc2420Adapter * cc,uint8 power);

/******************************************************************************
 * set channel of the transceiver.
 * this will affect the frequency.
 *****************************************************************************/
void cc2420_setchannel( TiCc2420Adapter * cc, uint8 channel );


void cc2420_receive_on(TiCc2420Adapter * cc);
void cc2420_receive_off(TiCc2420Adapter * cc);

void _cc2420_waitfor_crystal_oscillator(TiSpiAdapter * spi);
uint8 cc2420_rssi( TiCc2420Adapter * cc );

void cc2420_powerdown( TiCc2420Adapter * cc );
void cc2420_activate( TiCc2420Adapter * cc );
void cc2420_sleep( TiCc2420Adapter * cc );
void cc2420_wakeup( TiCc2420Adapter * cc );

void cc2420_disable_interrupt( TiCc2420Adapter * cc );
void cc2420_enable_interrupt( TiCc2420Adapter * cc );

#ifdef CONFIG_DEBUG
void cc2420_dump( TiCc2420Adapter * cc );
#endif

//void cc2420_test_command();
//void cc2420_test_ram();

#endif /* _HAL_CC2420_H_ */
