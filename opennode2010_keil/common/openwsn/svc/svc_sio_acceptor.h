#ifndef _AVC_IO4RS232_H_2143_
#define _AVC_IO4RS232_H_2143_

/* Library for RS232 serial port manipulation with frame filter capability.
 * @author Zhang Wei (TJU) on 2011.07.28
 */

/* devx_configall will recognized the following macro and give correct definition of DLLFUNC */
#define CONFIG_DLL

#include "../common/devx/devx_configall.h"
#include <stdint.h>

#define CONFIG_IO4RS232_TXBUF_CAPACITY 254
#define CONFIG_IO4RS232_RXBUF_CAPACITY 254
#define CONFIG_IO4RS232_TMPBUF_CAPACITY 64

/* Reference
 * - 从 DLL 导出, http://msdn.microsoft.com/zh-cn/library/z4zxe9k8(v=VS.80).aspx
 * - 确定要使用的导出方法, 
 *   http://msdn.microsoft.com/zh-cn/library/900axts6(v=VS.80).aspx
 * - 使用 DEF 文件从 DLL 导出, 
 *   http://msdn.microsoft.com/zh-cn/library/d91k01sh(v=VS.80).aspx
 * - 使用 __declspec(dllexport) 从 DLL 导出, 
 *   http://msdn.microsoft.com/zh-cn/library/a90k134d(VS.80).aspx
 * - 导出 C++ 函数以用于 C 语言可执行文件, 
 *   http://msdn.microsoft.com/zh-cn/library/wf2w9f6x(v=VS.80).aspx
 * - 导出 C 函数以用于 C 或 C++ 语言可执行文件, 
 *   http://msdn.microsoft.com/zh-cn/library/ys435b3s(v=VS.80).aspx
 * - 串口通信问题
 *   http://social.msdn.microsoft.com/Forums/zh-CN/1729/thread/5c485ca1-f62e-45a6-8e35-2a1803e4467a/
 */

/*
#ifdef CONFIG_APPLICATION
#define DLLFUNC __declspec(dllimport) 
#else
#define DLLFUNC __declspec(dllexport) 
#endif
*/

#define DLLFUNC __declspec(dllexport) 

/* attention
 * If sizeof(void*) == 4, then:
 * 	#define TiHandleId uint32_t
 * If sizeof(void*) == 8, then:
 * 	#define TiHandleId uint32_t
 */
 
#define TiHandleId uint32

/**
 * This module defined the interface of librs232 dynamic link library
 * which can be used in other program files such as C# or LabView.
 */

#ifdef __cplusplus
extern "C" {
#endif


    
/**
 * TiSioAcceptor component
 * 
 * Q: What's the difference between TiSioAcceptor and TiUartAdapter/TxUartAdapter?
 * R: TiUartAdapter/TxUartAdapter implements a byte-oriented transceiver which is
 * an light-weight encapsulation of the UART/USART/Serial Port hardware.
 *
 * TiSioAcceptor is running on top of TiUartAdapter/TxUartAdapter component. It
 * further implements a frame based transceiver interface. Everytime you call read()/write()
 * function of TiSioAcceptor, you will read/write an complete packet/frame.
 * The framing mechanism currently is based on the rules in SLIP protocol. 
 */
typedef struct{
	uint8 state;
	TiUartAdapter * device;
	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;
    #ifdef RS232_IOSERVICE_SLIP_ENABLE
	TiIoBuf * tmpbuf;
	uint8 rx_accepted;
	TiSlipFilter slipfilter;
    #endif
	// you can add your variables here
}TiSioAcceptor;

extern TiSioAcceptor m_sio;

/*DLLFUNC TiHandleId io_rs232_open( const TCHAR * name, uint32 baudrate, uint8 databits, uint8 stopbits, uint8 parity );
DLLFUNC void io_rs232_close( TiHandleId service );
DLLFUNC int32 io_rs232_read( TiHandleId service, char * buf, uint32 size, uint32 option );
DLLFUNC int32 io_rs232_write( TiHandleId service, char * buf, uint32 len, uint32 option );
DLLFUNC void io_rs232_evolve(  TiHandleId service );*/

void sac_write( TiSioAcceptor * sac, TiFrame * buf, uint8 len,uint8 option ); 
void sac_read( TiSioAcceptor * sac, TiFrame * buf, uint8 option ); 
void sac_evolve( TiSioAcceptor * sac, TiFrame * buf, uint8 option ); 
void sac_close( TiSioAcceptor * sac );
TiSioAcceptor * sac_open( TiSioAcceptor * buf, size_t size, TiUartAdapter * uart );



#ifdef __cplusplus
}
#endif

#endif

