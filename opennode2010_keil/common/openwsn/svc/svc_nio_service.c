

/*
 * Reference 
 * - Apache MINA's I/O Service, http://mina.apache.org/chapter-3-service.html
 * 
 * 		A MINA Service is the base class supporting all the IO services, either 
 *		from the server side or the client side. It's an interface, which is implemented 
 *		as an IoAcceptor for the server side, and IoConnector for the client side.
 * 		A service not only processes incoming or outgoing messages, it also manage 
 *		connections, sessions, and calls the application.
 *
 * - 顾锋磊, 软件工程师, IBM, 使用事件驱动模型实现高效稳定的网络服务器程序--几
 *   种网络服务器模型的介绍与比较 , 2010.10, 
 *   http://www.ibm.com/developerworks/cn/linux/l-cn-edntwk/index.html?ca=drs-
 * 
 * - libev architecture
 */
 


TiNioService * nvc_open( TiNioService * nvc, TiNioAcceptor * nac, TiNioDispatcher * ndis )
{
	nvc->acceptor = nac;
	nvc->dispatcher = ndis;
	nvc->rwque = rwque;
	return nvc;
}

void nvc_close( TiNioService * nvc )
{
	return;
}

/**
 * Initialize the network. The following function is only for demonstration. The developer
 * can fully initialize the network by calling TiNioService interface functions.
 */
void nvc_init( TiNioService * nvc )
{
	TiNioDispatcher * ndis = nvc->dispatcher;

	replace functions with nio_register( nio, level, id, object );
	
	nio_dispa_attachroot( ndis, nvc->acceptor );
	nio_dispa_attachchild( ndis, 0, mac );
	nio_dispa_attachsibling( ndis, 1, nanosync );
	nio_dispa_first( ndis );
	nio_dispa_attachchild( ndis, 0, net );
	nio_dispa_attachsibling( ndis, 1, neighbor_discovery );
	nio_dispa_attachsibling( ndis, 2, topo );
	nio_dispa_attachsibling( ndis, 3, localization );
	nio_dispa_attachsibling( ndis, 4, timesync );
	nio_dispa_first( ndis );
	nio_dispa_attachchild( ndis, endpoint1, addon1 );
	nio_dispa_attachchild( ndis, endpoint2, addon2 );
	nio_dispa_attachchild( ndis, endpoint3, addon3 );
	nio_dispa_attachchild( ndis, endpoint4, addon4 );
}

void nvc_register( TiNioService * nvc, uint8 level, uint8 id, void * object ) 
{
	nio_dispa_register( nvc->dispatcher, level, id, object );
}

void nio_service_evolve( TiNioService * nvc )
{
	acceptor.evolve()
	treedispatcher.execute( session ) = dispatch + inversedispatch;
	or 
	dispatcher3.execute( session ) = dispatch + inversedispatch;
	dispatcher2.execute( session ) = dispatch + inversedispatch;
	dispatcher1.execute( session ) = dispa tch + inversedispatch;
	
	if frame received in rxque
		dispatcher.dispatch
	if frame in txque 
		dispatcher.inversedispatch. however, you must know the level and id of this frame
}

nvc_evolve()
{
	TiFrame * rxbuf;
	 
	dispatcher = nios->raw_fmdept;
	rxbuf = nac_rxque_front( nios->acceptor );
	if (rxbuf != NULL)
	{
		for node in the path 
		{
			ret = treenode->object->process(session)
			if (ret == 0)
			{
				popfront
				break
			}
			else
				next node processing
			else 
				error
				popfront
				break;
			endif
		}
		
		
		frame_dispatcher_execute( fmdept );
		if rxque_front_unchange
			continue
		else
			break
	}
	
	if reach the leaf node of the tree and the frame is still pending
		call default handler to process the frame, which will remove the item from the rx queue
	endif 
}

/**
 * @brief Check for the next incoming frame inside TiNioService object and place 
 * it into frame buffer.
 * 
 * @param frame Where the received frame will be placed
 * @return the data length inside the frame object
 */
uint8 nvc_recv( TiNioService * nvc, TiFrame * frame, uint8 option )
{
	TiFrameQueue * rxque;
	
	nvc_evolve( nvc, NULL );
	rxque = nac_rxque( nvc->acceptor );
	if (!fmque_empty(rxque))
	{
		frame_totalcopyfrom( frame, fmque_front(rxque) );
		fmque_popfront( rxque );
		len = frame_length(frame);
	}
	else
		len = 0;
	
	return len;
}

uint8 nvc_send( TiNioService * nvc, TiFrame * frame, uint8 option )
{
	TiFrameQueue * txque;
	TiFrame * cur;
	
	txque = nac_txque( nvc->acceptor );
	if (!fmque_full(txque))
	{
		fmque_pushback( txque, frame );
		cur = fmque_rear( txqu );
		
		nvc_encapsulate
		
		nvc_evolve( nvc, NULL );
		len = frame_length(frame);
	}
	else
		len = 0;
	
	
	return len;
	
	txque = dispatcher txque
	pushback( txque, frame );
	
	while txque->front frame isn''t complete
		dispatcher.inverseprocess / gather 
	
}



rx_process
{
	get the first frame from the rxque
	if this frame should be service.read out, then just exit and wait for some to call read
	
	get the dispatch id and generate the session structure
	call dispatch.execute to the session
	if cannot find dispatch id, then simply delete the frame. our node cannot recognize it but delete it
	
	while the frame is still inside rxque, then repeat do it
	until it should be deleted or reach the outmost layer
}

tx_process = inverse_process
{
	pushback 1 or more frame into txque, this is done by the component itself
	for each frame do 
		get the dispatch id of it. how do i know it?
			assme every component must know the dispatcher id{layer, protocol identifier}
			this is a fixed value when initialziing the nio service. ok
		find it'' parent in the tree or through the dispatcher'' source object property
		call source object txprocess to process it
		find parent
		call source object txprocess to process it 
		until the source object is acceptor
}

init
	register components into dispatcher

