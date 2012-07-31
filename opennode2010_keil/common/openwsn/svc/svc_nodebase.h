/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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
 
#ifndef _SVC_NODEBASE_H_7643_
#define _SVC_NODEBASE_H_7643_

/**
 * Store the nodes descriptions(including the current node itself, and neighbor nodes.
 * and network topologies found.
 */
 
#include "svc_configall.h"
#include "svc_foundation.h"

#ifndef CONFIG_NBASE_NODE_CAPACITY 
#define CONFIG_NBASE_NODE_CAPACITY 4
#endif

/** 
 * This value should larger than CONFIG_NBASE_NODE_CAPACITY.
 */
#ifndef CONFIG_NBASE_RELATION_CAPACITY
#define CONFIG_NBASE_RELATION_CAPACITY CONFIG_NBASE_NODE_CAPACITY
#endif

#define NBASE_MARK_DELETED(state) ((state) &= 0xFE)
#define NBASE_MARK_ACTIVE(state) ((state) |= 0x01);

#define NBASE_STATE_FREE    0
#define NBASE_STATE_ACTIVE  1

#pragma pack(1)  
typedef struct{
    uint8   state;
    uint16  pan;
    uint16  address;
    uint8   weight;
    uint8   tag;
    uint8   lifetime;
    uint8   rssi;
    //uint8   reliable;
    //uint8   trustable;
}TiNodeDescriptor;

#pragma pack(1) 
typedef struct{
    uint8   state;
    uint8   addrfrom;
    uint8   addrto;
    uint16  rssi;
    uint8   weight;
    // uint8 option;
    uint16 scale_factor; // for time synchronization
    uint16 shift_factor; // for time synchronization
}TiRelationDescriptor;

/**
 * The database containing all environment settings in the current node.
 * 
 * TiNodeBase.nodes[0] saves always the current node description data.
 * 
 * sio_datarate: For serial USART such as RS232/RS485, this value equals bandrate.
 * sleep period = sleep duation + sleep interval
 */
#pragma pack(1) 
typedef struct{
    uint8 state;
    uint8 version;
    uint16 pan;
    uint16 shortaddress;
    uint64 longaddress;
    uint8 weight;
    uint8 capability;
    uint8 nio_channel;
    uint8 nio_txpower;
    uint8 nio_backoff_rule;
    uint8 nio_backoff_interval;
    uint32 uart_datarate; 
    uint8 uart_databits;
    uint8 uart_stopsbits;
    uint8 uart_parity;
    uint16 sleeplast;
    uint16 sleepduration;
    uint16 sleepinterval;
    TiNodeDescriptor nodes[CONFIG_NBASE_NODE_CAPACITY];
    TiRelationDescriptor relations[CONFIG_NBASE_RELATION_CAPACITY];
}TiNodeBase;

/**
 * Construct an TiNodeBase object on specified memory block.
 */
TiNodeBase * nbase_construct(void * mem, uint16 memsize);
//void nbase_construct(TiNodeBase * nbase);

TiNodeBase * nbase_open( TiNodeBase * nbase, uint16 pan,uint16 addr,uint8 channel)
;
void nbase_close( TiNodeBase * nbase );

void nbase_clear( TiNodeBase * nbase );

bool nbase_save( TiNodeBase * nbase );
bool nbase_load( TiNodeBase * nbase );

inline uint16 nbase_get_pan(TiNodeBase * nbase) {return nbase->pan;};
inline uint16 nbase_get_shortaddress(TiNodeBase * nbase) {return nbase->shortaddress;};
inline uint8 nbase_get_channel(TiNodeBase * nbase) {return nbase->nio_channel;};
    
/**
 * Returns the memory pointer to the first TiNodeDescriptor item in the database
 * node table. It's the same as _nbase_getnodeptr(nbase,0).
 */
TiNodeDescriptor * nbase_getnodetable( TiNodeBase * nbase );

TiNodeDescriptor * nbase_getnodetptr(TiNodeBase * nbase, uint16 address);

TiRelationDescriptor * nbase_getrelationtable( TiNodeBase * nbase );

TiRelationDescriptor * nbase_getrelationptr(TiNodeBase * nbase, uint16 addrfrom, uint16 addrto);

bool nbase_getnodebyaddress(TiNodeBase * nbase, uint16 address, TiNodeDescriptor * node);

bool nbase_getrelationbyaddress( TiNodeBase * nbase, uint16 addrfrom, uint16 addrto, TiRelationDescriptor * rel );

/** Put one node descriptor into database */
TiNodeDescriptor * nbase_setnode( TiNodeBase * nbase, uint16 address, TiNodeDescriptor * node );

/** Put one relation descriptor into database */
TiRelationDescriptor * nbase_setrelation( TiNodeBase * nbase, uint16 addrfrom, uint16 addrto, TiRelationDescriptor * relation );

/**
 * Return all index of TiNodeDescriptors related to specified node. You can use 
 * nbase_getnodeptr(nbase,idx) to get the memory pointer to the TiNodeDescriptor
 * item in the database for fast access.
 */
uint8 nbase_getnodeneighbors( TiNodeBase * nbase, uint16 address, TiNodeDescriptor ** nodeptr, uint8 capacity );

/** 
 * Returns all index of TiRelationDescriptors related to specified node. You can
 * use nbase_getrelationptr(nbase,idx) to get the memory pointer to that TiRelationDescriptor 
 * for fast access.
 */
uint8 nbase_getnoderelations( TiNodeBase * nbase, uint16 address, TiRelationDescriptor ** relptr, uint8 capacity );

bool nbase_deletenode( TiNodeBase * nbase, uint16 address );

bool nbase_deleterelation( TiNodeBase * nbase, uint16 addrfrom, uint16 addrto );


#endif /* _SVC_NODEBASE_H_7643_ */
