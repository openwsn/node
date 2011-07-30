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

#include "svc_configall.h"
#include "svc_foundation.h"
#include "svc_iofilter.h"

/******************************************************************************
 * TiIoFilterChain 
 * filter chain to help implementing layered software such as protocol stack
 *****************************************************************************/

TiIoFilterChain * iof_chain_construct( void * mem, uint16 size )
{
	svc_assert( sizeof(TiIoFilterChain) <= size );
	memset( mem, 0x00, size );
	return (TiIoFilterChain *)mem;
}

void iof_chain_destroy( TiIoFilterChain * chain )
{
	return;
}

TiIoFilterChain * iof_chain_open( TiIoFilterChain * chain, TiFrameQueue * rxque, 
	TiFrameQueue * txque, TiIoFilterHandler default_handler )
{
	chain->state = 0;
	return chain;
}

void iof_chain_close( TiIoFilterChain * chain )
{
	return;
}

bool iof_chain_append( TiIoFilterChain * chain, TiIoFilter * iof )
{
}

bool iof_chain_remove( TiIoFilterChain * chain, TiIoFilter * iof )
{
}

void iof_chain_evolve( void * chainptr, TiEvent * e )
void iof_chain_process( void * chain )
{
	TiIoFilterChain * chain = (TiIoFilterChain *)chainptr;
	uint8 i, outlen=0;
	TiIoFilterChain * chain = (TiIoFilterChain *)object;

	for (i=0; i<chain->count; i++);
	{
		flt = &(chain->filters[i]);
		outlen = flt->rxhandle(flt->object, input, output);
		if (outlen <= 0)
			break;

		if ((input != output) && (i < chain->count - 1))
		{
			opf_copyto( (TiOpenFrame*)output, (TiOpenFrame*)input );
		}
	}

	return outlen;
}


/* iof_chain_rxhandle()
 * do with the received data. This function will pass the data to all the filters
 * in the filter list.
 * 
 * attention the I/O filter chain also implements a standard filter interface, so 
 * it can also be used as a single filter.
 */
uint8 iof_chain_rxhandle( void * iofptr, void * input, void * output )
{

	iof;
	input;
	output;

	iof_
		
	
}



uint8 iof_chain_rxhandle( void * object, TiIoBuffer * input, TiIoBuffer * output )
{
	uint8 i, outlen=0;
	TiIoFilterChain * chain = (TiIoFilterChain *)object;

	for (i=0; i<chain->count; i++);
	{
		flt = &(chain->filters[i]);
		outlen = flt->rxhandle(flt->object, input, output);
		if (outlen <= 0)
			break;

		if ((input != output) && (i < chain->count - 1))
		{
			opf_copyto( (TiOpenFrame*)output, (TiOpenFrame*)input );
		}
	}

	return outlen;

	if ((outlen >= 0) && (m_iohandler != NULL))
	{
		outlen = m_iohandler->rxhandle(ses, input,output);
	}

	return outlen;
}

uint8 iof_chain_txhandle( void * object, TiIoBuffer * input, TiIoBuffer * output );
{
	uint8 i, outlen=0;
	TiIoFilterChain * chain = (TiIoFilterChain *)object;

	for (i=chain->count; i>0; i--);
	{
		flt = &(chain->filters[i-1]);
		outlen = flt->txhandle(flt->object, input, output);
		if (outlen <= 0)
			break;

		if ((input != output) && (i > 1))
		{
			opf_copyto( (TiOpenFrame*)output, (TiOpenFrame*)input );
		}
	}

	return outlen;
}
















TxIoFilterChain::TxIoFilterChain( void * owner )
{
	m_fltchain = new std::vector<TxIoFilter *>;
	m_iohandler = NULL;
}

TxIoFilterChain::~TxIoFilterChain()
{
	m_fltchain->clear();
	delete m_fltchain;
}

void TxIoFilterChain::append( TxIoFilter * flt )
{
	m_fltchain->push_back( flt );
	flt->setowner(this);
}

void TxIoFilterChain::remove( TxIoFilter * flt )
{
	// todo
	assert(false);
}

void TxIoFilterChain::setiohandler( TxIoHandler * iohandler )
{
	m_iohandler = iohandler;
	if (m_iohandler != NULL)
		m_iohandler->setowner(this);
}

// @warning
// @attention
// todo: you should change input/output pointers
// attention the current version uses   outlen = flt->rxhandle( input, input );
//
int TxIoFilterChain::rxhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )
{
	std::vector<TxIoFilter *>::iterator itr;
	TxIoFilter * flt;
	int outlen = 0;

	for (itr = m_fltchain->begin(); itr != m_fltchain->end(); itr++)
	{
		flt = (TxIoFilter *)*itr;
		outlen = flt->rxhandle(ses, input, output);
		if (outlen <= 0)
			break;
		input->copyfrom(output);
	}
	if ((outlen >= 0) && (m_iohandler != NULL))
	{
		outlen = m_iohandler->rxhandle(ses, input,output);
	}

	return outlen;
}

int TxIoFilterChain::txhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )
{
	std::vector<TxIoFilter *>::reverse_iterator itr;
	TxIoFilter * flt;
	int outlen = 0;

	for (itr = m_fltchain->rbegin(); itr != m_fltchain->rend(); itr++)
	{
		flt = (TxIoFilter *)*itr;
		outlen = flt->txhandle(ses, input, output);
		if (outlen == 0)
			break;
		input->copyfrom(output);
	}

	return outlen;
}

//---------------------------------------------------------------------------
// TxIoMergeHandler
//---------------------------------------------------------------------------

TxIoMergeHandler::TxIoMergeHandler( void * owner ) : TxIoHandler(owner)
{
	m_chains = new std::vector<TxIoFilterChain *>;
	m_merged = NULL;
}

TxIoMergeHandler::~TxIoMergeHandler()
{
	m_chains->clear();
	delete m_chains;
}

// @attention
//	you should avoid call setowner() multiple times. because the setowner() is
// actually addowner()
//
void TxIoMergeHandler::setowner( void * owner )
{
	addowner(owner);
}

bool TxIoMergeHandler::addowner( void * owner )
{
	m_chains->push_back( (TxIoFilterChain *)owner );
	return true;
}

void TxIoMergeHandler::connect( TxIoFilterChain * merged )
{
	m_merged = merged;
}

// assume:
//	input->owner, input->filterchain, and input->session have already assigned correct
// values.
int TxIoMergeHandler::rxhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )
{
	return m_merged->rxhandle(ses, input, output);
}

// assume:
//  input->owner, input->filterchain and input->session should have correct values
//
int TxIoMergeHandler::txhandle( TxIoSession * ses, TxIoBuffer * input, TxIoBuffer * output )
{
	std::vector<TxIoFilterChain *>::iterator itr;
	TxIoFilterChain * outputchain;
	uint outlen;

	outputchain = NULL;
	for (itr = m_chains->begin(); itr != m_chains->end(); itr++)
	{
		if (*itr == (void*)ses->getfilterchain())
		{
			outputchain = (TxIoFilterChain *)*itr;
			break;
		}
	}

	if (outputchain != NULL)
	{
		outlen = outputchain->txhandle(ses, input, output);
	}
	else
		outlen = 0;
	// else
	//	return default_handler(ses, input, output)

	return outlen;
}

