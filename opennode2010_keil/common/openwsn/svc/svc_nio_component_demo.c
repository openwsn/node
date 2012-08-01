TiDemo * demo_construct( void * mem, uint16 size )
{
	memset( mem, 0x00, size );
	hal_assert( sizeof(TiDemo) <= size );
	return (TiDemo *)(mem);
}

void demo_destroy( TiDemo * net )
{
	//close
	return;
}


TiDemo * demo_open( TiDemo * net, TiNioMac * mac, TiNodeBase * nbase, 	TiOsxTimeLineScheduler * scheduler,
    TiNioNetLayerDispatcher *dispatcher, uint8 option )
{
	net->mac = mac;
	net->scheduler = scheduler;
	net->dispatcher = dispatcher;
	//other setting
	
	return net;
}

intx nio_demo_rxhandler( void * object, TiFrame * input, TiFrame * output, uint8 option )
{
    intx retval;
    TiDemo * net=(TiDemo *)object;
    char * pkt;
    uint8 legth;
    pkt=frame_startptr(input);
    legth = frame_length( input );    

    /*process
	
	
	process*/
	
	//if there is no frame to be sent to the upper layer
    frame_totalclear(input);
    //else set the frame and it will be sent in the upper layer
    frame_skipinner( input, HEADLENGTH,0 );
    frame_setlength( input, legth- HEADLENGTH );

    //if there ia a frame need to be sent in the same layer
	//do nothing
	//else set the frame's addr and ack option and it will be sent in the upper layer
    output->address=0xXXXX£»
    output->option=0x01/0x00;

    return retval£»
}

intx nio_demo_txhandler( void * object, TiFrame * input, TiFrame * output, uint8 option )
{
    intx retval;
    TiDemo * net=(TiDemo *)object;
    char * pc; 
    uint16 addr = input->address;
    frame_totalcopyfrom( output, input );
    frame_skipouter( output, HEADLENGTH ,0);
    pc = frame_startptr( output );

    /*process
	
	
	process*/
    
    frame_totalclear(input);
    output->address=0xXXXX£»
    output->option=0x01/0x00;
    return retval£»
}

void demo_task_evolve( void * object, TiEvent * e)
{
    TiDemo * net=(TiDemo *)object;
    char * pc;
    TiFrame * temp_frame;
    char temp_frame_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

    temp_frame = frame_open( (char*)(& temp_frame_memory ), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ),3,20, 102 );
    frame_reset( temp_frame ,3,20,102);
    pc = frame_startptr( temp_frame );	

    /*process
	
	
	process*/

    osx_tlsche_taskspawn(net->scheduler,  demo_task_evolve , net, TASK_TIME , 0, 0);
}

//non-default call
intx demo_send(TiXXX * net, TiFrame f, uint16 addr, uint8 option)
{
    int retval;
    if( frame_empty(net->dispatcher->txbuf) )
    {
		retval = nio_demo_txhandler((void *)net, f, net->dispatcher->txbuf, 0x00);       
    }
    else
    {    
        retval = 0;
    }
    return retval;
}


