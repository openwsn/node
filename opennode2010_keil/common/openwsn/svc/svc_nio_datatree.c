#include "svc_configall.h"
#include <string.h>
#include "../hal/hal_mcu.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_cc2520.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_assert.h"
#include "svc_foundation.h"
#include "svc_nio_acceptor.h"
#include "svc_nio_mac.h"
#include "../osx/osx_tlsche.h"
#include "svc_nodebase.h"
#include "svc_nio_dispatcher.h"
#include "svc_nio_datatree.h"

/*******************************************************************************
 * @author Shi Zhirong in 2012.07.12
 *	- first created and need to test
 ******************************************************************************/

/******************************************************************************* 
 * obsolete content
 * Frame Format of DataTree Data Collection Protocol
 * 
 * [MAC Header nB] [DTP Section Length 1B] [DTP Identifier 1B] [DTP Protocol Control 1B] 
 * [Tree Id 1B] [Hopcount 1B] [Payload mB]
 * 
 * [DTP Section Length 1B]
 * Length of the DTP data. Not include the length byte itself.
 *
 * [DTP Identifier 1B]
 * Always 0x09. Indicate this is using Data Tree Data Collection Protocol. 
 *
 * [DTP Protocol Identifer 1B]
 * [DTP Command and Control 1B]
 * b1 b0  = 00 Broadcast 01 Multicast 10 Unicast 11 Response
 *		Broadcast is used to establish the tree and maintain the tree.
 *		Multicast will transmit the frame across a subtree. 
 *      Unicast will transmit the frame to the specified node.
 *      Response frame will always forwarded to the root node.
 *
 * [Tree Id 1B]
 * Actually, the entire tree Id should include the root node address. But the address
 * is already in MAC header. So we needn't to save a copy here. Now this value 
 * functions as the sequence number. 
 * 
 * [Hopcount 1B]
 * How many hops the frame is transmitted.
 *
 * DTP's Build Tree Frame 
 * [Length 1B] [DTP Identifier 1B] [DTP Protocol Control 1B] [Tree Id 1B] [HopCount 1B] 
 * Control = broadcast type
 *
 * DTP's Request Frame
 * [Length 1B] [DTP Identifier 1B] [DTP Protocol Control 1B] [Tree Id 1B] [Maximum Hopcount 1B] [User Data mB]
 * 
 * DTP's Response Frame
 * [Length 1B] [DTP Identifier 1B] [DTP Protocol Control 1B] [Tree Id 1B] [User Data mB]
 *
 ******************************************************************************/  

inline static uint8 _net_get_frame_feature( TiFrame * frame, char * feature, uint8 size );
static uint8 _dtp_broadcast( TiDataTreeNetwork * net, TiFrame * frame, uint8 option );
 
TiDataTreeNetwork * dtp_construct( void * mem, uint16 size )
{
	memset( mem, 0x00, size );
	hal_assert( sizeof(TiDataTreeNetwork) <= size );
	return (TiDataTreeNetwork *)(mem);
}

void dtp_destroy( TiDataTreeNetwork * net )
{
	dtp_close( net );
}

TiDataTreeNetwork * dtp_open_node( TiDataTreeNetwork * net, TiNioMac * mac, TiNodeBase * nbase,  TiNioNetLayerDispatcher *dispatcher, uint8 option )
{
	//net->state = DTP_STATE_STARTUP;
	net->state = DTP_STATE_IDLE;//for testing

	net->option = option;

	net->nbase = nbase;	
	//net->pan = CONFIG_ALOHA_DEFAULT_PANID;
	//net->localaddress = localaddress;
	net->root = 0x0000;
	net->parent = 0x0000;
	net->count = 0;//todo for testing
	net->depth = ~0;
	net->distance = ~0;
	net->mac = mac;
	net->dispatcher=dispatcher;
	//net->listener = listener;
	//net->lisowner = lisowner;
	//net->txtrytime = DTP_MAX_TX_TRYTIME;
    net->rssi = 0;
	net->cache = dtp_cache_open( (char *)( &net->cache_mem), DTP_CACHE_HOPESIZE );
	hal_assert( net->cache != NULL );
	net->request_id = 0;
	net->response_id = net->request_id;
	return net;
}
TiDataTreeNetwork * dtp_open_sink( TiDataTreeNetwork * net, TiNioMac * mac, TiNodeBase * nbase, TiNioNetLayerDispatcher *dispatcher, uint8 option )
{
	net->state = DTP_STATE_IDLE;
	net->option = option;
	//net->pan = CONFIG_ALOHA_DEFAULT_PANID;
	net->nbase = nbase;
	net->root = nbase->shortaddress;
	net->parent = 0x0000;
	net->count = 0;//todo for testing
	net->depth = ~0;
	net->distance = ~0;
	net->mac = mac;
	net->dispatcher=dispatcher;
	//net->listener = listener;
	//net->lisowner = lisowner;
	//net->txtrytime = DTP_MAX_TX_TRYTIME;
    net->rssi = 0;
	net->cache = dtp_cache_open( (char *)( &net->cache_mem), DTP_CACHE_HOPESIZE );
	hal_assert( net->cache != NULL );
	net->request_id = 0;
	net->response_id = net->request_id;
	return net;
}

void dtp_setlistener( TiDataTreeNetwork * net, TiFunEventHandler listener, void * object )
{
    net->listener = listener;
    net->lisowner = object;
}

void dtp_close( TiDataTreeNetwork * net )
{
	cache_close( net->cache );
}

/**
 * Summary the frame feature and place them into the feature buffer. The feature
 * includes: command and control(1B), sequence id(1B), destination address(2B)
 * and source address(2B).
 */ 
uint8 _net_get_frame_feature( TiFrame * frame, char * feature, uint8 size )
{
	memset( feature, 0x00, size );
	memmove( feature, (char*)frame_startptr( frame)+1, size );
	return size;
}


/*
 * dtp_evolve doesn't do anything, since dispatcher will send txbuf
 */
void dtp_evolve( void * object, TiEvent * e )
{
	return;
}

void dtp_maintain_evolve( void * object, TiEvent * e)
{
	TiDataTreeNetwork * net = (TiDataTreeNetwork *)object;
		
	char * request;
	char * pc;
    uint8 len, count;
    TiFrame * maintain_frame;

    //request = frame_startptr( f );
    char maintain_frame_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
	maintain_frame = frame_open( (char*)(&maintain_frame_memory), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 102 );
    frame_reset( maintain_frame,3,20,102);
    request = frame_startptr( maintain_frame);	
		
	DTP_SET_PROTOCAL_IDENTIFIER( request,DTP_PROTOCOL_IDENTIFER);
	DTP_SET_PACKETCONTROL( request, DTP_BROADCAST | DTP_MAINTAIN_REQUEST);
	DTP_SET_SEQUENCEID( request,net->request_id);
	DTP_SET_SHORTADDRTO( request, 0xffff );		
	DTP_SET_SHORTADDRFROM( request,net->nbase->shortaddress); 
	DTP_SET_HOPCOUNT( request,1);
	DTP_SET_MAX_HOPCOUNT( request,CONFIG_DTP_MAX_COUNT); //DTP_DEFAULT_MAX_HOPCOUNT
	DTP_SET_PATHDESC_COUNT( request,0);
	
	frame_setlength( maintain_frame, 20 );
	pc = DTP_PATHDESC_PTR(request) + ((DTP_HOPCOUNT(request)-1) << 1);
	pc[1] = ( net->nbase->shortaddress ) & 0xFF;
	pc[2] = ( net->nbase->shortaddress ) >> 8;	
	DTP_SET_PATHDESC_COUNT( request,1 );
	_net_get_frame_feature( maintain_frame, &(net->frame_feature[0]), sizeof(net->frame_feature) ); //todo 
	dtp_cache_visit( net->cache, &(net->frame_feature[0]) );

	count = 0;
    while (count < 0x04)
    {   
		//len=aloha_send(net->mac,0xFFFF,maintain_frame,0x00);
		len = mac_broadcast( net->mac, maintain_frame, 0x00 );
		if (len > 0)
		{  
			net->request_id ++;
			//hal_delayms( 100 );
			break;
		}
		//hal_delayms( 100 );
		count ++;
	}
	osx_taskspawn( dtp_maintain_evolve, net, DTP_MAINTAIN_TIME, 0, 0);
	return; 
}

intx nio_dtp_rxhandler_node( void * object, TiFrame * input, TiFrame * output, uint8 option )
{
	TiDataTreeNetwork * net = (TiDataTreeNetwork *)object;
    char * pc;
	char * pkt;
	uint8 rssi;
	uint8 depth,ret;
	uint8 cur_hopcount, max_hopcount;
	//uint8 count;
	uint8 legth=frame_length( input );
	uint16 addr;
	uint8 hopcount;
	pkt=frame_startptr(input);
	switch (net->state)
	{
	case DTP_STATE_STARTUP:
		//DTP_BROADCAST DTP_MAINTAIN_REQUEST
		if(!frame_empty(input))
		{
			//legth = frame_length( input );
			if( DTP_PACKETCONTROL(pkt) == DTP_BROADCAST | DTP_MAINTAIN_REQUEST )
			{
				cur_hopcount = DTP_HOPCOUNT( pkt );
				if (net->depth > cur_hopcount)
				{
					net->depth = cur_hopcount;
					net->root = DTP_SHORTADDRFROM( pkt );
					pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo
					net->parent = DTP_MAKEWORD( pc[2],pc[1]);//todo  父亲节点为上一节点的网络地址
					ret = true;
				}
				else
				{
					ret = false;
				}
				net->response_id = DTP_SEQUENCEID( pkt );//?????????????????????? response_id??????
				max_hopcount = DTP_MAX_HOPCOUNT( pkt );
				if (cur_hopcount >= max_hopcount)
				{   
					frame_totalclear( input );
					//break;
				}
				else
				{
					if(!frame_empty(input))
					{  
						cur_hopcount = DTP_HOPCOUNT( pkt );
						DTP_SET_HOPCOUNT( pkt, ++cur_hopcount );
						pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);
						pc[1] = ( net->nbase->shortaddress ) & 0xFF;
						pc[2] = ( net->nbase->shortaddress ) >> 8;
						DTP_PATHDESC_COUNT( pkt )++;
						frame_totalcopyfrom(output,input);
						frame_totalclear(input);
						output->address=0xffff;
						output->option=0;
					}
					net->state=DTP_STATE_IDLE;	
				}		
			}
			else
			{
				frame_totalclear(input);
			}
		}
		break;
													
	case DTP_STATE_IDLE:
		if(!frame_empty(input))
		{
				//break;
			pkt=frame_startptr(input);
			legth = frame_length( input );
			//count = frame_length( input );
			switch (DTP_TRANTYPE(pkt))
			{
			case DTP_BROADCAST:
				switch (DTP_CMDTYPE(pkt))
				{
				case DTP_MAINTAIN_REQUEST:
					frame_moveouter( input );//todo for testing
					rssi = pkt[(frame_capacity( input)-14)];//todo for testing
					depth = cur_hopcount = DTP_HOPCOUNT( pkt );//todo for testing
					depth--;//todo for testing
			
					if ( (rssi >= net->rssi)&&(depth < net->depth))//todo for testing
					{
						net->rssi = rssi;//todo for testing
						net->depth = cur_hopcount;//这一句是不是应该写成呢他net->depth = depth?
						net->root = DTP_SHORTADDRFROM( pkt );
						pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo
						net->parent = DTP_MAKEWORD( pc[2],pc[1]);//todo  父亲节点为上一节点的网络地址
					}
					else if ( (( rssi - net->rssi) > 4)&&( depth == net->depth))//todo for testing
					{
						net->rssi = rssi;//todo for testing
						net->depth = cur_hopcount;
						net->root = DTP_SHORTADDRFROM( pkt );
						pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo
						net->parent = DTP_MAKEWORD( pc[2],pc[1]);//todo  父亲节点为上一节点的网络地址
					}
					else
					{
					}
				
					frame_moveinner( input );//todo for testing
					frame_setlength( input ,legth);//todo for testing
	
					net->response_id = DTP_SEQUENCEID( pkt );
	
					// before try to forward the packet, we firstly check whether it has 
					// already reaches its maximum hopcount. the packet will be discarded
					// if this is true.
					//
					max_hopcount = DTP_MAX_HOPCOUNT( pkt );
					if (cur_hopcount >= max_hopcount)
					{
						frame_totalclear( input );
						//break;
					}
					else
					{
						if (!frame_empty( input ) )
						{
							cur_hopcount = DTP_HOPCOUNT( pkt );
							//DTP_SET_HOPCOUNT( pkt, ++cur_hopcount );
							//为了阻止后面的节点成为前面的节点的父亲节点，用下面语句替换上面的语句。
							if ( cur_hopcount < net->depth)//todo for testing
							{
								hopcount = net->depth;
								DTP_SET_HOPCOUNT( pkt,++hopcount);
							} 
							else
							{
								DTP_SET_HOPCOUNT( pkt, ++cur_hopcount );
							}
							// save this node address into the packet's path section
							pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);
							pc[1] = (net->nbase->shortaddress ) & 0xFF;
							pc[2] = (net->nbase->shortaddress ) >> 8;
							DTP_PATHDESC_COUNT( pkt)++;
		
							frame_totalcopyfrom( output ,input );
							frame_totalclear( input );
							output->address=0xFFFF;
							output->option=0x00;
						}
					}
					break;
				case DTP_DATA_REQUEST:
					net->response_id = DTP_SEQUENCEID( pkt );
					cur_hopcount = DTP_HOPCOUNT( pkt );
					max_hopcount = DTP_MAX_HOPCOUNT( pkt );
					if (cur_hopcount >= max_hopcount)
					{
						frame_totalclear( input );
						//break;
					}
					else
					{
						//exchange input and ouput temporarily
						frame_totalcopyfrom(output,input);
						cur_hopcount=DTP_HOPCOUNT(pkt);
						DTP_SET_HOPCOUNT(pkt,++cur_hopcount);
						pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);
						pc[1] = (net->nbase->shortaddress ) & 0xFF;
						pc[2] = (net->nbase->shortaddress ) >> 8;
		
						DTP_PATHDESC_COUNT(pkt)++;
		
						frame_totalcopyfrom(output,input);
		
						output->address=0xffff;
						output->option=0x00;
						frame_skipinner( input,  DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )),0 );//todo
						frame_setlength( input, legth-DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )));//todo
					}
					break;
				default://unused actually
					if(DTP_SHORTADDRTO(pkt)==net->nbase->shortaddress)
					{
						//do nothing, input will send it to app
						frame_skipinner( input,  DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )),0 );//todo
						frame_setlength( input, legth-DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )));//todo
					}				
					else
					{
						frame_totalclear(input);
					}
				}
				break;
			}
			//break;//0719
			
		case DTP_UNICAST_LEAF2ROOT:
			addr=DTP_SHORTADDRTO(pkt);
			if(addr==net->nbase->shortaddress)
			{
				//do nothing, input will send it to app
				frame_skipinner( input, DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )),0);//todo
				frame_setlength( input, legth-DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )));//todo
			}
			else
			{
				cur_hopcount = DTP_HOPCOUNT( pkt );
				max_hopcount = DTP_MAX_HOPCOUNT( pkt );
				if (cur_hopcount >= max_hopcount+1)
				{
					frame_totalclear( input );
				}
				else if (frame_empty(output))
				{   
					cur_hopcount ++;
					DTP_SET_HOPCOUNT( pkt, cur_hopcount );

					pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo for testing
					pc[1] = (net->nbase->shortaddress ) & 0xFF;//todo for testing
					pc[2] = (net->nbase->shortaddress ) >> 8;//todo for testing
					DTP_PATHDESC_COUNT(pkt)++;//todo for testing
					
					frame_totalcopyfrom(output,input);
					output->address=net->parent;
					output->option=0x01;
					frame_totalclear(input);//todo 0713
				}
				else
				{
				}
			}
			break;
		
		default:
			frame_totalclear(input);//clear
			break;
		}
				
	}
	return 1;//todo 0713
}

intx nio_dtp_rxhandler_sink( void * object, TiFrame * input, TiFrame * output, uint8 option )
{
	TiDataTreeNetwork * net = (TiDataTreeNetwork *)object;
    char * pkt;
	uint8 legth;
	pkt=frame_startptr(input);
	legth = frame_length( input );
	switch (DTP_TRANTYPE(pkt))
	{
	case DTP_BROADCAST:
		switch (DTP_CMDTYPE(pkt))
		{
		case DTP_MAINTAIN_REQUEST:
		case DTP_DATA_REQUEST: 
			frame_totalclear(input);
			break;
		case DTP_DATA_RESPONSE:
			if(DTP_SHORTADDRTO(pkt)==net->nbase->shortaddress)
			{
				//do nothing, input will send it to app
				frame_skipinner( input,  DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )),0 );//todo
				frame_setlength( input, legth-DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )));//todo
			}
			else
			{
				frame_totalclear(input);
			}
			break;
		}
		
	case DTP_UNICAST_LEAF2ROOT:
		if(DTP_SHORTADDRTO(pkt)==net->nbase->shortaddress)
		{
			//do nothing, input will send it to app
			frame_skipinner( input, DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )),0 );//todo
			frame_setlength( input, legth-DTP_HEADER_SIZE(DTP_MAX_HOPCOUNT( pkt )));//todo
		}
		else
		{
			frame_totalclear(input);
		}
		break;
	
	default:
		//clear
		frame_totalclear(input);
		break;
	}
	return 1;//todo 0713
}

intx nio_dtp_txhandler_node( void * object, TiFrame * input, TiFrame * output, uint8 option )
{
	intx retval;
	TiDataTreeNetwork * net = (TiDataTreeNetwork *)object;
    char * pkt;
	char * pc;
	uint8 legth,cont;

	retval=-1;//todo
	switch (net->state)
	{
	case DTP_STATE_STARTUP:
		//error: cannot do anything
		frame_totalclear( input );
		retval=-1;
		break;
		
	case DTP_STATE_IDLE:
		if( option == DTP_UNICAST_LEAF2ROOT | DTP_DATA_RESPONSE)
		{
			if ( !frame_empty( input ))
			{	
				cont = 0;
				legth = frame_length( input);
				
				if (frame_empty(output)) 
				{ 
					cont = frame_totalcopyfrom( output, input );
					output->option = 0x00;//no ack
					//frame_skipouter( output, 10, 0 );// 10???  
					frame_skipouter( output,DTP_HEADER_SIZE(CONFIG_DTP_MAX_COUNT),0);//todo
					pkt = frame_startptr(output );
					DTP_SET_PROTOCAL_IDENTIFIER( pkt,DTP_PROTOCOL_IDENTIFER);
					DTP_SET_SEQUENCEID(pkt,net->response_id);	 //JOE 0717
					DTP_SET_PACKETCONTROL( pkt, option );
					DTP_SET_SHORTADDRTO( pkt, net->root );
					DTP_SET_SHORTADDRFROM( pkt, net->nbase->shortaddress );
					DTP_SET_HOPCOUNT( pkt, 0 );
					if (DTP_MAX_HOPCOUNT( pkt) == 0)
					{
						DTP_SET_MAX_HOPCOUNT( pkt, CONFIG_DTP_MAX_COUNT );	// DTP_DEFAULT_MAX_HOPCOUNT
					}
					DTP_SET_PATHDESC_COUNT( pkt, 0 );

					//todo 0717
					//cur_hopcount ++;
					DTP_SET_HOPCOUNT( pkt, 1 );//cur_hopcount

					pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);//todo for testing
					pc[1] = (net->nbase->shortaddress ) & 0xFF;//todo for testing
					pc[2] = (net->nbase->shortaddress ) >> 8;//todo for testing
					DTP_PATHDESC_COUNT(pkt)++;//todo for testing
					
					//frame_setlength( output,(i+10));	
					frame_setlength( output, legth+DTP_HEADER_SIZE(CONFIG_DTP_MAX_COUNT) );//todo
					
////////////////					if (aloha_send( net->mac,net->parent, output, 0x01 ) > 0)//有问题 0717 address————done
////////////////					{
////////////////						frame_totalclear( output );
////////////////					//	net->response_id++;
////////////////					}
////////////////					else{
////////////////						output->address=net->parent;
////////////////						output->option=0x01;
////////////////					//	net->request_id++;//dispatch不负责，只有这里先加了吧？
////////////////						//count=1;//todo
////////////////					}
					
					output->address=net->parent;
					output->option=0x01;

					net->request_id++;
				}
			}
			frame_totalclear( input );
			retval=1;
		}
		else
		{
			//error
			frame_totalclear( input );
			retval=-1;
		}
		break;
	}
	return retval;
}

intx nio_dtp_txhandler_sink( void * object, TiFrame * input, TiFrame * output, uint8 option )
{
	TiDataTreeNetwork * net = (TiDataTreeNetwork *)object;
    uint8 cont;
	uint8 legth;
	uint8 len;
	intx retval;
	char * pc; 
   
	retval=-1;//todo

	if( option == DTP_BROADCAST | DTP_DATA_REQUEST )
	{
		if ( !frame_empty( input ))
		{	
			cont = 0;
			legth = frame_length( input);
			
			if (frame_empty(output)) 
			{ 
				cont = frame_totalcopyfrom( output, input );
				output->option = 0x00;//no ack
				//frame_skipouter( output, 10, 0 );
				frame_skipouter( output, DTP_HEADER_SIZE(CONFIG_DTP_MAX_COUNT), 0 );//todo
				pc = frame_startptr(output );
				DTP_SET_PROTOCAL_IDENTIFIER( pc,DTP_PROTOCOL_IDENTIFER);
				DTP_SET_PACKETCONTROL( pc, option );
				DTP_SET_SEQUENCEID( pc, net->request_id );
				DTP_SET_HOPCOUNT( pc,0);
				DTP_SET_MAX_HOPCOUNT( pc, CONFIG_DTP_MAX_COUNT );//	 DTP_DEFAULT_MAX_HOPCOUNT
				//frame_setlength( output,(i+10));	
				frame_setlength( output, legth +DTP_HEADER_SIZE(CONFIG_DTP_MAX_COUNT) );//todo
				
				len = _dtp_broadcast( net, output, 0x00 );//dtp_broadcast需要修改
				if (len > 0)
				{
					net->request_id ++;////////////
					frame_totalclear( output );
				}
				else
				{
					output->address = 0xffff;
					output->option=0x00;
					net->request_id++;//dispatch不负责，只有这里先加了吧？
				}
				retval=1;
			}
			frame_totalclear( input );
		}
	}
	else
	{
		//error
		frame_totalclear( input );
		retval=-1;//todo
	}
	return retval;
}


uint8 _dtp_broadcast( TiDataTreeNetwork * net, TiFrame * frame, uint8 option )
{ 
	
	uint8 retval=0;
	char * pkt;
	char * pc;
	
	hal_assert( frame_length(frame) > 0 );

	//count = frame_totalcopyfrom( net->txque, frame ); 
	//frame_setlength( net->txque,frame_length( frame));//todo
	pkt = frame_startptr( frame );

	//DTP_SET_PROTOCAL_IDENTIFIER( pkt, DTP_PROTOCOL_IDENTIFER);
	//DTP_PACKETCONTROL( pkt) |= DTP_BROADCAST;
	DTP_SET_HOPCOUNT( pkt,DTP_HOPCOUNT(pkt) + 1 );
	
	pc = DTP_PATHDESC_PTR(pkt) + ((DTP_HOPCOUNT(pkt)-1) << 1);
	pc[1] = (net->nbase->shortaddress ) & 0xFF;
	pc[2] = ( net->nbase->shortaddress ) >> 8;
	DTP_SET_PATHDESC_COUNT( pkt,DTP_PATHDESC_COUNT(pkt) + 1 );

	 _net_get_frame_feature( frame, &(net->frame_feature[0]), sizeof(net->frame_feature) ); //todo 
	dtp_cache_visit( net->cache, &(net->frame_feature[0]) );

//	if(aloha_broadcast(net->mac,frame,0x00)>0)	 //JOE 0718	 因为不需要自己发送
//	{
//		retval=1;
//	}
//	else
//	{
//		retval=0;
//	}
	retval=0;
   	return retval;			//JOE 0718 让调用者自己发送，返回0
}




/*
void dtp_test_recv_evolve( void * object, TiEvent * e)	 //for recv
{
	TiDataTreeNetwork * net = (TiDataTreeNetwork *)object;
    TiFrame * maintain_frame;
	char * ptr;
	char legth=0;
	uint16 addr;	
    char maintain_frame_memory[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
	maintain_frame = frame_open( (char*)(&maintain_frame_memory), FRAME_HOPESIZE( MAX_IEEE802FRAME154_SIZE ), 3, 20, 102 );
    frame_reset( maintain_frame,3,20,102);

	if(	nio_dispa_recv(net->dispatcher,&addr,maintain_frame,0x00)>0)	
	{
		ptr=frame_startptr( maintain_frame);
		legth=frame_length( maintain_frame);
		if(ptr[0]==0)
		{
			led_toggle(LED_RED);
		}
	}
	//osx_tlsche_taskspawn(net->scheduler, dtp_test_recv_evolve, net,1,0,0); //just for testing
	return; 
}
*/