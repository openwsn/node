/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/

#ifndef _SVC_IOFILTER_H_9872_
#define _SVC_IOFILTER_H_9872_

#include "svc_configall.h"
#include "../rtl/rtl_openframe.h"
#include "../rtl/rtl_framequeue.h"
#include "svc_foundation.h"

struct TiIoFilter;
struct TiIoSession;
struct TiIoFilterChain;

/* TiIoFilter Interface
 * This module doesn't implement a IoFilter object, but regulate the interface of 
 * an IoFilter. All IoFilter object must implement the following interface because 
 * they'll be used by the filter chain. 
 */

typedef (TiIoFilter * )(* TiFunIoFilterConstruct)( void * mem, uint16 size );
typedef (void)(* TiFunIoFilterDestroy)();
typedef (TiIoFilter *)(TiFunIoFilterOpen)( TiIoFilter * filter, TiFrameQueue * rxque, 
	TiFrameQueue * txque, TiIoFilterChain * chain );
typedef (TiIoFilter *)(TiFunIoFilterSetOwnerChain)( TiIoFilter * filter, TiIoFilterChain * chain, 
	TiFrameQueue * rxque, TiFrameQueue * txque );
typedef (void)(* TiFunIoFilterClose)();
typedef (uint8)(* TiFunIoProcess)( void * object, TiFrameQueue * rxque, TiFrameQueue * txque );

typedef struct{
	void * iofilter;
	TiFunIoFilterSetOwnerChain iof_setchain;
	TiFunEvolve iof_evolve;
}TiIoFilterInterface;

#define TiIoFilter TiIoFilterInterface 


default IoFilter implementation
is actually the iohandler in MINA

typedef struct{
	void * owner;
	uint8 iofcount;
	TiIoFilter * iofilters[IOF_FILTER_CAPACITY];
}TiIoFilterChain;

TiIoFilterChain * iof_chain_construct( void * mem, uint16 size );
void iof_chain_destroy( TiIoFilterChain * chain );
TiIoFilterChain * iof_chain_open( TiIoFilterChain * chain, TiIoHandler default_handler );
void iof_chain_close( TiIoFilterChain * chain );
bool iof_chain_append( TiIoFilterChain * chain, TiIoFilter * iof );
bool iof_chain_remove( TiIoFilterChain * chain, TiIoFilter * iof );









/* For the OpenWSN system, the TiIoBuffer is actually the same as TiOpenFrame */
#define TiIoBuffer TiOpenFrame

typedef (uintx)(* TiFunIoHandler)( void * object, TiIoSession * ses, TiIoBuffer * input, TiIoBuffer * output );

typedef struct{
	uint8 id;
	TiIoBuffer * request;
	TiIoBuffer * response;
}TiIoSession;

typedef (TiIoFilter * )(* TiFunIoFilterConstruct)( void * mem, uint16 size );
typedef (void)(* TiFunIoFilterDestroy)();
typedef (TiIoFilter *)(TiFunIoFilterOpen)( TiIoFilter * filter, TiIoFilterChain * chain );
typedef (void)(* TiFunIoFilterClose)();
typedef (uint8)(* TiFunIoHandler)( void * object, TiIoBuffer * input, TiIoBuffer * output );

typedef struct{
	void * filter;
	TiFunIoFilterConstruct construct;
	TiFunIoFilterDestroy destroy;
	TiFunIoFilterOpen open;
	TiFunIoFilterClose close;
	TiFunIoFilterHandle rxhandle;
	TiFunIoFilterHandle txhandle;
}TiIoFilter;


TiIoFilter * iof_construct( void * mem, uint16 size );
void iof_destroy( TiIoFilter * chain );
TiIoFilter * iof_open( TiIoFilter * chain, TiIoHandler default_handler );
void iof_close( TiIoFilter * chain );
uintx iof_rxhandle( void * object, TiIoBuffer * input, TiIoBuffer * output );
uintx iof_txhandle( void * object, TiIoBuffer * input, TiIoBuffer * output );

typedef struct{
	uint8 iofcount;
	TiIoFilter * iofilters[IOF_FILTER_CAPACITY];
	TiFunIoHandle handler;
}TiIoFilterChain;

TiIoFilterChain * iof_chain_construct( void * mem, uint16 size );
void iof_chain_destroy( TiIoFilterChain * chain );
TiIoFilterChain * iof_chain_open( TiIoFilterChain * chain, TiIoHandler default_handler );
void iof_chain_close( TiIoFilterChain * chain );
bool iof_chain_append( TiIoFilterChain * chain, TiIoFilter * iof );
bool iof_chain_remove( TiIoFilterChain * chain, TiIoFilter * iof );

uintx iof_chain_rxhandle( void * object, TiIoSession * ses, TiIoBuffer * input, TiIoBuffer * output );
uintx iof_chain_txhandle( void * object, TiIoSession * ses, TiIoBuffer * input, TiIoBuffer * output );
evolve()

iof_chain_service( service )
iof_chain_send
iof_chain_recv

iof_chain_on_frame_arrival()

iof_chain_uses( cc/mac )


iof_chain_read

iof_chain_handle( void * object, input, output )
iof_chain_send = txhandle









fltchain_
fltchain_append
fltchain_append
fltchain_append
fltchain_append


class TxIoFilterChain : public TxIoFilter{
private:
	void * m_owner;
	std::vector<TxIoFilter *> * m_fltchain;
	TxIoHandler * m_iohandler;
public:
	TxIoFilterChain( void * owner=NULL );
	virtual ~TxIoFilterChain();
	void append( TxIoFilter * flt );
	void remove( TxIoFilter * flt );
	TxIoHandler * iohandler() {return m_iohandler;};
	void setiohandler( TxIoHandler * iohandler );
	virtual int rxhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output );
	virtual int txhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output );







//-----------------------------------------------------------------------------
// iofilters architecture
// this module implements an filter architecture, which help to simplify the
// multi-layer networking protocol developing.
//
// @state
//	finished. not fully tested. but can be used in real projects.
//
// @author zhangwei on 20090415
//	- first author
//-----------------------------------------------------------------------------

#include "svx_configall.h"
#include <vector>
#include "svx_foundation.h"
#include "svx_objectbase.h"
#include "svx_framebuffer.h"

//-----------------------------------------------------------------------------
// an simple application architecture is
//
//               data stream        app packet
// TxNetworkAcceptor <--> TxIoFilterChain <--> TxIoHandler
//
//
// a more complex application architecture is:
//
//               data stream        app packet
// TxUartAcceptor <-----> TxIoFilterChain <---|
//                                        |---> TxIoMergeFilter <---> TxIoFilterChain <--> TxIoHandler
// TxNetworkAcceptor <--> TxIoFilterChain <---|
//               data stream        app packet
//
// Reference:
// [] the design of apache MINA networking/uart library, http://www.apache.org
// [] MINA based Application Architecture, http://mina.apache.org/mina-based-application-architecture.html
// [] IoBuffer in MINA, http://mina.apache.org/iobuffer.html
// [] IoHandler in MINA, http://mina.apache.org/iohandler.html
// [] MINA v2.0 Quick Start Guide, http://mina.apache.org/mina-v20-quick-start-guide.html
// [] IoFilter in MINA, http://mina.apache.org/iofilter.html
// [] ProxyFilter in MINA, http://mina.apache.org/report/trunk/xref/org/apache/mina/proxy/filter/ProxyFilter.html
//-----------------------------------------------------------------------------

class TxIoBuffer;
class TxIoSession;
class TxIoFilter;
class TxIoFilterChain;
class TxIoHandler;
class TxIoMergeHandler;

//-----------------------------------------------------------------------------
// TxIoBuffer
//-----------------------------------------------------------------------------

class TxIoBuffer : public TxFrameBuffer{
public:
	TxIoBuffer(uint capacity, uint skiplen=0) : TxFrameBuffer(capacity,skiplen) {};
	virtual ~TxIoBuffer() {};
};

//-----------------------------------------------------------------------------
// TxIoSession
//-----------------------------------------------------------------------------

// @modified by zhangwei on 20090419
//	- this class will be the third parameter of rxhandler() and txhandler()
// and it will replace the m_fltchain and m_session member variable in the TxIoBuffer.
// Buffer is just a buffer. you shouldn't let the buffer do too much.

class TxIoSession : public TxObjectBase{
	void * m_owner;
	uint m_state;
	TxIoFilterChain * m_fltchain;
public:
	TxIoSession( void * owner ) : m_owner(owner), m_state(0) {};
	void * owner();
	uint state() {return m_state;};
	void setfilterchain( TxIoFilterChain * fltchain ) {m_fltchain = fltchain;};
	TxIoFilterChain * getfilterchain() {return m_fltchain;};
};

//-----------------------------------------------------------------------------
// TxIoFilter
// architecture of filter chain design
//   TxIoFilter
//     => TxIoFilterChain: is also a filter
//   TxIoHandler: which is actually the last one in the filter chain
//     => TxIoMergeHandler: merge serveral filter's output into one
//-----------------------------------------------------------------------------

// owner: point to the filter chain belonged to
// txhandler and rxhandler:
//	return value >=0 means success.  == 0 means there's no output value
//  return value <0 means error occured.
//
class TxIoFilter : public TxObjectBase{
	void * m_owner;
public:
	TxIoFilter( void * owner=NULL ) : m_owner(NULL) {};
	virtual ~TxIoFilter() {};
	void * owner() {return m_owner;};
	void setowner( void * owner ){m_owner = owner;};
	virtual int rxhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )=0;
	virtual int txhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )=0;
};

//-----------------------------------------------------------------------------
// TxIoHandler
//
// server/acceptor <--> TxFilters <--> TxIoHandler
//                          |
//             maintain a list of filters
//
// Compare to apache MINA:
// In MINA, the IoHandler object handles all I/O events fired by MINA. The interface
// is hub of all activities done at the end of the Filter Chain. It supports the
// following event handlers
//
//	* sessionCreated
//	* sessionOpened
//	* sessionClosed
//	* sessionIdle
//	* exceptionCaught
//	* messageReceived
//	* messageSent
//
//-----------------------------------------------------------------------------

class TxIoHandler : public TxIoFilter{
public:
	TxIoHandler( void * owner=NULL ) : TxIoFilter(owner) {};
	virtual ~TxIoHandler() {};
	virtual int rxhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )=0;
	virtual int txhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output ){return 0;};
};

//-----------------------------------------------------------------------------
// TxIoFilterChain
// this is a filter chain. it's also a filter which means it can be append or insert
// into other filter chains.
//-----------------------------------------------------------------------------

class TxIoFilterChain : public TxIoFilter{
private:
	void * m_owner;
	std::vector<TxIoFilter *> * m_fltchain;
	TxIoHandler * m_iohandler;
public:
	TxIoFilterChain( void * owner=NULL );
	virtual ~TxIoFilterChain();
	void append( TxIoFilter * flt );
	void remove( TxIoFilter * flt );
	TxIoHandler * iohandler() {return m_iohandler;};
	void setiohandler( TxIoHandler * iohandler );
	virtual int rxhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output );
	virtual int txhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output );
};

//-----------------------------------------------------------------------------
// TxIoMergeHandler
//
// Q: why TxIoMergeHandler?
// R: considering the following case
//
//    FilterChain A ---|
//                 |--- MergeFilter ---- filters C
//    FilterChain B ---|
//
//-----------------------------------------------------------------------------

class TxIoMergeHandler : public TxIoHandler{
	std::vector<TxIoFilterChain *> * m_chains;
	TxIoFilterChain * m_merged;
public:
	TxIoMergeHandler( void * owner=NULL );
	virtual ~TxIoMergeHandler();
	void setowner( void * owner );
	bool addowner( void * owner );
	void connect( TxIoFilterChain * merged );
	//void setdefaultfilters( TxIoFilterChain * def );
	virtual int rxhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output );
	virtual int txhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output );
};

//-----------------------------------------------------------------------------
// TxIoEchoFilter
// this is a simple filter for testing only. it will reponse using the request
// data.
//-----------------------------------------------------------------------------

class TxIoEchoFilter : public TxIoFilter{
public:
	virtual int rxhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )
	{
		output->copyfrom( input );
		return output->datalen();
	}
	virtual int txhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )
	{
		output->copyfrom( input );
		return output->datalen();
	}
};

//-----------------------------------------------------------------------------
// TxIoEchoHandler
// this is a simple handler for testing only. it will reponse using the request
// data.
//-----------------------------------------------------------------------------

class TxIoEchoHandler : public TxIoHandler{
public:
	virtual int rxhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )
	{
		output->copyfrom(input);
		return output->datalen();
	}
	virtual int txhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )
	{
		return 0;
	}
};

#endif

