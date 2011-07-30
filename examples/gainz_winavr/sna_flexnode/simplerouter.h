//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// 一个简单的Wireless Modem程序，称之为simple router。
// 它会把串口->SIO收到的packet从Wireless->Cc2420发送出去，并且把CC2420过来的数据包
// 通过Uart反向发送出去。
// 还包括了一个Background Debug Data Sending的调用。不过程序中并没有调用debug_write()
// 的地方。
//
// 该测试程序保证收发两条数据通路的正确性和可行性。
//----------------------------------------------------------------------------

#ifndef _SIMPLEROUTE_H_6753_
#define _SIMPLEROUTE_H_6753_

#include "..\foundation.h"
#include "..\global.h"

int16 splrouter_run( void );

#endif /*_SIMPLEROUTE_H_6753_*/
